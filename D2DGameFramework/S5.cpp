#include "S5.h"
#include "GameTool.h"
#include "Soma.h"
#include "PauseMenu.h"
#include "TypeDefine/Geometric.h"
#include "Drop.h"
void S5::OnEnter()
{
	// 场景切换：销毁非持久化掉落物，并刷新持久化掉落物可见性
	Drop::OnSceneChanged();

	if (!mPlayerState.IsInit())
	{
		auto canvas = GetUICanvas();
		mPlayerState.Init(canvas);
	}

	mMenuStack.SetCanvas(GetUICanvas());

	float cw = 768 * 2.0f;
	float ch = 576 * 1.0f;
	float half_w = 256 * 3 / 2.0f;
	float half_h = 192 * 3 / 2.0f;
	int order = 0;


	m_lj = GT::DrawPicture(L"resource\\backgrounds\\lost_village\\room_07_01.bmp", { 0,0 }, order++, { 0,0 }, 0x00FF00, m_bg, GamePlay::GameLayer::Layer_3)->gameobject();

	//玩家初始化
	//m_player = GT::CreateGameObject<Soma>();
	//m_player->layer() = GamePlay::GameLayer::Layer_3;
	//m_player->transform().SetPosition({ 15 * 16 * 3, 8 * 16 * 3 });
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
	pcam->SetCameraBound({ 0,0,768,576});

	//碰撞初始化
	m_ov = GT::InitObstacles<Obstacle>({
		{1 * 16 * 3, 2 * 16 * 3,2 * 16 * 3, 4 * 16 * 3},
		{8 * 16 * 3, 0.5f * 16 * 3,16 * 16 * 3, 1 * 16 * 3},
		{15 * 16 * 3, 2 * 16 * 3,2 * 16 * 3, 4 * 16 * 3},
		{15 * 16 * 3, 10 * 16 * 3,2 * 16 * 3, 4 * 16 * 3},
		{13 * 16 * 3, 10.5f * 16 * 3,2 * 16 * 3, 3 * 16 * 3},
		{8 * 16 * 3, 11.5f * 16 * 3,16 * 16 * 3, 1 * 16 * 3},
		{5 * 16 * 3, 9 * 16 * 3,2 * 16 * 3, 6 * 16 * 3},
		{2 * 16 * 3, 10 * 16 * 3,4 * 16 * 3, 4 * 16 * 3},
	});
}

void S5::OnExit()
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
}

void S5::Update(float deltaTimeSecond)
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
			//通关
			GT::SwitchTo("game_over");
		}
		else if (curPos.x > MainCamera->CameraBound().right)
		{
			m_player->transform().SetPosition({ 2 * 16 * 3, 8 * 16 * 3 });
			GT::SwitchTo("s4");
		}
	}
}

void S5::Render(float deltaTimeSecond)
{
}

void S5::OnKeyDown(WPARAM key)
{
	if (!mMenuStack.IsEmpty())
		mMenuStack.OnKeyDown(key);
}

void S5::LaterUpdate(float deltaTime)
{
}
