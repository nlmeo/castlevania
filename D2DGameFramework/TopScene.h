#pragma once
#include "./GameCore/Scene.h"
#include "./GamePlay/GameObject.h"
#include "GameUI/UIText.h"
#include "GameUI/UITextArea.h"
class TopScene : public GameCore::Scene
{

public:
	virtual void OnEnter() override;
	virtual void OnExit() override;
	virtual void Update(float deltaTimeSecond) override;
	virtual void Render(float deltaTimeSecond) override;
	void RenderUICanvas(float dt) override;
	virtual void OnKeyDown(WPARAM key) override;
private:
	std::vector<GamePlay::GameObject*> m_options;
	GamePlay::GameObject* m_camera;

	int m_cur_opt_index = 0;
	int mDescriptionFont = -1;
	int mHintFont = -1;
	GameUI::UITextArea* mDescription = nullptr;
	GameUI::UIText* mKeyHint = nullptr;
	//std::vector<std::pair<std::wstring, std::wstring>> m_options; //选中的bmp key

};
