#include "GameCore/GameLoop.h"
#include "GameCore/SceneManager.h"
#include "GamePlay/GameObject.h"
#include "Player.h"
#include "SaveManager.h"
#include "HelpTool/INIConfig.h"
#include "Axe.h"
#include "Axe_Dead.h"
#include "AxePart.h"
#include "Zombi.h"
#include "Zombi_Dead.h"
#include "ZombiPart.h"
#include "StartScene.h"
#include "GameCore/Application.h"
#include "D2DCore/D2DRender.h"
#include "D2DCore/TextureManager.h"
#include "GameUI/UIFloatingNumber.h"
#include "Fly.h"
#include "Fly_Sword.h"
#include <cmath>
#include <filesystem>
#include <fstream>
#include <cstdio>
#include <cstdlib>

static void Check(bool condition, const char* message)
{
    if (!condition) { std::fprintf(stderr, "FAIL: %s\n", message); std::exit(1); }
}

static void Step(float deltaTime)
{
    GameLooper.UpdateFrame(deltaTime);
}

class DeferredResource : public GamePlay::GameObject
{
public:
    inline static int ready = 0;
    void Start() override { ++ready; }
};

class TrackedPlayer : public Player
{
public:
    inline static int initialized = 0, deleted = 0, updates = 0;
    void Awake() override
    {
        ++initialized;
        CreateGameObject<DeferredResource>();
    }
    void Update(float) override { ++updates; }
    ~TrackedPlayer() override { ++deleted; }
};

// Reproduce the selection page's pending-player ownership contract without assets or saves.
class Selection : public GameCore::Scene
{
public:
    bool start = false;
    void Update(float) override
    {
        if (!start) return;
        start = false;
        Player::SetMainPlayer(GamePlay::GameObject::CreateGameObject<TrackedPlayer>());
        SceneMgr.SwitchTo("room");
    }
    void OnExit() override { GameObjectMgr.ClearAllObjects({}, false); }
};

class Room : public GameCore::Scene
{
public:
    int entries = 0;
    int updates = 0, keys = 0;
    void Update(float) override { ++updates; }
    void OnKeyDown(WPARAM) override { ++keys; }
    GamePlay::SpriteRender* sprite = nullptr;
    void OnEnter() override
    {
        Check(MainPlayer != nullptr, "player survives selection exit");
        Check(TrackedPlayer::deleted == 0, "pending player was not deleted");
        ++entries;
        if (!sprite)
        {
            GamePlay::GameObject::CreateGameObject<GamePlay::GameObject>()->AddComponent<GamePlay::Camera>();
            auto* visual = GamePlay::GameObject::CreateGameObject<GamePlay::GameObject>();
            visual->transform().SetPosition({1000, 500});
            sprite = visual->AddComponent<GamePlay::SpriteRender>();
            sprite->SetSrcRect(0, 0, 20, 40);
        }
    }
};

static void TestSavePreview()
{
    namespace fs = std::filesystem;
    const auto originalDirectory = fs::current_path();
    const auto scratch = fs::absolute(originalDirectory / "../artifacts" /
        ("save-preview-" + std::to_string(GetCurrentProcessId()) + "-" + std::to_string(GetTickCount64())));
    Check(fs::create_directory(scratch), "create isolated save preview directory");
    fs::create_directory(scratch / "save");
    fs::current_path(scratch);
    Check(SaveMgr.GetSaveTime(-1).empty() && SaveMgr.GetSaveTime(3).empty(), "invalid slots have no preview");
    Check(!SaveMgr.HasSave(0) && SaveMgr.GetSaveTime(0).empty(), "empty slot has no time");
    { std::ofstream file("save/slot0.ini"); file << "[meta]\nversion=1\n"; }
    Check(SaveMgr.GetSaveTime(0).size() == 19, "legacy save falls back to file modification time");
    { std::ofstream file("save/slot1.ini"); file << "[meta]\nversion=1\nsaved_at=2026-10-31 14:30:00\n"; }
    Check(SaveMgr.GetSaveTime(1) == "2026-10-31 14:30:00", "preview reads saved timestamp");
    Check(SaveMgr.CopySave(1, 2) && SaveMgr.GetSaveTime(2) == SaveMgr.GetSaveTime(1), "copy preserves save timestamp");
    Check(SaveMgr.DeleteSave(2) && SaveMgr.GetSaveTime(2).empty(), "deleted slot clears timestamp");
    {
        Player player;
        Player::SetMainPlayer(&player);
        Check(SaveMgr.SaveGame(0), "save succeeds with timestamp metadata");
        HelpTool::INIConfig ini;
        Check(ini.ParseINI("save/slot0.ini") && ini.HasKey("meta", "saved_at"), "saved timestamp is persisted");
        Check(SaveMgr.GetSaveTime(0) == static_cast<std::string>(ini.Value("meta", "saved_at")), "preview matches saved metadata");
        Check(MainPlayer == &player, "preview does not replace player");
    }
    fs::current_path(originalDirectory);
    fs::remove(scratch / "save/slot0.ini");
    fs::remove(scratch / "save/slot1.ini");
    fs::remove(scratch / "save");
    fs::remove(scratch);
}

template<class EnemyType, class DeathState, class PartType>
static void CheckCorpseDirection(float initialPlayerX, int expectedParts)
{
    Player player;
    Player::SetMainPlayer(&player);
    player.transform().SetPosition({initialPlayerX, 0});
    EnemyType enemy;
    enemy.AddComponent<GamePlay::HitFlash>();
    enemy.transform().SetPosition({0, 0});
    enemy.Hit(10000, 1);
    const float expectedDirection = initialPlayerX > 0 ? -1.0f : 1.0f;
    Check(enemy.DeathScatterDirection() == expectedDirection, "fatal hit captures corpse direction");
    // Cross the enemy before its death state runs, then try another hit.
    player.transform().SetPosition({-initialPlayerX, 0});
    enemy.Hit(10000, 1);
    DeathState state;
    state.SetOwner(&enemy);
    state.OnEnter();
    enemy.GetComponent<GamePlay::Animator>()->Stop();
    state.OnUpdate(0);
    GameObjectMgr._MergeNewObjects();
    int found = 0;
    for (auto* object : GamePlay::GameObject::FindGameObjectsByComponent<GamePlay::Animator>())
    {
        if (auto* part = dynamic_cast<PartType*>(object))
        {
            const float previousX = part->transform().position().x;
            part->Update(1.0f / 60.0f);
            Check((part->transform().position().x - previousX) * expectedDirection > 0,
                "spawned corpse moves in fatal-hit direction after player crosses");
            ++found;
        }
    }
    Check(found == expectedParts, "all corpse parts checked");
    GameObjectMgr.ClearAllObjects({});
}

static void TestCorpseDirection()
{
    namespace fs = std::filesystem;
    const auto originalDirectory = fs::current_path();
    const auto scratch = fs::absolute(originalDirectory / "../artifacts" /
        ("corpse-direction-" + std::to_string(GetCurrentProcessId()) + "-" + std::to_string(GetTickCount64())));
    Check(fs::create_directory(scratch), "create asset-free corpse test directory");
    fs::current_path(scratch);
    for (float side : {-200.0f, 200.0f})
    {
        CheckCorpseDirection<Axe, Axe_Dead, AxePart>(side, 16);
        CheckCorpseDirection<Zombi, Zombi_Dead, ZombiPart>(side, 6);
    }
    fs::current_path(originalDirectory);
    fs::remove(scratch);
}

static void TestReturnToTitleCleanup()
{
	Check(SUCCEEDED(CoInitializeEx(nullptr, COINIT_MULTITHREADED)), "initialize COM for title textures");
    Check(App.Initialize(GetModuleHandle(nullptr), SW_HIDE, 768, 576, "Title cleanup regression"), "initialize hidden test window");
    Check(D2DRd.Initialize() && TexMgr.Initialize(), "initialize texture decoding for actual title scene");
    const int previouslyDeleted = TrackedPlayer::deleted;
    auto* player = GamePlay::GameObject::CreateGameObject<TrackedPlayer>();
    Player::SetMainPlayer(player);
    GameObjectMgr._MergeNewObjects();
    GamePlay::GameObject::CreateGameObject<TrackedPlayer>();
    auto* pendingCollider = GamePlay::GameObject::CreateGameObject<GamePlay::GameObject>();
    pendingCollider->AddComponent<GamePlay::BoxCollider>();
    SceneMgr.CreateScene<StartScene>("start");
    SceneMgr.BeginUpdate();
    SceneMgr.SwitchTo("start");
    Check(MainPlayer == player, "player lives until scene boundary");
    SceneMgr.EndUpdate();
    Check(MainPlayer == nullptr, "return to title clears main player");
    Check(TrackedPlayer::deleted == previouslyDeleted + 2, "active and pending players both deleted");
    Check(GameObjectMgr.ColliderObjects().empty(), "old collider removed on return to title");
    GameObjectMgr._MergeNewObjects();
    Check(GameObjectMgr.GameObjectCount() == 10, "only title scene objects remain");
    GameObjectMgr.ClearAllObjects({});
}

class CanvasProbe : public GameUI::UIElement
{
public:
    CanvasProbe() { mType = GameUI::UIElementType::UI_Window; }
    void Update(float) override {}
    D2D1_MATRIX_3X2_F drawn{};
    int mouseX = -1, mouseY = -1;
    void Render() override { D2DRd.GetD2DRenderTarget()->GetTransform(&drawn); }
    void OnMouseDown(int x, int y) override { mouseX = x; mouseY = y; }
};

static void TestUIScaling()
{
    GamePlay::GameObject cameraObject;
    auto* camera = cameraObject.AddComponent<GamePlay::Camera>();
    camera->SetSizeY(576);
    camera->SetAspect(4.0f / 3.0f);
    cameraObject.transform().SetPosition({384, 288});
    camera->UpdateCamera();
    GameUI::UICanvas canvas;
    canvas.Initialize(768, 576);
    canvas.SetScaleWithCamera();
    auto* probe = new CanvasProbe();
    probe->SetActive(true);
    probe->SetPosition(0, 0);
    probe->SetSize(768, 576);
    canvas.AddChild(probe);
    for (const auto size : { SIZE{768,576}, SIZE{1536,1152}, SIZE{1280,720}, SIZE{384,288}, SIZE{1001,751} })
    {
        SendMessage(App.GetHwnd(), WM_SIZE, SIZE_RESTORED, MAKELPARAM(size.cx, size.cy));
        canvas.Update(0);
        const float scale = size.cy / 576.0f;
        const float scaleX = size.cx / 768.0f;
        const auto topLeft = camera->ToScreenPos(MathType::vec2f{0, 0});
        const auto bottomRight = camera->ToScreenPos(MathType::vec2f{768, 576});
        Check(topLeft.magnitude() < 0.001f && std::abs(bottomRight.x - size.cx) < 0.001f &&
            std::abs(bottomRight.y - size.cy) < 0.001f,
            "background viewport updates immediately on resize without a frame or scene switch");
        camera->SetAspect(4.0f / 3.0f);
        camera->SetSizeY(576);
        Check((camera->ToScreenPos(MathType::vec2f{768,576}) - bottomRight).magnitude() < 0.001f,
            "resize mapping matches reentering a scene");
        canvas.Render();
        Check(std::abs(probe->drawn._11 - scaleX) < 0.001f &&
              std::abs(probe->drawn._22 - scale) < 0.001f, "HUD scales with camera at multiple viewport sizes");
        canvas.OnMouseDown(static_cast<int>(100 * scaleX), static_cast<int>(80 * scale));
        Check(std::abs(probe->mouseX - 100) <= 1 && std::abs(probe->mouseY - 80) <= 1, "scaled canvas maps mouse back to layout coordinates");
        Check(std::abs(canvas.GetSize().y - 576) < 0.001f, "scaled HUD keeps logical viewport height");
        D2D1_MATRIX_3X2_F restored;
        D2DRd.GetD2DRenderTarget()->GetTransform(&restored);
        Check(restored._11 == 1 && restored._22 == 1, "canvas restores render transform");
    }
    GameUI::UIFloatingNumber number;
    number.SetUseEaseOut(false);
    number.Spawn(12, 1000, 500, 60, 1, true);
    number.Update(0.5f);
    Check(number.GetPosition().x == 1000 && number.GetPosition().y == 470,
        "world floating number keeps world position and float distance before rendering");
    SendMessage(App.GetHwnd(), WM_SIZE, SIZE_RESTORED, MAKELPARAM(768, 576));
    std::puts("PASS: immediate background resize at five viewport sizes, scene-entry consistency, HUD scaling, mouse mapping, transform restoration, floating animation");
}

static void TestBossCycle(float dt, bool escape = false)
{
    GameObjectMgr.ClearAllObjects({});
    auto* camera = GamePlay::GameObject::CreateGameObject<GamePlay::GameObject>()->AddComponent<GamePlay::Camera>();
    camera->SetSizeY(576);
    camera->SetCameraBound({0, 0, 2000, 1000});
    camera->transform()->SetPosition({1000, 288});
    auto* player = GamePlay::GameObject::CreateGameObject<Player>();
    Player::SetMainPlayer(player);
    player->transform().SetPosition({1000, 400});
    auto* boss = GamePlay::GameObject::CreateGameObject<Fly>();
    boss->transform().SetPosition({300, 200});
    GameObjectMgr._MergeNewObjects();
    GameObjectMgr._MergeNewObjects();
    auto* left = boss->GetSwordLeft();
    auto* right = boss->GetSwordRight();
    boss->GetComponent<GamePlay::FSM>()->InitState(L"attack");
    bool previousAttack = false, orbitChecked = false, retreated = false, repeated = false;
    int volleys = 0;
    float recoveryTime = 0;
    float retreatTime = 0, maximumRetreatSpeed = 0, firstRetreatSpeed = 0, lastRetreatSpeed = 0;
    bool playerEscaped = false, chased = false;
    bool idlePlayerPlaced = false, idleMoved = false;
    float retreatRise = 0;
    for (float time = 0; time < 40; time += dt)
    {
        const auto before = boss->transform().position();
        GameObjectMgr._UpdateAllObjects(dt);
        const auto position = boss->transform().position();
        Check(position.y <= camera->transform()->position().y + 0.001f,
            "boss stays above screen midpoint throughout attack cycle");
        const bool attacking = left->IsAttacking();
        if (attacking && !previousAttack)
        {
            if (left->GetAtkType() == 1) ++volleys;
            else if (volleys > 0)
            {
                Check(volleys == 2 && retreated, "boss fires exactly two volleys before retreat and next spin");
                Check(recoveryTime >= 1.9f, "boss waits before restarting attack cycle");
                if (escape)
                {
                    Check(chased, "boss chases escaped player before restarting attack");
                    const auto delta = player->transform().position() - position;
                    Check(std::abs(delta.x) <= Fly::GetDetectWidth() * 0.5f &&
                        std::abs(delta.y) <= Fly::GetDetectHeight() * 0.5f,
                        "boss resumes attack only after player reenters detection range");
                }
                repeated = true;
                break;
            }
        }
        previousAttack = attacking;
        if (time > 1 && time < 1.3f)
        {
            const auto center = position + MathType::vec2f{0, -70};
            const auto l = left->transform().position() - center;
            const auto r = right->transform().position() - center;
            Check(std::abs(l.magnitude() - 100) < 0.1f && (l + r).magnitude() < 0.1f,
                "opposite swords orbit 70 pixels above boss pivot while body approaches");
            Check(position.x > 350, "boss body approaches player during spin");
            orbitChecked = true;
        }
        if (position.x < before.x - 0.001f)
        {
            Check(volleys == 2 && !left->IsAttacking() && !right->IsAttacking(),
                "retreat starts only after both swords complete second volley");
            Check(position.x < before.x, "boss retreats away from player");
            retreated = true;
            const float speed = (before.x - position.x) / dt;
            if (retreatTime == 0) firstRetreatSpeed = speed;
            lastRetreatSpeed = speed;
            maximumRetreatSpeed = (std::max)(maximumRetreatSpeed, speed);
            retreatTime += dt;
            Check(position.y <= before.y + 0.001f, "boss climbs throughout diagonal retreat");
            Check(position.y >= 100 - 0.001f, "retreat respects top clearance");
            retreatRise += before.y - position.y;
            if (escape && !playerEscaped)
            {
                player->transform().SetPosition({1800, 400});
                playerEscaped = true;
            }
        }
        else if (retreated)
        {
            recoveryTime += dt;
            if (!escape && !idlePlayerPlaced)
            {
                player->transform().SetPosition({position.x + 200, 400});
                idlePlayerPlaced = true;
            }
            else if (!escape && recoveryTime < 1.8f && std::abs(position.y - before.y) > 0.001f)
            {
                Check(!attacking && !right->IsAttacking(), "idle bob does not attack during cooldown");
                idleMoved = true;
            }
        }
        if (escape && playerEscaped && position.x > before.x + 0.001f && !attacking)
        {
            Check((position - before).magnitude() <= 65.1f * dt, "boss pursues slowly at bounded speed");
            Check(!right->IsAttacking(), "neither sword attacks during chase");
            if (!chased) Check(recoveryTime < 0.5f, "boss starts chasing immediately during cooldown");
            chased = true;
        }
        if (escape && playerEscaped && std::abs(player->transform().position().x - position.x) > 400)
            Check(!left->IsAttacking() && !right->IsAttacking(), "out-of-range chase never launches swords");
    }
    Check(orbitChecked && repeated, "boss completes and repeats full attack cycle");
    if (!escape) Check(idleMoved, "in-range boss idles with movement during cooldown");
    Check(retreatTime >= 1.5f && maximumRetreatSpeed < 340,
        "boss retreats slower over a longer interval");
    Check(retreatRise >= 39.0f && retreatRise <= 189.0f,
        "boss retreat gains random height within upper screen region");
    Check(firstRetreatSpeed < maximumRetreatSpeed * 0.15f && lastRetreatSpeed < maximumRetreatSpeed * 0.15f,
        "boss retreat smoothly accelerates and decelerates");
    boss->transform().SetPosition({300, 900});
    boss->GetComponent<GamePlay::FSM>()->InitState(L"idle");
    GameObjectMgr._UpdateAllObjects(dt);
    Check(boss->transform().position().y <= 288, "idle boss also respects height limit");
    GameObjectMgr.ClearAllObjects({});
}

int main()
{
    auto* selection = static_cast<Selection*>(SceneMgr.CreateScene<Selection>("selection"));
    auto* room = static_cast<Room*>(SceneMgr.CreateScene<Room>("room"));
    SceneMgr.CreateScene<Room>("other");
    SceneMgr.SwitchTo("selection");
    selection->start = true;
    Step(0.05f);
    auto* player = MainPlayer;
    Check(SceneMgr.GetCurrentSceneName() == "room", "scene switches at frame boundary without fade delay");
    Check(TrackedPlayer::deleted == 0, "pending player survives selection exit");
    Check(DeferredResource::ready == 0, "nested initialization remains queued safely");
    Check(room->sprite->GetRenderRect().left == 990.0f, "render bounds ready on first scene frame");
    Check(!SceneMgr.HasPendingTransition(), "visual fade does not block world updates");
    SceneMgr.ForwardMessage(WM_KEYDOWN, 'J', 0);
    Check(room->keys == 1, "input remains enabled during fade");
    Step(0.01f);
    Check(DeferredResource::ready == 1, "nested resources initialize during fade");
    Check(room->entries == 1 && MainPlayer == player, "player survives scene switch");
    Check(TrackedPlayer::initialized == 1, "player initialized once after old scene exits");
    Check(TrackedPlayer::updates > 0 && room->updates > 0, "object and scene update during fade");
    for (int i = 0; i < 10; ++i) Step(0.05f);
    Check(!SceneMgr.HasPendingTransition(), "transition finishes");
    Check(TrackedPlayer::updates > 0, "world keeps updating after fade");
    SceneMgr.SwitchTo("missing");
    Check(!SceneMgr.HasPendingTransition(), "invalid scene does not start fade");
    bool callback = false;
    SceneMgr.BeginUpdate();
    SceneMgr.SwitchTo("other", [&] { callback = true; });
    SceneMgr.SwitchTo("room");
    Check(!callback, "update-time transition waits for safe boundary");
    SceneMgr.EndUpdate();
    Check(callback && SceneMgr.GetCurrentSceneName() == "other", "first request wins at safe boundary");
    for (int i = 0; i < 20; ++i) Step(0.05f);
    Check(callback && MainPlayer == player, "room transition preserves player and callback");
    SceneMgr.SwitchTo("room");
    for (int i = 0; i < 20; ++i) Step(0.05f);
    Check(room->entries == 2 && MainPlayer == player, "round trip preserves player");
    GameObjectMgr.ClearAllObjects({});
    Check(TrackedPlayer::deleted == 1 && !MainPlayer, "player released exactly once");
    TestSavePreview();
    TestCorpseDirection();
    TestReturnToTitleCleanup();
    TestUIScaling();
    TestBossCycle(1.0f / 60);
    TestBossCycle(1.0f / 30);
    TestBossCycle(1.0f / 60, true);
    TestBossCycle(1.0f / 30, true);
    std::puts("PASS: Boss approach, pivot-centered orbit, two volleys, retreat, recovery and repeat at 30/60 FPS");
    std::puts("PASS: switch before fade, scene/object updates and input during fade, resource initialization, player ownership, safe-boundary callback, first request wins, round trip, cleanup");
    std::puts("PASS: save preview metadata, legacy fallback, empty/invalid slots, copy/delete refresh, timestamp serialization");
    std::puts("PASS: Axe/Zombi corpse directions on both sides, crossing after fatal hit, duplicate dead hit, all 44 spawned parts");
    std::puts("PASS: actual title entry clears active/pending players and colliders before creating title objects");
}
