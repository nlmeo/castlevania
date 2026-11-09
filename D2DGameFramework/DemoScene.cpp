#include "DemoScene.h"
#include "./D2DCore/FontRenderer.h"
#include "./D2DCore/TextureManager.h"
#include "./D2DCore/D2DRender.h"
#include "./D2DCore/Sprite.h"
#include "./TypeDefine/Color.h"
#include "./GameCore/InputMgr.h"
#include "./GamePlay/GameObject.h"
#include "./HelpTool/RandomMaker.h"
#include "./GamePlay/Debug.h"
#include "rock.h"
#include "Soma.h"
#include "GameTool.h"
GamePlay::GameObject* g = nullptr;

const int ROCK_COUNT = 500;

Rock* g_Rocks[ROCK_COUNT];

GamePlay::GameObject* g_Camera;

Soma* g_Hero = nullptr;

DemoScene::~DemoScene()
{
}

void DemoScene::OnEnter()
{

	Debug.ShowGameInfo(true);

	//for (int i=0;i< ROCK_COUNT;++i)
	//{
	//	g_Rocks[i] = GamePlay::GameObject::CreateGameObject<Rock>();
	//	g_Rocks[i]->transform().SetMatOrder(GamePlay::Transform::MatrixOrder::SRT);
	//	g_Rocks[i]->transform().SetPosition(MathType::vec2f::random()* (rand() / 32767.0f-0.5f)* ROCK_COUNT);
	//	g_Rocks[i]->transform().SetRotation(rand() % 360);
	//	g_Rocks[i]->transform().SetScale(MathType::vec2f(HelpTool::RandomFloat(0.01f, 0.05f), HelpTool::RandomFloat(0.01f, 0.05f)));
	//}


	GT::DrawPicture(L"resource\\backgrounds\\lost_village\\15.bmp", { 300,0 },0, { 0, 0 }, 0x00FF00, GT::CreateGameObject<GamePlay::GameObject>());

	

	g = GamePlay::GameObject::CreateGameObject<GamePlay::GameObject>();
	GamePlay::SpriteRender* p = g->AddComponent<GamePlay::SpriteRender>();
	p->SetTexture(TexMgr.LoadTextureWithoutColor(L"resource\\backgrounds\\lost_village\\15.bmp", RGB(0, 255, 0)));
	p->SetAnchor(0, 0);
	p->SetOrderInLayer(0);


	g_Hero = GamePlay::GameObject::CreateGameObject<Soma>();
	g_Hero->transform().SetMatOrder(GamePlay::Transform::MatrixOrder::SRT);
	g_Hero->transform().SetPosition({0,0});
	//g_Hero->transform().SetRotation(rand() % 360);
	g_Hero->transform().SetScale({ 1.0f,1.0f });
	
	g_Camera = GamePlay::GameObject::CreateGameObject<GamePlay::GameObject>();
	GamePlay::Camera* pcam = g_Camera->AddComponent<GamePlay::Camera>();
	pcam->SetAspect(8/3);
	pcam->SetSizeY(576);
	pcam->SetDeadZoonFollowParam(0.1f, 0.1f, 0.98f);
	pcam->SetFollowMode(GamePlay::CFM::Follow_DeadZone);
	pcam->SetTarget(&g_Hero->transform());
	pcam->transform()->SetPosition({ 0,0 });


	m_rotatin = 0.0f;
	m_time = 0.0f;
}

void DemoScene::OnExit()
{

}

void DemoScene::Update(float deltaTimeSecond)
{
	m_time += deltaTimeSecond;
	m_rotatin += deltaTimeSecond * 45.0f;
	if (m_rotatin > 360.0f)
	{
		m_rotatin -= 360.0f;
	}


	if (GameInput.KeyDown(VK_UP))
	{
		g_Hero->transform().translate(MathType::vec2f::down() * deltaTimeSecond * 100);
	}
	else if (GameInput.KeyDown(VK_DOWN))
	{
		g_Hero->transform().translate(MathType::vec2f::up() * deltaTimeSecond * 100);
	}

	if (GameInput.KeyDown(VK_LEFT))
	{
		g_Hero->transform().translate(MathType::vec2f::left() * deltaTimeSecond * 100);
	}
	else if (GameInput.KeyDown(VK_RIGHT))
	{
		g_Hero->transform().translate(MathType::vec2f::right() * deltaTimeSecond * 100);
	}

	if (GameInput.KeyDown(VK_PRIOR))
	{
		float sizey = MainCamera->SizeY();
		MainCamera->SetSizeY(sizey += deltaTimeSecond * 100);
	}
	else if (GameInput.KeyDown(VK_NEXT))
	{
		float sizey = MainCamera->SizeY();
		MainCamera->SetSizeY(sizey -= deltaTimeSecond * 100);
	}
	
	if (GameInput.KeyJustDown('Q'))
	{
		std::cout << "Q" << "\n";
		g_Hero->GetComponent<GamePlay::Animator>()->OnDeBugPlay();
	}


}

void DemoScene::Render(float deltaTimeSecond)
{
	D2DCore::D2DRender& a = D2DCore::D2DRender::Instance();
	a.DrawCircle(100, 100, 50, ColorType::COLORF::Red());
}

void DemoScene::OnKeyDown(WPARAM key)
{

	if (key == VK_ESCAPE)
	{
		HWND hwnd = GetActiveWindow();
		if (hwnd)
		{
			PostMessage(hwnd, WM_CLOSE, 0, 0);
		}
	}

	if (key == VK_SPACE)
	{
		MainCamera->Shake(3, 10);
	}

	if (key == (VK_INSERT))
	{
		GamePlay::CFM mode = GamePlay::CFM((int(MainCamera->getFollowMode()) + 1) % int(GamePlay::CFM::Follow_Mode_Limit));
		std::cout << "当前跟随模式" << (int)mode << "\n";
		MainCamera->SetFollowMode(mode);
		MainCamera->SetSmoothFollowParam(30.0f,20.0f, 0.95f);
	}

}

void DemoScene::DrawTitle(float w, float h)
{
}

void DemoScene::DrawBasicShapesDemo(float w, float h)
{
}

void DemoScene::DrawTextDemo(float w, float h)
{
}

void DemoScene::DrawTextureDemo(float w, float h)
{
}

void DemoScene::DrawInstructions(float w, float h)
{
}
