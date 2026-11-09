#pragma once
#include "./GameCore/Scene.h"
#include "./GamePlay/GameObject.h"

class StartScene : public GameCore::Scene
{
	
public:
	virtual void OnEnter() override;
	virtual void OnExit() override;
	virtual void Update(float deltaTimeSecond) override;
	virtual void Render(float deltaTimeSecond) override;
	virtual void OnKeyDown(WPARAM key) override;
private:
	GamePlay::SpriteRender* pOption1;
	GamePlay::SpriteRender* pOption2;
	GamePlay::SpriteRender* pOption3;
	GamePlay::GameObject* pCamera;

	int m_cur_opt_index = 0;
	std::vector<std::pair<std::wstring, std::wstring>> m_options; //选中的bmp key

};
