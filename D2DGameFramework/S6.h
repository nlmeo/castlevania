#pragma once
#include "./GameCore/Scene.h"
#include "./GamePlay/GameObject.h"
#include "PlayerState.h"
#include "GameUI/MenuStack.h"

class S6 : public GameCore::Scene
{

public:
	bool IsWorldPaused() const override { return !mMenuStack.IsEmpty(); }
	virtual void OnEnter() override;
	virtual void OnExit() override;
	virtual void Update(float deltaTimeSecond) override;
	virtual void Render(float deltaTimeSecond) override;
	virtual void OnKeyDown(WPARAM key) override;
	virtual void LaterUpdate(float deltaTime) override;
private:
	PlayerState mPlayerState;
	GameUI::MenuStack mMenuStack;
	GamePlay::GameObject* m_camera;
	class Soma* m_player;


	GamePlay::GameObject* m_bg;
	GamePlay::GameObject* m_bg2;
	GamePlay::GameObject* m_yj;
	GamePlay::GameObject* m_zj2;
	GamePlay::GameObject* m_zj;
	GamePlay::GameObject* m_lj;
	GamePlay::GameObject* m_ui;

};
