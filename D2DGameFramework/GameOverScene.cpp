#include "GameOverScene.h"
#include "GameTool.h"

void GameOverScene::OnEnter()
{
	GT::ClearAllGameObject();
	float cw = 768 * 2.0f;
	float ch = 576 * 1.0f;
	float half_w = 256 * 3 / 2.0f;
	int order = 0;

	GT::StopAllMusic();

	// 相机
	m_camera = GT::CreateGameObject<GamePlay::GameObject>();
	GamePlay::Camera* pcam = m_camera->AddComponent<GamePlay::Camera>();
	pcam->transform()->SetPosition({ cw / 2, ch / 2 });
	pcam->SetAspect(4.0f / 3);
	pcam->SetSizeY(576);
	pcam->SetFollowMode(GamePlay::CFM::Follow_Hard);
	pcam->SetTarget(&m_camera->transform());
	pcam->SetCameraBound({ 0, 0, cw, ch });

	// 占位图（暂用选档界面背景）
	GT::DrawPicture(L"resource\\ui\\gameover\\frame000.png", { half_w, 0 }, order++, { 0, 0 }, 0x00FF00);
	GT::DrawPicture(L"resource\\ui\\gameover\\frame001.png", { half_w, 0 }, order++, { 0, 0 }, 0xFFFFFF);
	m_wait_time = 0.0f;
}

void GameOverScene::OnExit()
{
	GT::ClearAllGameObject();
}

void GameOverScene::Update(float deltaTimeSecond)
{
	m_wait_time += deltaTimeSecond;
	if (m_wait_time >= 3.0f)
	{
		GT::SwitchTo("start");
	}
}

void GameOverScene::Render(float deltaTimeSecond)
{
}

void GameOverScene::OnKeyDown(WPARAM key)
{
}
