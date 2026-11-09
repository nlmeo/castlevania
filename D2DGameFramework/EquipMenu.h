#pragma once
#include "GameUI/UICanvas.h"
#include "GameUI/UIImage.h"
#include "GameUI/UIText.h"
#include "GameUI/UIAnimator.h"
#include "GameUI/UITextArea.h"
#include "GameUI/IMenu.h"
#include "GameUI/MenuStack.h"
#include "D2DCore/TextureManager.h"
#include "D2DCore/FontRenderer.h"
#include <vector>
#include <string>


class EquipMenu : public GameUI::IMenu
{
	struct OptionItem
	{
		D2DCore::Texture* itemIcon; //图标
		std::wstring itemName;
		std::wstring itemDetail;
		int num;
	};

	GameUI::UICanvas* mCanvas = nullptr;
	GameUI::MenuStack* mMenuStack = nullptr;

	GameUI::UIImage* mBackGround = nullptr;
	GameUI::UIText* mTexts[20] = {};
	GameUI::UIImage* mImage[3] = {};
	GameUI::UIImage* detailIcon = nullptr;
	GameUI::UIAnimator* mCateGoryOption = nullptr;
	GameUI::UIAnimator* mItemOption = nullptr;
	GameUI::UIAnimator* mOption = nullptr;
	GameUI::UITextArea* mDetail = nullptr;
	GameUI::UITextArea* mTitle = nullptr;
	GameUI::UIImage* mDetailImg = nullptr;
	bool mIsArms = true;
	int mCategoryIndex = 0;
	bool mIsChoosed = false;
	
	int mItemIndex = 0;
	int mPageIndex = 0;
	GameUI::UIText* mItems[16] = {};


	int mFontId24 = -1;
	int mFontId24B = -1;
	int mChineseFontId36 = -1;
	bool mIsFontReady = false;

public:
	EquipMenu() = default;
	~EquipMenu();
	EquipMenu(const EquipMenu&) = delete;
	EquipMenu& operator=(const EquipMenu&) = delete;

	// IMenu 接口
	virtual void OnEnter(GameUI::UICanvas* canvas, GameUI::MenuStack* menuStack) override;
	virtual void OnExit(GameUI::UICanvas* canvas) override;
	virtual void Update(float deltaTime) override;
	virtual void OnKeyDown(WPARAM key) override;
	
private:
	void _InitFonts();
	void _BuildUI();
	void _RefreshItemList();
	void _RefreshCategoryView();
};
