#include "S0.h"
#include "GameTool.h"
#include "Soma.h"
#include "PauseMenu.h"
#include "TypeDefine/Geometric.h"
#include "D2DCore/TextureManager.h"
#include "Drop.h"


void S0::OnEnter()
{
	// 场景切换：销毁非持久化掉落物，并刷新持久化掉落物可见性
	Drop::OnSceneChanged();

	if (!mPlayerState.IsInit())
	{
		auto canvas = GetUICanvas();
		mPlayerState.Init(canvas);
	}

	mMenuStack.SetCanvas(GetUICanvas());
	
	Debug.ShowGameInfo();
	GT::StopAllMusic();
	float cw = 768 * 2.0f;
	float ch = 576 * 1.0f;
	float half_w = 256 * 3 / 2.0f;
	float half_h = 192 * 3 / 2.0f;
	int order = 0;

	m_bg = GT::DrawPicture(L"resource\\backgrounds\\lost_village\\room_01_02.bmp", { 0,0 }, order++, { 0,0 }, 0x00FF00,m_bg, GamePlay::GameLayer::Layer_0)->gameobject();
	 
	//m_ui = GO::DrawPicture(L"resource\\ui\\runtime\\profile.bmp",			{ (cw / 2),0 }, order++, { 0,0 }, 0x00FF00, m_ui, GamePlay::GameLayer::Layer_5)->gameobject();
	m_yj = GT::DrawPicture(L"resource\\backgrounds\\lost_village\\room_01_04.bmp", { 0,0 }, order++, { 0,0 }, 0x00FF00, m_yj, GamePlay::GameLayer::Layer_1)->gameobject();
	m_zj = GT::DrawPicture(L"resource\\backgrounds\\lost_village\\room_01_03.bmp", { 0,0 }, order++, { 0,0 }, 0x00FF00, m_zj, GamePlay::GameLayer::Layer_2)->gameobject();
	m_lj = GT::DrawPicture(L"resource\\backgrounds\\lost_village\\room_01_01.bmp", { 0,0 }, order++, { 0,0 }, 0x00FF00, m_lj, GamePlay::GameLayer::Layer_3)->gameobject();
	
	m_player = dynamic_cast<Soma*>(MainPlayer);
	GT::SetOrderInLayer(order++, *m_player);
	//
	//m_player = GT::CreateGameObject<Soma>();
	//m_player->layer() = GamePlay::GameLayer::Layer_4;
	//m_player->transform().SetPosition({ 600,100 });
	
	//相机
	m_camera = GT::CreateGameObject<GamePlay::GameObject>();
	GamePlay::Camera* pcam = m_camera->AddComponent<GamePlay::Camera>();
	pcam->transform()->SetPosition({ cw / 2,ch / 2 });
	pcam->SetAspect(4.0f / 3);
	pcam->SetSizeY(576);
	pcam->SetFollowMode(GamePlay::CFM::Follow_Hard);
	pcam->SetTarget(&m_player->transform());
	pcam->SetCameraBound({ 0,0,1536,576 });

	// TEST: 测试掉落物 — 在(300,200)生成ID=4的掉落物，玩家碰触自动入包
	if (!mIsSetSword)
	{
		Drop::Spawn(4, { 300, 200 }, true);
		mIsSetSword = true;
	}
		


	Debug.ShowGameInfo(true);

	m_ov = GT::InitObstacles<Obstacle>({
		{0   * 3  + 512 * 3 / 2.0f,		160 * 3 + 32 * 3 / 2.0f,		512 * 3,			32 * 3	},
		{432 * 3  + 80 * 3 / 2.0f,		0   * 3 + 32 * 3 / 2.0f,		80  * 3,			32 * 3	},
		{448 * 3  + 64 * 3 / 2.0f,		32  * 3 + 16 * 3 / 2.0f,		64  * 3,			16 * 3	},
		{464 * 3  + 48 * 3 / 2.0f,		48  * 3 + 16 * 3 / 2.0f,		48  * 3,			16 * 3	},
		{32  * 3  + 16 * 3 / 2.0f,		96  * 3 + 64 * 3 / 2.0f,		16  * 3,			64 * 3	},
		{32  * 3  + 64 * 3 / 2.0f,		64  * 3 + 32 * 3 / 2.0f,		64  * 3,			32 * 3	},

	});


	m_epv = GT::InitObstacles<EdgePlatform>({
		{480 * 3 + 32 * 3 / 2.0f,		128 * 3 + 32 * 3 / 2.0f,				32 * 3,		32 * 3	},
	});


	m_lsv = GT::InitObstacles<LeftSlop>({
		{416 * 3 + 64 * 3 / 2.0f,		128 * 3 + 32 * 3 / 2.0f,				64 * 3,		32 * 3	},
		});

}

void S0::OnExit()
{
	mMenuStack.Clear();
	SAFE_DESTROY(m_camera);

	for (auto p : m_ov)  { p->Destroy(); }
	for (auto p : m_pv)  { p->Destroy(); }
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

void S0::Update(float deltaTimeSecond)
{
	mPlayerState.Update();

	// 菜单模式：仅驱动菜单栈
	if (!mMenuStack.IsEmpty())
	{
		mMenuStack.Update(deltaTimeSecond);
		return;
	}
	else
	{
		// 游戏模式：Enter 键打开暂停菜单
		if (GT::KeyJustDown(VK_RETURN))
		{
			mMenuStack.Push(std::make_unique<PauseMenu>());
			return;
		}
	}
	

	if (m_player->transform().position().x > MainCamera->CameraBound().right)
	{
		GT::StopAllMusic();
		GameTool::PlayBGM(BGM_PITCH_BLACK_INTRUSION,true);
		m_player->transform().SetPosition({ 1 * 16 * 3, 20 * 16 * 3 });
		GT::SwitchTo("s1");
	}

	
}

void S0::LaterUpdate(float deltaTime)
{
	//摄像机边界
	float cw = 768;
	float ch = 576;
	const MathType::vec2f& cameraPos = MainCamera->GetPredictedPosition();
	float camera_left = cameraPos.x - cw / 2;
	float camera_top = cameraPos.y - ch / 2;

	m_bg->transform().SetPosition({ camera_left,camera_top });
	m_yj->transform().SetPosition({ camera_left * 0.8f,camera_top });
	m_zj->transform().SetPosition({ camera_left * 0.5f,camera_top });


}


void S0::Render(float deltaTimeSecond)
{
	
}

void S0::OnKeyDown(WPARAM key)
{
	//if (!mMenuStack.IsEmpty())
	//{
	//	mMenuStack.OnKeyDown(key);
	//}
}
