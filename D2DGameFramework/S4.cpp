#include "S4.h"
#include "GameTool.h"
#include "Soma.h"
#include "PauseMenu.h"
#include "TypeDefine/Geometric.h"
#include "Fly.h"
#include "Drop.h"
void S4::OnEnter()
{
	// 场景切换：销毁非持久化掉落物，并刷新持久化掉落物可见性
	Drop::OnSceneChanged();

	mMenuStack.SetCanvas(GetUICanvas());

	float cw = 768 * 2.0f;
	float ch = 576 * 1.0f;
	float half_w = 256 * 3 / 2.0f;
	float half_h = 192 * 3 / 2.0f;
	int order = 0;


	m_bg = GT::DrawPicture(L"resource\\backgrounds\\lost_village\\room_06_01.bmp", { 0,0 }, order++, { 0,0 }, 0x00FF00, m_bg, GamePlay::GameLayer::Layer_0)->gameobject();

	//m_ui = GO::DrawPicture(L"resource\\ui\\runtime\\profile.bmp",			{ (cw / 2),0 }, order++, { 0,0 }, 0x00FF00, m_ui, GamePlay::GameLayer::Layer_5)->gameobject();
	m_zj = GT::DrawPicture(L"resource\\backgrounds\\lost_village\\room_06_02.bmp", { 0,0 }, order++, { 0,0 }, 0x00FF00, m_zj, GamePlay::GameLayer::Layer_2)->gameobject();
	m_lj = GT::DrawPicture(L"resource\\backgrounds\\lost_village\\room_06_03.bmp", { 0,0 }, order++, { 0,0 }, 0x00FF00, m_lj, GamePlay::GameLayer::Layer_3)->gameobject();
	//玩家初始化
	//m_player = GT::CreateGameObject<Soma>();
	//m_player->layer() = GamePlay::GameLayer::Layer_3;
	//m_player->transform().SetPosition({ 300,300 });
	//GT::SetOrderInLayer(order++, *m_player);

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
	pcam->SetCameraBound({ 0,0,1536,576 });

	//碰撞初始化
	m_ov = GT::InitObstacles<Obstacle>({	
		{15 * 16 * 3, 11.5f * 16 * 3,12 * 16 * 3, 1 * 16 * 3},
		});

	//m_pv = GO::InitObstacles<Platform>({
	//	{9 * 16 * 3, 11 * 16 * 3,2 * 16 * 3, 5},
	//	{9 * 16 * 3, 14 * 16 * 3 + 16,2 * 16 * 3, 5},
	//	{9 * 16 * 3, 17 * 16 * 3 + 16,2 * 16 * 3, 5},
	//	});

	m_epv = GT::InitObstacles<EdgePlatform>({
		{31 * 16 * 3, 10 * 16 * 3,2 * 16 * 3, 4 * 16 * 3},
		{31 * 16 * 3, 2 * 16 * 3,2 * 16 * 3, 4 * 16 * 3},
		{29 * 16 * 3, 1.5 * 16 * 3,2 * 16 * 3, 3 * 16 * 3},
		{25.5f * 16 * 3, 10.5f * 16 * 3,1 * 16 * 3, 1 * 16 * 3},
		{4.5f * 16 * 3, 10.5f * 16 * 3,1 * 16 * 3, 1 * 16 * 3},
		{2.5f * 16 * 3, 2 * 16 * 3,5 * 16 * 3, 4 * 16 * 3},
		{1 * 16 * 3, 9 * 16 * 3,2 * 16 * 3, 2 * 16 * 3},
		});


	m_lsv = GT::InitObstacles<LeftSlop>({
		{28 * 16 * 3, 9 * 16 * 3,4 * 16 * 3, 2 * 16 * 3},
		{23 * 16 * 3, 10.5f * 16 * 3,4 * 16 * 3, 1 * 16 * 3},
		
		});

	m_rsv = GT::InitObstacles<RightSlop>({
		{7 * 16 * 3, 10.5f * 16 * 3,4 * 16 * 3, 1 * 16 * 3},
		{3 * 16 * 3, 9 * 16 * 3,2 * 16 * 3, 2 * 16 * 3},

		});


	m_fly = GT::CreateGameObject<Fly>("fly_boss");
	m_fly->transform().SetPosition({ 500, 300 });
	m_fly->transform().SetScale({ 1.0f, 1.0f });
	m_fly->layer() = GamePlay::GameLayer::Layer_4;
	GT::SetOrderInLayer(order++, *m_fly);

	if (!mPlayerState.IsInit())
	{
		auto canvas = GetUICanvas();
		mPlayerState.Init(canvas);
		mBossHP.Init(canvas, m_fly->GetAttributes().hp);
	}
	// 返回房间重新生成 Boss 时，首帧即恢复血条。
	mBossHP.Update(m_fly->GetAttributes().cur_hp, m_fly->GetAttributes().hp, 0);

}

void S4::OnExit()
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
	SAFE_DESTROY(m_fly);
}

void S4::Update(float deltaTimeSecond)
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

		MathType::vec2f curPos = m_player->transform().position();
		if (curPos.x < 0)
		{
			m_player->transform().SetPosition({ 15 * 16 * 3, 8 * 16 * 3 });
			GT::SwitchTo("s5");
		}
		else if (curPos.x > MainCamera->CameraBound().right)
		{
			m_player->transform().SetPosition({ 1 * 16 * 3, 20 * 16 * 3 });
			GT::SwitchTo("s3");
		}
	}
}

void S4::Render(float deltaTimeSecond)
{
}

void S4::OnKeyDown(WPARAM key)
{
	if (!mMenuStack.IsEmpty())
		mMenuStack.OnKeyDown(key);
}

void S4::LaterUpdate(float deltaTime)
{
	//摄像机边界
	float cw = 768;
	float ch = 576;
	const MathType::vec2f& cameraPos = MainCamera->GetPredictedPosition();
	float camera_left = cameraPos.x - cw / 2;
	float camera_top = cameraPos.y - ch / 2;
	m_bg->transform().SetPosition({ camera_left * 0.5f,camera_top });
	m_zj->transform().SetPosition({ camera_left * 0.25f,0 });

	//BOSS HP
	auto attr = m_fly->GetAttributes();
	mBossHP.Update(attr.cur_hp, attr.hp, deltaTime);

}
