#pragma once
#include "./GameCore/Scene.h"
#include "./GamePlay/GameObject.h"

class GameOverScene : public GameCore::Scene
{
public:
	virtual void OnEnter() override;
	virtual void OnExit() override;
	virtual void Update(float deltaTimeSecond) override;
	virtual void Render(float deltaTimeSecond) override;
	virtual void OnKeyDown(WPARAM key) override;
private:
	GamePlay::GameObject* m_camera = nullptr;
	float m_wait_time = 0.0f;   // 累计等待时间，超时后回到标题场景
};
