#pragma once
#include "./GameCore/Scene.h"
#include "./GamePlay/GameObject.h"
class TopScene : public GameCore::Scene
{
	//中心点坐标
	
	//
public:
	virtual void OnEnter() override;
	virtual void OnExit() override;
	virtual void Update(float deltaTimeSecond) override;
	virtual void Render(float deltaTimeSecond) override;
	virtual void OnKeyDown(WPARAM key) override;
private:
	std::vector<GamePlay::GameObject*> m_options;
	GamePlay::GameObject* m_camera;


};
