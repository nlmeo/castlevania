#include "PauseMenu.h"
#include "EquipMenu.h"
#include "GameTool.h"
#include "Player.h"
#include "Soma.h"
#include "SaveManager.h"
#include "GameUI/ResourceBar.h"


PauseMenu::~PauseMenu()
{
}

void PauseMenu::_InitFonts()
{
	if (mIsFontReady)
		return;

	mFontId24 = FontMgr.CreateFontFromFile(L"resource\\font\\English.ttf", 24.0f);
	mFontId18 = FontMgr.CreateFontFromFile(L"resource\\font\\English.ttf", 18.0f);
	mChineseFontId36 = FontMgr.CreateFontFromFile(L"resource\\font\\Chinese.ttf", 36.0f);
	mIsFontReady = true;
}

void PauseMenu::_BuildUI()
{
	// 背景
	mBackGround = new GameUI::UIImage();
	mBackGround->Create(0, 0, 768, 576, TexMgr.LoadTextureWithoutColor(L"resource\\ui\\menu\\Pause.png"));
	mCanvas->AddChild(mBackGround);

	// 血条
	mHPBar = new GameUI::UIImage();
	mHPBar->Create(504, 55, 192, 9, TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\05.bmp"));
	mCanvas->AddChild(mHPBar);

	// 蓝条
	mMPBar = new GameUI::UIImage();
	mMPBar->Create(504, 79, 192, 9, TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\06.bmp"));
	mCanvas->AddChild(mMPBar);

	// 等级图标
	mLVIMG = new GameUI::UIImage();
	mLVIMG->Create(100, 55, 66, 39, TexMgr.LoadTextureWithoutColor(L"resource\\ui\\profile\\frame00A.png", 0xffffff));
	mCanvas->AddChild(mLVIMG);

	// 等级数字
	mLevelNumber = new GameUI::UINumber();
	D2DCore::Texture* texs[10] = {};
	texs[0] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\profile\\frame000.png", 0xffffff);
	texs[1] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\profile\\frame001.png", 0xffffff);
	texs[2] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\profile\\frame002.png", 0xffffff);
	texs[3] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\profile\\frame003.png", 0xffffff);
	texs[4] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\profile\\frame004.png", 0xffffff);
	texs[5] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\profile\\frame005.png", 0xffffff);
	texs[6] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\profile\\frame006.png", 0xffffff);
	texs[7] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\profile\\frame007.png", 0xffffff);
	texs[8] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\profile\\frame008.png", 0xffffff);
	texs[9] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\profile\\frame009.png", 0xffffff);

	mLevelNumber->CreateWithTextures(170, 55, 24, 39, texs);
	mLevelNumber->SetDigitCount(2);
	mLevelNumber->SetNumber(12);
	mCanvas->AddChild(mLevelNumber);

	// 属性文字
	mTexts[0] = new GameUI::UIText();
	mTexts[0]->Create(L"H P    3 0 0 /  3 0 0", mFontId24, ColorType::COLORF::White(), 240.0f, 24.0f);
	mCanvas->AddChild(mTexts[0]);

	mTexts[1] = new GameUI::UIText();
	mTexts[1]->Create(L"M P    3 0 0 /  3 0 0", mFontId24, ColorType::COLORF::White(), 240.0f, 54.0f);
	mCanvas->AddChild(mTexts[1]);

	mTexts[2] = new GameUI::UIText();
	mTexts[2]->Create(L"A T K", mFontId24, ColorType::COLORF::Red(), 100.0f + 8, 100.0f - 4);
	mCanvas->AddChild(mTexts[2]);

	mTexts[3] = new GameUI::UIText();
	mTexts[3]->Create(L"D E F", mFontId24, ColorType::COLORF::Blue(), 100.0f + 8, 130.0f - 4);
	mCanvas->AddChild(mTexts[3]);

	mTexts[4] = new GameUI::UIText();
	mTexts[4]->Create(L"S T R       1 5    1 5             N E X T", mFontId24, ColorType::COLORF::White(), 100.0f + 8, 160.0f - 4);
	mCanvas->AddChild(mTexts[4]);

	mTexts[5] = new GameUI::UIText();
	mTexts[5]->Create(L"C O N       1 5    1 5                  1 2 3 4 5", mFontId24, ColorType::COLORF::White(), 100.0f + 8, 190.0f - 4);
	mCanvas->AddChild(mTexts[5]);

	mTexts[6] = new GameUI::UIText();
	mTexts[6]->Create(L"I  N T       1 5    1 5             G O L D", mFontId24, ColorType::COLORF::White(), 100.0f + 8, 220.0f - 4);
	mCanvas->AddChild(mTexts[6]);

	mTexts[7] = new GameUI::UIText();
	mTexts[7]->Create(L"L C K       1 5    1 5                  1 2 3 4 5", mFontId24, ColorType::COLORF::White(), 100.0f + 8, 250.0f - 4);
	mCanvas->AddChild(mTexts[7]);

	mTexts[8] = new GameUI::UIText();
	mTexts[8]->Create(L"              1 5    1 5             E X P", mFontId24, ColorType::COLORF::White(), 100.0f + 4, 100.0f - 4);
	mCanvas->AddChild(mTexts[8]);

	mTexts[9] = new GameUI::UIText();
	mTexts[9]->Create(L"              1 5    1 5                  1 2 3 4 5", mFontId24, ColorType::COLORF::White(), 100.0f + 4, 130.0f - 4);
	mCanvas->AddChild(mTexts[9]);

	mTexts[10] = new GameUI::UIText();
	mTexts[10]->Create(L"T I M E    : 2 2 : 3 4 : 2 5", mFontId24, ColorType::COLORF::White(), 400.0f - 24, 220.0f + 70);
	mCanvas->AddChild(mTexts[10]);

	mTexts[11] = new GameUI::UIText();
	mTexts[11]->Create(L"R A T E   : 1 4 . 5 %", mFontId24, ColorType::COLORF::White(), 400.0f - 24, 220.0f + 100);
	mCanvas->AddChild(mTexts[11]);

	// 菜单选项（初始全灰色，OnEnter 根据 mCurIndex 恢复高亮）
	mOptionItem =
	{
		{L"Equip",    L"设定魂、武器和防具等装备", {108, 312}, mFontId24, ColorType::COLORF::Gray()},
		{L"Use Item", L"查看和使用道具。", {108, 338}, mFontId24, ColorType::COLORF::Gray()},
		{L"Ability",  L"调整能力形魂是否生效。", {108, 368}, mFontId24, ColorType::COLORF::Gray()},
		{L"Sleep",    L"保存当前进度并返回标题。", {108, 398}, mFontId24, ColorType::COLORF::Gray()},
		{L"Config",   L"更改按键设定。", {108, 428}, mFontId24, ColorType::COLORF::Gray()},
		{L"Enemy",    L"敌人图鉴，能查看已获得的敌人信息", {108, 458}, mFontId24, ColorType::COLORF::Gray()},
		{L"Library",  L"可查看游戏用语相关说明", {108, 488}, mFontId24, ColorType::COLORF::Gray()},
		{L"Save",     L"保存当前游戏进度。", {108, 518}, mFontId24, ColorType::COLORF::Gray()},
	};

	for (int i = 0; i < (int)mOptionItem.size(); i++)
	{
		mTexts[12 + i] = new GameUI::UIText();
		mTexts[12 + i]->Create(
			mOptionItem[i].itemName, mOptionItem[i].fontId,
			mOptionItem[i].fontColor,
			mOptionItem[i].itemPos.x, mOptionItem[i].itemPos.y);
		mCanvas->AddChild(mTexts[12 + i]);
	}

	// 箭头动画
	mOption = new GameUI::UIAnimator();
	mOption->SetPosition(50.0f, mOptionItem[mCurIndex].itemPos.y + 10);

	GameUI::UIAnimationClip clip;
	clip.frameRate = 60.0f;
	clip.loop = true;
	clip.AddFrames({
		{L"resource\\ui\\top\\frame048.bmp", MathType::vec2f(0.0f, 0.0f), 31, 0xffffff},
		{L"resource\\ui\\top\\frame049.bmp", MathType::vec2f(0.0f, 0.0f), 4,  0xffffff},
		{L"resource\\ui\\top\\frame04A.bmp", MathType::vec2f(0.0f, 0.0f), 4,  0xffffff},
		{L"resource\\ui\\top\\frame04B.bmp", MathType::vec2f(0.0f, 0.0f), 4,  0xffffff},
		{L"resource\\ui\\top\\frame04C.bmp", MathType::vec2f(0.0f, 0.0f), 4,  0xffffff},
		{L"resource\\ui\\top\\frame04D.bmp", MathType::vec2f(0.0f, 0.0f), 4,  0xffffff},
		});

	mOption->AddClip(L"arrow", clip);
	mOption->Play(L"arrow");
	mCanvas->AddChild(mOption);

	// 详情文本
	mDetail = new GameUI::UITextArea();
	mDetail->Create(
		mOptionItem[mCurIndex].itemDetail,
		mChineseFontId36,
		ColorType::COLORF::White(),
		330.0f, 430.0f,
		380.0f, 150.0f
	);
	mCanvas->AddChild(mDetail);
}

void PauseMenu::OnEnter(GameUI::UICanvas* canvas, GameUI::MenuStack* menuStack)
{
	mCanvas = canvas;
	mMenuStack = menuStack;

	_InitFonts();
	_BuildUI();
	_RefreshStats();

	// 恢复选中状态（数据 + UI）
	for (int i = 0; i < (int)mOptionItem.size(); i++)
	{
		if (i == mCurIndex)
		{
			mOptionItem[i].fontColor = ColorType::COLORF::White();
			mTexts[12 + i]->SetColor(ColorType::COLORF::White());
		}
		else
		{
			mOptionItem[i].fontColor = ColorType::COLORF::Gray();
			mTexts[12 + i]->SetColor(ColorType::COLORF::Gray());
		}
	}
}

void PauseMenu::OnExit(GameUI::UICanvas* canvas)
{
	// 移除所有 UI 子节点（Canvas 会销毁它们）
	if (mBackGround)   { canvas->RemoveChild(mBackGround);   mBackGround = nullptr; }
	if (mHPBar)        { canvas->RemoveChild(mHPBar);        mHPBar = nullptr; }
	if (mMPBar)        { canvas->RemoveChild(mMPBar);        mMPBar = nullptr; }
	if (mLVIMG)        { canvas->RemoveChild(mLVIMG);        mLVIMG = nullptr; }
	if (mLevelNumber)  { canvas->RemoveChild(mLevelNumber);  mLevelNumber = nullptr; }
	if (mDetail)       { canvas->RemoveChild(mDetail);       mDetail = nullptr; }
	if (mOption)       { canvas->RemoveChild(mOption);       mOption = nullptr; }

	for (int i = 0; i < 30; i++)
	{
		if (mTexts[i])
		{
			canvas->RemoveChild(mTexts[i]);
			mTexts[i] = nullptr;
		}
	}

	mCanvas = nullptr;
}

void PauseMenu::_RefreshStats()
{
	auto* player = dynamic_cast<Soma*>(MainPlayer);
	if (!player) return;
	const auto& attr = player->GetAttributes();
	GameUI::SetResourceBarValue(mHPBar, attr.cur_hp, attr.hp);
	GameUI::SetResourceBarValue(mMPBar, attr.cur_mp, static_cast<float>(attr.mp));
	
	
	auto formatDigits = [](int value) -> std::wstring
		{
			std::wstring s = std::to_wstring(value);
			std::wstring result;
			for (size_t i = 0; i < s.size(); ++i)
			{
				if (i > 0) result += L' ';
				result += s[i];
			}
			return result;
		};



 	std::wstring hpStr = L"H P    ";
	hpStr += formatDigits(attr.cur_hp);
	hpStr += L" /  ";
	hpStr += formatDigits(attr.hp);
	mTexts[0]->SetText(hpStr);
	
	std::wstring mpStr = L"M P    ";
	mpStr += formatDigits(static_cast<int>(attr.cur_mp));
	mpStr += L" /  ";
	mpStr += formatDigits(attr.mp);
	mTexts[1]->SetText(mpStr);
}

void PauseMenu::Update(float deltaTime)
{
	if (mOptionItem.empty() || !mCanvas) return;
	_RefreshStats();

	
	//箭头改变
	int oldIndex = mCurIndex;

	if (GT::KeyJustDown('W'))
	{
		mCurIndex--;
		if (mCurIndex < 0)
			mCurIndex = (int)mOptionItem.size() - 1;
	}
	else if (GT::KeyJustDown('S'))
	{
		mCurIndex++;
		if (mCurIndex >= (int)mOptionItem.size())
			mCurIndex = 0;
	}

	if (mCurIndex != oldIndex)
	{
		GT::PlaySE(SE_MENU_CHANGE);
		// 更新选项颜色
		for (int i = 0; i < (int)mOptionItem.size(); i++)
		{
			mTexts[12 + i]->SetColor(
				(i == mCurIndex) ? ColorType::COLORF::White() : ColorType::COLORF::Gray());
		}

		// 箭头跟随
		mOption->SetPosition(50.0f, mOptionItem[mCurIndex].itemPos.y + 10);

		// 更新详情
		mDetail->SetText(mOptionItem[mCurIndex].itemDetail);

		
	}


	if (mMenuStack)
	{
		// J 键：进入子菜单
		if (GT::KeyJustDown('J'))
		{
			GT::PlaySE(SE_MENU_CHOOSE);
			switch (mCurIndex)
			{
			case 0: // Equip
				mMenuStack->Push(std::make_unique<EquipMenu>());
				break;
			case 3: // Sleep
			case 7: // Save
			{
				const bool saved = SaveMgr.SaveGame(SaveMgr.GetCurrentSlot());
				mDetail->SetText(saved ? L"存档完成。" : L"存档失败，请重试。");
				if (!saved) GT::PlaySE(SE_MENU_WRONG);
				else if (mCurIndex == 3) GT::SwitchTo("start");
				break;
			}
			}
		}
		else
		{
			// Enter 键：返回上一级（出栈）
			if (GT::KeyJustDown('K'))
			{
				GT::PlaySE(SE_MENU_BACK);
				mMenuStack->Pop();
			}
			else if (GT::KeyJustDown(VK_RETURN))
			{
				GT::PlaySE(SE_MENU_BACK);
				mMenuStack->Clear();
			}

		}
	}

	


}

void PauseMenu::OnKeyDown(WPARAM key)
{
	
}
