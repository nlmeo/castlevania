#pragma once
#include "./GameCore/Scene.h"
#include "./GamePlay/GameObject.h"
#include "GameUI/UITextArea.h"

class SelectDataScene : public GameCore::Scene
{
	bool mStartingGame = false;
	int m_cur_opt_index = 0;
	int m_copy_src = -1;                     // 复制模式下的源槽（-1 表示尚未选择）
	std::vector<std::pair<int, int>> m_options;
	GamePlay::GameObject* m_title;
	GamePlay::GameObject* m_pointer;
	GamePlay::GameObject* m_camera;
	int mDescriptionFont = -1;
	int mPreviewFont = -1;
	int mNewGameFont = -1;
	GameUI::UITextArea* mDescription = nullptr;
	GameUI::UITextArea* mSavePreview = nullptr;
	void RefreshSlotText();
public:
	virtual void OnEnter() override;
	virtual void OnExit() override;
	virtual void Update(float deltaTimeSecond) override;
	virtual void Render(float deltaTimeSecond) override;
	void RenderUICanvas(float dt) override;
	virtual void OnKeyDown(WPARAM key) override;
};
