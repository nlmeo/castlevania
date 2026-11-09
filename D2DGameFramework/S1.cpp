#include "S1.h"
#include "GameTool.h"
#include "Soma.h"
#include "PauseMenu.h"
#include "TypeDefine/Geometric.h"
#include "Zombi.h"
#include "Drop.h"
void S1::OnEnter()
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


	m_bg = GT::DrawPicture(L"resource\\backgrounds\\lost_village\\room_02_03.bmp", { 0,0 }, order++, { 0,0 }, 0x00FF00, m_bg, GamePlay::GameLayer::Layer_0)->gameobject();

	//m_ui = GO::DrawPicture(L"resource\\ui\\runtime\\profile.bmp",			{ (cw / 2),0 }, order++, { 0,0 }, 0x00FF00, m_ui, GamePlay::GameLayer::Layer_5)->gameobject();
	m_zj = GT::DrawPicture(L"resource\\backgrounds\\lost_village\\room_02_02.bmp", { 0,0 }, order++, { 0,0 }, 0x00FF00, m_zj, GamePlay::GameLayer::Layer_2)->gameobject();
	m_lj = GT::DrawPicture(L"resource\\backgrounds\\lost_village\\room_02_01.bmp", { 0,0 }, order++, { 0,0 }, 0x00FF00, m_lj, GamePlay::GameLayer::Layer_3)->gameobject();
	//
	//m_player = GT::CreateGameObject<Soma>();
	//m_player->layer() = GamePlay::GameLayer::Layer_3;
	//m_player->transform().SetPosition({ 500,500 });
	//GT::SetOrderInLayer(order++, *m_player);


	m_player = dynamic_cast<Soma*>(MainPlayer);
	GT::SetOrderInLayer(order++, *m_player);

	//相机
	m_camera = GT::CreateGameObject<GamePlay::GameObject>();
	GamePlay::Camera* pcam = m_camera->AddComponent<GamePlay::Camera>();
	pcam->transform()->SetPosition({ cw / 2,ch / 2 });
	pcam->SetAspect(4.0f / 3);
	pcam->SetSizeY(576);
	pcam->SetFollowMode(GamePlay::CFM::Follow_Hard);
	pcam->SetTarget(&m_player->transform());
	pcam->SetCameraBound({ 0,0,1536,1152 });

	Debug.ShowGameInfo(true);

	m_ov = GT::InitObstacles<Obstacle>({
		{16 * 16 * 3, 22 * 16 * 3, 32 * 16 * 3, 2 * 16 * 3},
	
	});

	m_epv = GT::InitObstacles<EdgePlatform>({
		{1.5f * 16 * 3, 20.5f * 16 * 3,3 * 16 * 3, 1 * 16 * 3},
		{29.5f * 16 * 3, 20.5f * 16 * 3,5 * 16 * 3, 1 * 16 * 3}
	});


	m_lsv = GT::InitObstacles<LeftSlop>({
		{25 * 16 * 3, 20.5 * 16 * 3,4 * 16 * 3, 1 * 16 * 3},
	});

	m_rsv = GT::InitObstacles<RightSlop>({
		{4 * 16 * 3, 20.5 * 16 * 3,2 * 16 * 3, 1 * 16 * 3},
	});


	m_zombi1 = GT::CreateGameObject<Zombi>();
	m_zombi1->transform().SetPosition({ 1 * 16 * 3 + 300, 20 * 16 * 3 });
	m_zombi1->transform().SetScale({ 1.0f, 1.0f });
	m_zombi1->layer() = GamePlay::GameLayer::Layer_4;
	GT::SetOrderInLayer(order++, *m_zombi1);


	m_zombi2 = GT::CreateGameObject<Zombi>();
	m_zombi2->transform().SetPosition({ 1 * 16 * 3 + 600, 20 * 16 * 3 });
	m_zombi2->transform().SetScale({ 1.0f, 1.0f });
	m_zombi2->layer() = GamePlay::GameLayer::Layer_4;
	GT::SetOrderInLayer(order++, *m_zombi2);

	m_zombi3 = GT::CreateGameObject<Zombi>();
	m_zombi3->transform().SetPosition({ 1 * 16 * 3 + 900, 20 * 16 * 3 });
	m_zombi3->transform().SetScale({ 1.0f, 1.0f });
	m_zombi3->layer() = GamePlay::GameLayer::Layer_4;
	GT::SetOrderInLayer(order++, *m_zombi3);


	m_zombi4 = GT::CreateGameObject<Zombi>();
	m_zombi4->transform().SetPosition({ 1 * 16 * 3 + 1200, 20 * 16 * 3 });
	m_zombi4->transform().SetScale({ 1.0f, 1.0f });
	m_zombi4->layer() = GamePlay::GameLayer::Layer_4;
	GT::SetOrderInLayer(order++, *m_zombi4);

}
void S1::OnExit()
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
	SAFE_DESTROY(m_zombi3);
	SAFE_DESTROY(m_zombi4);
}

void S1::Update(float deltaTimeSecond)
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

		if (m_player->transform().position().x > MainCamera->CameraBound().right)
		{
			GT::SwitchTo("s3");
			m_player->transform().SetPosition({ 1 * 16 * 3, 8 * 16 * 3 });
		}
		else if (m_player->transform().position().x < 0)
		{
			GT::SwitchTo("s0");
			m_player->transform().SetPosition({ 30 * 16 * 3, 8 * 16 * 3 });
		}
	}
}

void S1::Render(float deltaTimeSecond)
{
}

void S1::OnKeyDown(WPARAM key)
{
	if (!mMenuStack.IsEmpty())
		mMenuStack.OnKeyDown(key);
}

void S1::LaterUpdate(float deltaTime)
{
	//摄像机边界
	float cw = 768;
	float ch = 576;
	const MathType::vec2f& cameraPos = MainCamera->GetPredictedPosition();
	float camera_left = cameraPos.x - cw / 2;
	float camera_top = cameraPos.y - ch / 2;

	m_bg->transform().SetPosition({ camera_left ,camera_top });
	//m_yj->transform().SetPosition({ camera_left * 0.8f,camera_top });
	m_zj->transform().SetPosition({ camera_left * 0.3f,3* 16*3 });





}
