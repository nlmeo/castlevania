#include "S6.h"
#include "GameTool.h"
#include "Soma.h"
#include "PauseMenu.h"
#include "TypeDefine/Geometric.h"
#include "Drop.h"
void S6::OnEnter()
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

	m_bg = GT::DrawPicture(L"resource\\backgrounds\\lost_village\\room_08_01.bmp", { 0,0 }, order++, { 0,0 }, 0x00FF00, m_bg, GamePlay::GameLayer::Layer_0)->gameobject();
	m_lj = GT::DrawPicture(L"resource\\backgrounds\\lost_village\\room_08_02.bmp", { 0,0 }, order++, { 0,0 }, 0x00FF00, m_lj, GamePlay::GameLayer::Layer_3)->gameobject();
	
	m_bg2 = GT::DrawPicture(L"resource\\backgrounds\\lost_village\\room_07_01.bmp", { 0,0 }, order++, { 0,0 }, 0x00FF00, m_bg2, GamePlay::GameLayer::Layer_3)->gameobject();

	
	m_player = dynamic_cast<Soma*>(MainPlayer);
	GT::SetOrderInLayer(order++, *m_player);
	//相机
	m_camera = GT::CreateGameObject<GamePlay::GameObject>();
	GamePlay::Camera* pcam = m_camera->AddComponent<GamePlay::Camera>();
	pcam->transform()->SetPosition({ cw / 2,ch / 2 });
	pcam->SetAspect(4.0f / 3);
	pcam->SetSizeY(576);
	pcam->SetFollowMode(GamePlay::CFM::Follow_Hard);
	pcam->SetTarget(&m_camera->transform());
}

void S6::OnExit()
{
	mMenuStack.Clear();
	SAFE_DESTROY(m_camera);
	SAFE_DESTROY(m_bg);
	SAFE_DESTROY(m_bg2);
	SAFE_DESTROY(m_lj);
}

void S6::Update(float deltaTimeSecond)
{
	mPlayerState.Update();

	if (!mMenuStack.IsEmpty())
	{
		mMenuStack.Update(deltaTimeSecond);
		return;
	}

	if (GT::KeyJustDown(VK_RETURN))
	{
		mMenuStack.Push(std::make_unique<PauseMenu>());
		return;
	}

	//std::cout << "Q" << "\n";
	std::cout << "HERO_POSITION: X: " << m_player->transform().position().x << " Y: " << m_player->transform().position().y << "\n";
	//256 * 3 * 2, 192 * 3



	//摄像机边界
	float cw = 768.0f;
	float ch = 576.0f;
	float camera_x = m_player->transform().position().x;
	float camera_y = m_player->transform().position().y;
	MathType::RectF cameraBound = { 0,0,1536,2304 };

	if (camera_x < cameraBound.left + cw / 2)
	{
		camera_x = cameraBound.left + cw / 2;
	}
	else if (camera_x > cameraBound.right - cw / 2)
	{
		camera_x = cameraBound.right - cw / 2;
	}

	if (camera_y < cameraBound.top + ch / 2)
	{
		camera_y = cameraBound.top + ch / 2;
	}
	else if (camera_y > cameraBound.bottom - ch / 2)
	{
		camera_y = cameraBound.bottom - ch / 2;
	}

	m_camera->transform().SetPosition({ camera_x,camera_y });

	//卷轴计算
	//摄像机初始位置为
	float camera_left = camera_x - cw / 2;
	float camera_top = camera_y - ch / 2;
	m_bg->transform().SetPosition({ camera_left * 0.95f,camera_top });
	


	if (GT::KeyDown(VK_UP))
	{
		m_player->transform().translate(MathType::vec2f::down() * deltaTimeSecond * 200);
	}
	else if (GT::KeyDown(VK_DOWN))
	{
		m_player->transform().translate(MathType::vec2f::up() * deltaTimeSecond * 200);
	}

	if (GT::KeyDown(VK_LEFT))
	{
		m_player->transform().translate(MathType::vec2f::left() * deltaTimeSecond * 200);
	}
	else if (GT::KeyDown(VK_RIGHT))
	{
		m_player->transform().translate(MathType::vec2f::right() * deltaTimeSecond * 200);
	}

	if (GT::KeyDown(VK_PRIOR))
	{
		float sizey = MainCamera->SizeY();
		MainCamera->SetSizeY(sizey += deltaTimeSecond * 100);
	}
	else if (GT::KeyDown(VK_NEXT))
	{
		float sizey = MainCamera->SizeY();
		MainCamera->SetSizeY(sizey -= deltaTimeSecond * 100);
	}
	if (GT::KeyJustDown('Q'))
	{
		m_player->GetComponent<GamePlay::Animator>()->OnDeBugPlay();
	}

}

void S6::Render(float deltaTimeSecond)
{
}

void S6::OnKeyDown(WPARAM key)
{
	if (!mMenuStack.IsEmpty())
		mMenuStack.OnKeyDown(key);
}

void S6::LaterUpdate(float deltaTime)
{
}
