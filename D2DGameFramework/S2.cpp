#include "S2.h"
#include "GameTool.h"
#include "TypeDefine/Geometric.h"
#include "Soma.h"
#include "PauseMenu.h"
#include "Zombi.h"
#include "Axe.h"
#include "EffectsPool.h"
#include "ZombiPart.h"
#include "Fly_Sword.h"
#include "Player.h"
#include "BloodParticle.h"
#include "Drop.h"
#include "BreakableObject.h"
void S2::OnEnter()
{
	// 场景切换：销毁非持久化掉落物，并刷新持久化掉落物可见性
	Drop::OnSceneChanged();

	if (!mPlayerState.IsInit())
	{
		auto canvas = GetUICanvas();
		mPlayerState.Init(canvas);
	}

	mMenuStack.SetCanvas(GetUICanvas());

	// 预热特效纹理，避免运行时 IO 卡顿
	Effect::PreloadTextures();

	Debug.ShowGameInfo();
	float cw = 768 * 2.0f;
	float ch = 576 * 1.0f;
	float half_w = 256 * 3 / 2.0f;
	float half_h = 192 * 3 / 2.0f;
	int order = 0;
	m_bg = GT::DrawPicture(L"resource\\backgrounds\\lost_village\\room_05_03.bmp", { 0,0 }, order++, { 0,0 }, 0x00FF00, m_bg, GamePlay::GameLayer::Layer_0)->gameobject();
	//m_ui = GO::DrawPicture(L"resource\\ui\\runtime\\profile.bmp",			{ (cw / 2),0 }, order++, { 0,0 }, 0x00FF00, m_ui, GamePlay::GameLayer::Layer_5)->gameobject();
	m_yj = GT::DrawPicture(L"resource\\backgrounds\\lost_village\\room_05_01.bmp", { 0,0 }, order++, { 0,0 }, 0x00FF00, m_yj, GamePlay::GameLayer::Layer_1)->gameobject();
	m_zj = GT::DrawPicture(L"resource\\backgrounds\\lost_village\\room_05_02.bmp", { 0,0 }, order++, { 0,0 }, 0x00FF00, m_zj, GamePlay::GameLayer::Layer_2)->gameobject();
	m_lj = GT::DrawPicture(L"resource\\backgrounds\\lost_village\\room_05_04.bmp", { 0,0 }, order++, { 0,0 }, 0x00FF00, m_lj, GamePlay::GameLayer::Layer_3)->gameobject();
	
	//玩家初始化
	//m_player = GT::CreateGameObject<Soma>("mainPlayer");
	//m_player->layer() = GamePlay::GameLayer::Layer_4;
	//m_player->transform().SetPosition({ 79,900 });
	//GT::SetOrderInLayer(order++, *m_player);
	//Player::SetMainPlayer(m_player);
	m_player = dynamic_cast<Soma*>(MainPlayer);
	GT::SetOrderInLayer(order++, *m_player);


	//相机初始化
	m_camera = GT::CreateGameObject<GamePlay::GameObject>();
	GamePlay::Camera* pcam = m_camera->AddComponent<GamePlay::Camera>();
	pcam->transform()->SetPosition({ cw / 2,ch / 2 });
	pcam->SetAspect(4.0f / 3);
	pcam->SetSizeY(576);
	pcam->SetFollowMode(GamePlay::CFM::Follow_Hard);
	pcam->SetTarget(&m_player->transform());
	pcam->SetCameraBound({ 0,0,3072,1152 });

	//碰撞初始化
	m_ov = GT::InitObstacles<Obstacle>({
		
		
		{1.5f * 16 * 3, 12 * 16 * 3,3 * 16 * 3, 8 * 16 * 3},
		{62 * 16 * 3, 15 * 16 * 3,4 * 16 * 3, 2 * 16 * 3},
		{32 * 16 * 3, 23 * 16 * 3,64 * 16 * 3, 2 * 16 * 3}
		});

	m_pv = GT::InitObstacles<Platform>({
		{22 * 16 * 3, 19.5f * 16 * 3,1 * 16 * 3, 1 * 16 * 3},

		});

	m_epv = GT::InitObstacles<EdgePlatform>({
		{1 * 16 * 3, 21 * 16 * 3,2 * 16 * 3, 2 * 16 * 3},
		{63 * 16 * 3, 21 * 16 * 3,2 * 16 * 3, 2 * 16 * 3},
		});


	m_lsv = GT::InitObstacles<LeftSlop>({
		{60 * 16 * 3, 21 * 16 * 3,4 * 16 * 3, 2 * 16 * 3},
		});

	m_rsv = GT::InitObstacles<RightSlop>({
		{4 * 16 * 3, 21 * 16 * 3,4 * 16 * 3, 2 * 16 * 3},
		});

	m_zombi1 = GT::CreateGameObject<Zombi>();
	m_zombi1->transform().SetPosition({ 1300, 900 });
	m_zombi1->transform().SetScale({ 1.0f, 1.0f });
	m_zombi1->layer() = GamePlay::GameLayer::Layer_4;
	GT::SetOrderInLayer(order++, *m_zombi1);
	

	m_zombi2 = GT::CreateGameObject<Zombi>();
	m_zombi2->transform().SetPosition({ 1000, 900 });
	m_zombi2->transform().SetScale({ 1.0f, 1.0f });
	m_zombi2->layer() = GamePlay::GameLayer::Layer_4;
	GT::SetOrderInLayer(order++, *m_zombi2);
	
	m_axe = GT::CreateGameObject<Axe>("axe_01");
	m_axe->transform().SetPosition({ 500, 1000 });
	m_axe->transform().SetScale({ 1.0f, 1.0f });
	m_axe->layer() = GamePlay::GameLayer::Layer_3;
	GT::SetOrderInLayer(order++, *m_axe);

	// 可破坏物体示例：掉落道具 / part特效 / 自身动画均可通过预留接口替换
	//m_breakable = GT::CreateGameObject<BreakableObject>();
	//m_breakable->transform().SetPosition({ 1400, 1064-200 });
	//GT::SetOrderInLayer(order++, *m_breakable);
	// 物体被击碎销毁后置空指针，避免切场景时 SAFE_DESTROY 访问悬挂指针
	//m_breakable->SetOnBreakCallback([this]() { m_breakable = nullptr; });
	
}

void S2::OnExit()
{
	mMenuStack.Clear();
	SAFE_DESTROY(m_camera);

	for (auto p : m_ov) { p->Destroy(); }
	for (auto p : m_pv) { p->Destroy(); }
	for (auto p : m_epv) { p->Destroy(); }
	for (auto p : m_lsv) { p->Destroy(); }
	for (auto p : m_rsv) { p->Destroy(); }
	SAFE_DESTROY(m_bg);
	SAFE_DESTROY(m_bg2);
	SAFE_DESTROY(m_yj);
	SAFE_DESTROY(m_zj2);
	SAFE_DESTROY(m_zj);
	SAFE_DESTROY(m_lj);
	SAFE_DESTROY(m_ui);
	SAFE_DESTROY(m_zombi1);
	SAFE_DESTROY(m_zombi2);
	SAFE_DESTROY(m_axe);
	SAFE_DESTROY(m_breakable);
}
static int timer = 10;
void S2::Update(float deltaTimeSecond)
{
	mPlayerState.Update();

	if (!mMenuStack.IsEmpty())
	{
		mMenuStack.Update(deltaTimeSecond);
	}
	else
	{
		if (GT::KeyJustDown(VK_RETURN))
			mMenuStack.Push(std::make_unique<PauseMenu>());

		if (m_player->transform().position().x < 0)
		{
			GT::SwitchTo("s3");
			m_player->transform().SetPosition({ 14 * 16 * 3, 8 * 16 * 3 });
		}
		else if (m_player->transform().position().x > MainCamera->CameraBound().right)
		{
			GT::SwitchTo("s2");
			m_player->transform().SetPosition({ 1 * 16 * 3, 20 * 16 * 3 });
		}
	}
}

void S2::Render(float deltaTimeSecond)
{
}

void S2::OnKeyDown(WPARAM key)
{
	if (!mMenuStack.IsEmpty())
		mMenuStack.OnKeyDown(key);
}

void S2::LaterUpdate(float deltaTime)
{
	//摄像机边界
	float cw = 768;
	float ch = 576;
	const MathType::vec2f& cameraPos = MainCamera->GetPredictedPosition();
	float camera_left = cameraPos.x - cw / 2;
	float camera_top = cameraPos.y - ch / 2;
	m_bg->transform().SetPosition({ camera_left,camera_top });
	m_yj->transform().SetPosition({ camera_left * 0.8f,camera_top * 0.8f - 16 * 6 });
	m_zj->transform().SetPosition({ camera_left * 0.5f,camera_top * 0.5f - 16 });


}
