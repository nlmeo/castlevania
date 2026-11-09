#pragma once
#include "GameUI/UICanvas.h"
#include "GameUI/UIImage.h"
#include "GameUI/UINumber.h"
#include "GameUI/UIText.h"
#include "GameUI/UIAnimator.h"
#include "GameUI/UITextArea.h"
#include "GameUI/IMenu.h"
#include "GameUI/MenuStack.h"
#include "D2DCore/TextureManager.h"
#include "D2DCore/FontRenderer.h"
#include <vector>
#include <string>


class PauseMenu : public GameUI::IMenu
{
	struct OptionItem
	{
		std::wstring itemName;
		std::wstring itemDetail;
		MathType::vec2f itemPos;
		int fontId;
		ColorType::COLORF fontColor;
	};

	GameUI::UICanvas* mCanvas = nullptr;
	GameUI::MenuStack* mMenuStack = nullptr;

	GameUI::UIImage* mBackGround = nullptr;
	GameUI::UIImage* mHPBar = nullptr;
	GameUI::UIImage* mMPBar = nullptr;
	GameUI::UINumber* mLevelNumber = nullptr;
	GameUI::UIImage* mLVIMG = nullptr;
	GameUI::UIText* mTexts[30] = {};
	GameUI::UIAnimator* mOption = nullptr;
	GameUI::UITextArea* mDetail = nullptr;
	
	int mCurIndex = 0;
	std::vector<OptionItem> mOptionItem;

	int mFontId24 = -1;
	int mFontId18 = -1;
	int mChineseFontId36 = -1;
	bool mIsFontReady = false;

public:
	PauseMenu() = default;
	~PauseMenu();
	PauseMenu(const PauseMenu&) = delete;
	PauseMenu& operator=(const PauseMenu&) = delete;

	// IMenu 接口
	virtual void OnEnter(GameUI::UICanvas* canvas, GameUI::MenuStack* menuStack) override;
	virtual void OnExit(GameUI::UICanvas* canvas) override;
	virtual void Update(float deltaTime) override;
	virtual void OnKeyDown(WPARAM key) override;

private:
	void _InitFonts();
	void _BuildUI();
	void _RefreshStats();
};
