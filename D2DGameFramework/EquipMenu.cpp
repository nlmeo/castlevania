#include "EquipMenu.h"
#include "GameTool.h"
#include "BackPack.h"
#include "ItemConfig.h"
EquipMenu::~EquipMenu()
{
}

void EquipMenu::_InitFonts()
{
	if (mIsFontReady)
		return;

	mFontId24 = FontMgr.CreateFontFromFile(L"resource\\font\\English.ttf", 24.0f);
	mFontId24B = FontMgr.CreateFontFromFile(L"resource\\font\\English.ttf", 24.0f,true);
	mChineseFontId36 = FontMgr.CreateFontFromFile(L"resource\\font\\Chinese.ttf", 36.0f);
	mIsFontReady = true;


}

void EquipMenu::_BuildUI()
{
	// 背景
	mBackGround = new GameUI::UIImage();
	mBackGround->Create(0, 0, 768, 576, TexMgr.LoadTextureWithoutColor(L"resource\\ui\\runtime\\arms.bmp"));
	mCanvas->AddChild(mBackGround);

	// 分类箭头
	mOption = new GameUI::UIAnimator();
	mOption->SetPosition(0.0f, mCategoryIndex * 45.0f + 115.0f);
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

	mItemOption = new GameUI::UIAnimator();
	mItemOption->SetPosition(0, 270);
	clip.AddFrames({
		{L"resource\\ui\\top\\frame048.bmp", MathType::vec2f(0.0f, 0.0f), 31, 0xffffff},
		{L"resource\\ui\\top\\frame049.bmp", MathType::vec2f(0.0f, 0.0f), 4,  0xffffff},
		{L"resource\\ui\\top\\frame04A.bmp", MathType::vec2f(0.0f, 0.0f), 4,  0xffffff},
		{L"resource\\ui\\top\\frame04B.bmp", MathType::vec2f(0.0f, 0.0f), 4,  0xffffff},
		{L"resource\\ui\\top\\frame04C.bmp", MathType::vec2f(0.0f, 0.0f), 4,  0xffffff},
		{L"resource\\ui\\top\\frame04D.bmp", MathType::vec2f(0.0f, 0.0f), 4,  0xffffff},
		});
	mItemOption->AddClip(L"arrow", clip);
	mItemOption->Play(L"arrow");
	mItemOption->SetActive(false);
	mCanvas->AddChild(mItemOption);



	// 分类文字（初始全灰，OnEnter 后由 Update 驱动首次高亮）
	mTexts[0] = new GameUI::UIText();
	mTexts[0]->Create(L"", mChineseFontId36, ColorType::COLORF::White(), 110.0f, 115.0f);
	mCanvas->AddChild(mTexts[0]);
	mTexts[1] = new GameUI::UIText();
	mTexts[1]->Create(L"", mChineseFontId36, ColorType::COLORF::Gray(), 110.0f, 160.0f);
	mCanvas->AddChild(mTexts[1]);
	mTexts[2] = new GameUI::UIText();
	mTexts[2]->Create(L"", mChineseFontId36, ColorType::COLORF::Gray(), 110.0f, 205.0f);
	mCanvas->AddChild(mTexts[2]);

	mTitle = new GameUI::UITextArea();
	mTitle->Create(
		L"",
		mFontId24B,
		ColorType::COLORF::White(),
		135, 230,
		160, 150
	);
	mTitle->SetAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	mCanvas->AddChild(mTitle);
	
	mCanvas->AddChild(mTexts[3]);
	// 分类图标
	mImage[0] = new GameUI::UIImage();
	mImage[0]->Create(55, 113, 40, 40, TexMgr.LoadTextureWithoutColor(L"resource\\ui\\Icons\\083.bmp"));
	mCanvas->AddChild(mImage[0]);

	mImage[1] = new GameUI::UIImage();
	mImage[1]->Create(55, 158, 40, 40, TexMgr.LoadTextureWithoutColor(L"resource\\ui\\Icons\\124.bmp"));
	mCanvas->AddChild(mImage[1]);

	mImage[2] = new GameUI::UIImage();
	mImage[2]->Create(55, 203, 40, 40, TexMgr.LoadTextureWithoutColor(L"resource\\ui\\Icons\\159.bmp"));
	mCanvas->AddChild(mImage[2]);

	mDetailImg = new GameUI::UIImage();
	mDetailImg->Create(100, 430, 80, 80, TexMgr.LoadTextureWithoutColor(L"resource\\ui\\Icons\\159.bmp"));
	mCanvas->AddChild(mDetailImg);


	// 详情文本（由 _RefreshCategoryView 填充真实数据）
	mDetail = new GameUI::UITextArea();
	mDetail->Create(
		L"",
		mChineseFontId36,
		ColorType::COLORF::White(),
		230.0f, 430.0f,
		380.0f, 150.0f
	);
	mCanvas->AddChild(mDetail);

	for (int i = 0; i < 8; ++i)
	{
		mItems[i] = new GameUI::UIText();
		mItems[i + 8] = new GameUI::UIText();
		float x = (i % 2 == 0) ? 30  + 30: 380 + 30;
		float y = 272 + (int)(i / 2) * 28;
		mItems[i]->Create(L"", mChineseFontId36, ColorType::COLORF::Gray(), x, y);
		mItems[i + 8]->Create(L"", mChineseFontId36, ColorType::COLORF::Gray(), x + 250, y);
		mCanvas->AddChild(mItems[i]);
		mCanvas->AddChild(mItems[i + 8]);
	}

	// 使用背包真实数据填充分类视图
	_RefreshCategoryView();
}

void EquipMenu::OnEnter(GameUI::UICanvas* canvas, GameUI::MenuStack* menuStack)
{
	mCanvas = canvas;
	mMenuStack = menuStack;

	_InitFonts();
	_BuildUI();

}

void EquipMenu::OnExit(GameUI::UICanvas* canvas)
{
	if (mBackGround)     { canvas->RemoveChild(mBackGround);     mBackGround = nullptr; }
	if (mDetail)         { canvas->RemoveChild(mDetail);         mDetail = nullptr; }
	if (mOption)         { canvas->RemoveChild(mOption);         mOption = nullptr; }
	if (mCateGoryOption) { canvas->RemoveChild(mCateGoryOption); mCateGoryOption = nullptr; }
	if (mItemOption)     { canvas->RemoveChild(mItemOption);     mItemOption = nullptr; }
	if (detailIcon)      { canvas->RemoveChild(detailIcon);      detailIcon = nullptr; }
	SAFE_REMOVECHILDE(canvas, mTitle)
	SAFE_REMOVECHILDE(canvas, mDetailImg)
	for (int i = 0; i < 20; i++)
	{
		if (mTexts[i])
		{
			canvas->RemoveChild(mTexts[i]);
			mTexts[i] = nullptr;
		}
	}

	for (int i = 0; i < 3; i++)
	{
		if (mImage[i])
		{
			canvas->RemoveChild(mImage[i]);
			mImage[i] = nullptr;
		}
	}

	for (int i = 0; i < 16; i++)
	{
		if (mItems[i])
		{
			canvas->RemoveChild(mItems[i]);
			mItems[i] = nullptr;
		}
	}

	mCanvas = nullptr;
}

void EquipMenu::_RefreshItemList()
{
	// ── 1. 获取当前分类物品总数 ──
	int totalItems = 0;
	switch (mCategoryIndex)
	{
	case 0:
		totalItems = mIsArms ? (int)BackPack.GetWeapons().size() : (int)BackPack.GetRedSouls().size();
		break;
	case 1:
		totalItems = mIsArms ? (int)BackPack.GetArmors().size() : (int)BackPack.GetBlueSouls().size();
		break;
	case 2:
		totalItems = mIsArms ? (int)BackPack.GetAccessories().size() : (int)BackPack.GetYellowSouls().size();
		break;
	}

	// ── 2. 钳制页码 ──
	// 第一页：卸下装备 + 7 件物品；后续页：满 8 件物品
	int totalPages = (totalItems <= 7) ? 1 : (1 + totalItems / 8);
	if (mPageIndex < 0) mPageIndex = 0;
	if (mPageIndex >= totalPages) mPageIndex = totalPages - 1;

	// ── 3. 填充当前页 8 个槽位 ──
	auto setSlotText = [&](int itemIdx, int slot) {
		std::wstring name;
		std::wstring count;
		switch (mCategoryIndex)
		{
		case 0:
			if (mIsArms) {
				const auto& v = BackPack.GetWeapons();
				if (itemIdx >= 0 && itemIdx < (int)v.size()) { name = v[itemIdx]->name; count = std::to_wstring(v[itemIdx]->item_count); }
			} else {
				const auto& v = BackPack.GetRedSouls();
				if (itemIdx >= 0 && itemIdx < (int)v.size()) { name = v[itemIdx]->name; count = std::to_wstring(v[itemIdx]->item_count); }
			}
			break;
		case 1:
			if (mIsArms) {
				const auto& v = BackPack.GetArmors();
				if (itemIdx >= 0 && itemIdx < (int)v.size()) { name = v[itemIdx]->name; count = std::to_wstring(v[itemIdx]->item_count); }
			} else {
				const auto& v = BackPack.GetBlueSouls();
				if (itemIdx >= 0 && itemIdx < (int)v.size()) { name = v[itemIdx]->name; count = std::to_wstring(v[itemIdx]->item_count); }
			}
			break;
		case 2:
			if (mIsArms) {
				const auto& v = BackPack.GetAccessories();
				if (itemIdx >= 0 && itemIdx < (int)v.size()) { name = v[itemIdx]->name; count = std::to_wstring(v[itemIdx]->item_count); }
			} else {
				const auto& v = BackPack.GetYellowSouls();
				if (itemIdx >= 0 && itemIdx < (int)v.size()) { name = v[itemIdx]->name; count = std::to_wstring(v[itemIdx]->item_count); }
			}
			break;
		}
		mItems[slot]->SetText(name);
		mItems[slot + 8]->SetText(count);
	};

	for (int slot = 0; slot < 8; ++slot)
	{
		// 第一页第一行固定为“卸下装备”
		if (mPageIndex == 0 && slot == 0)
		{
			mItems[0]->SetText(L"- - - - -");
			mItems[8]->SetText(L"");
			continue;
		}

		int itemIdx = (mPageIndex == 0) ? (slot - 1) : (7 + (mPageIndex - 1) * 8 + slot);
		setSlotText(itemIdx, slot);
	}
}

void EquipMenu::_RefreshCategoryView()
{
	// 分类文字高亮
	for (int i = 0; i < 3; i++)
	{
		mTexts[i]->SetColor(
			(i == mCategoryIndex) ? ColorType::COLORF::White() : ColorType::COLORF::Gray());
	}

	// 箭头跟随
	mOption->SetPosition(0.0f, mCategoryIndex * 47.0f + 110.0f);

	// 背景切换
	mBackGround->SetTexture(
		mIsArms
		? TexMgr.LoadTextureWithoutColor(L"resource\\ui\\runtime\\arms.bmp")
		: TexMgr.LoadTextureWithoutColor(L"resource\\ui\\runtime\\soul.bmp")
	);

	// 获取当前装备
	WeaponItem* pWeapon = BackPack.GetEquippedWeapon();
	ArmItem* pBody = BackPack.GetEquippedArmor();
	ArmItem* pAcc = BackPack.GetEquippedAccessory();
	SoulItem* pRedSoul = BackPack.GetEquippedRedSoul();
	SoulItem* pBlueSoul = BackPack.GetEquippedBlueSoul();
	SoulItem* pYellowSoul = BackPack.GetEquippedYellowSoul();

	// 分类名称 + 图标
	if (mIsArms)
	{
		mTexts[0]->SetText(pWeapon == nullptr ? L"- - - - -" : pWeapon->name);
		mTexts[1]->SetText(pBody == nullptr ? L"- - - - -" : pBody->name);
		mTexts[2]->SetText(pAcc == nullptr ? L"- - - - -" : pAcc->name);
		mImage[0]->SetTexture(pWeapon == nullptr ? nullptr : TexMgr.LoadTextureWithoutColor(pWeapon->icon_id.c_str()));
		mImage[1]->SetTexture(pBody == nullptr ? nullptr : TexMgr.LoadTextureWithoutColor(pBody->icon_id.c_str()));
		mImage[2]->SetTexture(pAcc == nullptr ? nullptr : TexMgr.LoadTextureWithoutColor(pAcc->icon_id.c_str()));
	}
	else
	{
		mTexts[0]->SetText(pRedSoul == nullptr ? L"- - - - -" : pRedSoul->name);
		mTexts[1]->SetText(pBlueSoul == nullptr ? L"- - - - -" : pBlueSoul->name);
		mTexts[2]->SetText(pYellowSoul == nullptr ? L"- - - - -" : pYellowSoul->name);
		mImage[0]->SetTexture(pRedSoul == nullptr ? TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\r03.png", 0xffffff) :
			TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\r02.png", 0xffffff));
		mImage[1]->SetTexture(pBlueSoul == nullptr ? TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\b03.png", 0xffffff) :
			TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\b02.png", 0xffffff));
		mImage[2]->SetTexture(pYellowSoul == nullptr ? TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\y03.png", 0xffffff) :
			TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\y02.png", 0xffffff));
	}

	// 标题、详情、物品列表、详情图标
	const wchar_t* detailIcon = L"";
	switch (mCategoryIndex)
	{
	case 0:
		mIsArms ? mTitle->SetText(L"HAND") : mTitle->SetText(L"BULLET");
		mIsArms ? mDetail->SetText(pWeapon == nullptr ? L"" : pWeapon->desc) : mDetail->SetText(pRedSoul == nullptr ? L"" : pRedSoul->desc);
		if (mIsArms)
		{
			mItems[0]->SetText(L"- - - - -");
			mItems[8]->SetText(L"");
			auto items = BackPack.GetWeapons();
			for (int i = 0; i < 7; ++i)
			{
				if (i < (int)items.size())
				{
					mItems[i + 1]->SetText(items[i]->name);
					mItems[i + 9]->SetText(std::to_wstring(items[i]->item_count));
				}
				else
				{
					mItems[i + 1]->SetText(L"");
					mItems[i + 9]->SetText(L"");
				}
			}
			if (pWeapon) detailIcon = pWeapon->icon_id.c_str();
		}
		else
		{
			auto items = BackPack.GetRedSouls();
			for (int i = 0; i < 7; ++i)
			{
				if (i < (int)items.size())
				{
					mItems[i + 1]->SetText(items[i]->name);
					mItems[i + 9]->SetText(std::to_wstring(items[i]->item_count));
				}
				else
				{
					mItems[i + 1]->SetText(L"");
					mItems[i + 9]->SetText(L"");
				}
			}
		}
		break;
	case 1:
		mIsArms ? mTitle->SetText(L"BODY") : mTitle->SetText(L"GUARDIAN");
		mIsArms ? mDetail->SetText(pBody == nullptr ? L"" : pBody->desc) : mDetail->SetText(pBlueSoul == nullptr ? L"" : pBlueSoul->desc);
		if (mIsArms)
		{
			mItems[0]->SetText(L"- - - - -");
			mItems[8]->SetText(L"");
			auto items = BackPack.GetArmors();
			for (int i = 0; i < 7; ++i)
			{
				if (i < (int)items.size())
				{
					mItems[i + 1]->SetText(items[i]->name);
					mItems[i + 9]->SetText(std::to_wstring(items[i]->item_count));
				}
				else
				{
					mItems[i + 1]->SetText(L"");
					mItems[i + 9]->SetText(L"");
				}
			}
			if (pBody) detailIcon = pBody->icon_id.c_str();
		}
		else
		{
			auto items = BackPack.GetBlueSouls();
			for (int i = 0; i < 7; ++i)
			{
				if (i < (int)items.size())
				{
					mItems[i + 1]->SetText(items[i]->name);
					mItems[i + 9]->SetText(std::to_wstring(items[i]->item_count));
				}
				else
				{
					mItems[i + 1]->SetText(L"");
					mItems[i + 9]->SetText(L"");
				}
			}
		}
		break;
	case 2:
		mIsArms ? mTitle->SetText(L"ACCESSORY") : mTitle->SetText(L"ENCHANT");
		mIsArms ? mDetail->SetText(pAcc == nullptr ? L"" : pAcc->desc) : mDetail->SetText(pYellowSoul == nullptr ? L"" : pYellowSoul->desc);
		if (mIsArms)
		{
			mItems[0]->SetText(L"- - - - -");
			mItems[8]->SetText(L"");
			auto items = BackPack.GetAccessories();
			for (int i = 0; i < 7; ++i)
			{
				if (i < (int)items.size())
				{
					mItems[i + 1]->SetText(items[i]->name);
					mItems[i + 9]->SetText(std::to_wstring(items[i]->item_count));
				}
				else
				{
					mItems[i + 1]->SetText(L"");
					mItems[i + 9]->SetText(L"");
				}
			}
			if (pAcc) detailIcon = pAcc->icon_id.c_str();
		}
		else
		{
			auto items = BackPack.GetYellowSouls();
			for (int i = 0; i < 7; ++i)
			{
				if (i < (int)items.size())
				{
					mItems[i + 1]->SetText(items[i]->name);
					mItems[i + 9]->SetText(std::to_wstring(items[i]->item_count));
				}
				else
				{
					mItems[i + 1]->SetText(L"");
					mItems[i + 9]->SetText(L"");
				}
			}
		}
		break;
	}

	if (detailIcon && detailIcon[0] != L'\0')
		mDetailImg->SetTexture(TexMgr.LoadTextureWithoutColor(detailIcon));
	else
		mDetailImg->SetTexture(nullptr);
	

	// 重置所有物品颜色为灰色
	for (int i = 0; i < 8; ++i)
	{
		if (mItems[i]->GetColor().Equal(ColorType::COLORF::White()))
		{
			mItems[i]->SetColor(ColorType::COLORF::Gray());
			mItems[i + 8]->SetColor(ColorType::COLORF::Gray());
		}
	}
}

void EquipMenu::Update(float deltaTime)
{
	if (!mCanvas)
		return;

	// ========== 分类选择模式 ==========
	if (!mIsChoosed)
	{
		const int oldCategory = mCategoryIndex;
		const bool oldIsArms = mIsArms;
		

		if (GT::KeyJustDown('W'))
		{
			mCategoryIndex--;
			if (mCategoryIndex < 0)
				mCategoryIndex = 2;
		}
		else if (GT::KeyJustDown('S'))
		{
			mCategoryIndex++;
			if (mCategoryIndex > 2)
				mCategoryIndex = 0;
		}

		if (GT::KeyJustDown('A') || GT::KeyJustDown('D'))
		{
			mIsArms = !mIsArms;
		}

		// J 键：进入道具选择模式
		if (mCategoryIndex != oldCategory || mIsArms != oldIsArms)
			GT::PlaySE(SE_MENU_CHANGE);

		if (GT::KeyJustDown('J'))
		{
			GT::PlaySE(SE_MENU_CHOOSE);
			mIsChoosed = true;
			mItemIndex = 0;
			mPageIndex = 0;
			_RefreshItemList();
			mItemOption->SetActive(true);
			mItemOption->SetPosition(0, 270);
		}

		// 刷新分类视图
		_RefreshCategoryView();
	}
	// ========== 道具选择模式 ==========
	else
	{
		// 获取当前分类的物品总数
		int totalItems = 0;
		switch (mCategoryIndex)
		{
		case 0:
			totalItems = mIsArms ? (int)BackPack.GetWeapons().size() : (int)BackPack.GetRedSouls().size();
			break;
		case 1:
			totalItems = mIsArms ? (int)BackPack.GetArmors().size() : (int)BackPack.GetBlueSouls().size();
			break;
		case 2:
			totalItems = mIsArms ? (int)BackPack.GetAccessories().size() : (int)BackPack.GetYellowSouls().size();
			break;
		}

		// ── 总页数 ──
		// 第一页：卸下装备 + 7 件物品；后续页：满 8 件物品
		int totalPages = (totalItems <= 7) ? 1 : (1 + totalItems / 8);

		// ── 当前页最大可选槽位 ──
		auto computeMaxSlot = [&](int page) -> int {
			if (page == 0)
				return (totalItems > 7) ? 7 : totalItems;
			int itemsOnPage = totalItems - (7 + (page - 1) * 8);
			if (itemsOnPage < 0) itemsOnPage = 0;
			if (itemsOnPage > 8) itemsOnPage = 8;
			return itemsOnPage - 1;
		};
		int curMaxSlot = computeMaxSlot(mPageIndex);

		// ── 槽位 → 物品索引（-1 表示“卸下装备”槽）──
		auto getItemIdx = [&](int slot) -> int {
			if (mPageIndex == 0)
				return (slot == 0) ? -1 : (slot - 1);
			return 7 + (mPageIndex - 1) * 8 + slot;
		};

		// ── W/S/A/D 移动 + 滚动翻页 ──
		const int oldItemIndex = mItemIndex;
		const int oldPageIndex = mPageIndex;
		if (curMaxSlot >= 0)
		{
			if (mItemIndex > curMaxSlot) mItemIndex = curMaxSlot;
			if (mItemIndex < 0) mItemIndex = 0;

			// S: 向下移动一行；已在最后一行且还有下一页时翻到下一页
			if (GT::KeyJustDown('S'))
			{
				int target = mItemIndex + 2;
				if (target <= curMaxSlot)
				{
					mItemIndex = target;
				}
				else if (mPageIndex < totalPages - 1)
				{
					mPageIndex++;
					mItemIndex = 0;
					_RefreshItemList();
				}
			}
			// W: 向上移动一行；已在第一行且还有上一页时翻回上一页
			else if (GT::KeyJustDown('W'))
			{
				if (mItemIndex >= 2)
				{
					mItemIndex -= 2;
				}
				else if (mPageIndex > 0)
				{
					mPageIndex--;
					mItemIndex = computeMaxSlot(mPageIndex);
					_RefreshItemList();
				}
			}

			// A/D: 左右切换，仅目标位置有效时才执行
			if (GT::KeyJustDown('A') || GT::KeyJustDown('D'))
			{
				int target = mItemIndex ^ 1;
				if (target <= curMaxSlot)
					mItemIndex = target;
			}
		}
		else
		{
			mItemIndex = 0;
		}

		// ── 更新 itemOption 箭头位置 ──
		if (mItemIndex != oldItemIndex || mPageIndex != oldPageIndex)
			GT::PlaySE(SE_MENU_CHANGE);

		{
			float x = (mItemIndex % 2 == 0) ? 0.0f : 350.0f;
			float y = 266.0f + (mItemIndex / 2) * 28.0f;
			mItemOption->SetPosition(x, y);
		}

		// ── 高亮当前选中项 ──
		for (int i = 0; i < 8; ++i)
		{
			bool isSelected = (i == mItemIndex);
			mItems[i]->SetColor(isSelected ? ColorType::COLORF::White() : ColorType::COLORF::Gray());
			mItems[i + 8]->SetColor(isSelected ? ColorType::COLORF::White() : ColorType::COLORF::Gray());
		}

		// 更新详情文字
		{
			const wchar_t* desc = L"";
			const wchar_t* icon = L"";
			int itemIdx = getItemIdx(mItemIndex);
			if (itemIdx >= 0)
			{
				// 普通物品：从列表获取描述
				switch (mCategoryIndex)
				{
				case 0:
				{
					if (mIsArms) {
						const auto& v = BackPack.GetWeapons();
						if (itemIdx < (int)v.size()) desc = v[itemIdx]->desc.c_str();
						if (itemIdx < (int)v.size()) icon = v[itemIdx]->icon_id.c_str();
					} else {
						const auto& v = BackPack.GetRedSouls();
						if (itemIdx < (int)v.size()) desc = v[itemIdx]->desc.c_str();
					}
					break;
				}
				case 1:
				{
					if (mIsArms) {
						const auto& v = BackPack.GetArmors();
						if (itemIdx < (int)v.size()) desc = v[itemIdx]->desc.c_str();
						if (itemIdx < (int)v.size()) icon = v[itemIdx]->icon_id.c_str();
					} else {
						const auto& v = BackPack.GetBlueSouls();
						if (itemIdx < (int)v.size()) desc = v[itemIdx]->desc.c_str();
					}
					break;
				}
				case 2:
				{
					if (mIsArms) {
						const auto& v = BackPack.GetAccessories();
						if (itemIdx < (int)v.size()) desc = v[itemIdx]->desc.c_str();
						if (itemIdx < (int)v.size()) icon = v[itemIdx]->icon_id.c_str();
					} else {
						const auto& v = BackPack.GetYellowSouls();
						if (itemIdx < (int)v.size()) desc = v[itemIdx]->desc.c_str();
					}
					break;
				}
				}
			}
			mDetail->SetText(desc);
			if (icon && icon[0] != L'\0')
				mDetailImg->SetTexture(TexMgr.LoadTextureWithoutColor(icon));
			else
				mDetailImg->SetTexture(nullptr);
	
		}

		// J 键：装备 / 卸下
		if (GT::KeyJustDown('J'))
		{
			GT::PlaySE(SE_MENU_CHOOSE);
			int itemIdx = getItemIdx(mItemIndex);
			if (itemIdx < 0)
			{
				// 卸下当前装备（仅第一页第一行）
				switch (mCategoryIndex)
				{
				case 0: mIsArms ? BackPack.UnequipWeapon() : BackPack.UnequipRedSoul(); break;
				case 1: mIsArms ? BackPack.UnequipArmor() : BackPack.UnequipBlueSoul(); break;
				case 2: mIsArms ? BackPack.UnequipAccessory() : BackPack.UnequipYellowSoul(); break;
				}
			}
			else
			{
				// 装备选中物品
				int itemId = -1;

				switch (mCategoryIndex)
				{
				case 0:
				{
					if (mIsArms) {
						const auto& v = BackPack.GetWeapons();
						if (itemIdx < (int)v.size()) itemId = v[itemIdx]->id;
					}
					else {
						const auto& v = BackPack.GetRedSouls();
						if (itemIdx < (int)v.size()) itemId = v[itemIdx]->id;
					}
					break;
				}
				case 1:
				{
					if (mIsArms) {
						const auto& v = BackPack.GetArmors();
						if (itemIdx < (int)v.size()) itemId = v[itemIdx]->id;
					}
					else {
						const auto& v = BackPack.GetBlueSouls();
						if (itemIdx < (int)v.size()) itemId = v[itemIdx]->id;
					}
					break;
				}
				case 2:
				{
					if (mIsArms) {
						const auto& v = BackPack.GetAccessories();
						if (itemIdx < (int)v.size()) itemId = v[itemIdx]->id;
					}
					else {
						const auto& v = BackPack.GetYellowSouls();
						if (itemIdx < (int)v.size()) itemId = v[itemIdx]->id;
					}
					break;
				}
				}

				if (itemId > 0)
				{
					switch (mCategoryIndex)
					{
					case 0: mIsArms ? BackPack.EquipWeapon(itemId) : BackPack.EquipRedSoul(itemId); break;
					case 1: mIsArms ? BackPack.EquipArmor(itemId) : BackPack.EquipBlueSoul(itemId); break;
					case 2: mIsArms ? BackPack.EquipAccessory(itemId) : BackPack.EquipYellowSoul(itemId); break;
					}
				}
			}

			// 刷新列表显示
			_RefreshItemList();

			// 刷新分类视图
			_RefreshCategoryView();
			mItemOption->SetActive(false);
			mIsChoosed = false;
		}

		// K 键：返回分类选择
		if (GT::KeyJustDown('K'))
		{
			GT::PlaySE(SE_MENU_BACK);
			mIsChoosed = false;
			mItemOption->SetActive(false);
			return;
		}
	}

	// ========== 全局按键 ==========
	if (mMenuStack)
	{
		// 分类模式下 K → 返回上级菜单
		if (!mIsChoosed && GT::KeyJustDown('K'))
		{
			GT::PlaySE(SE_MENU_BACK);
			mMenuStack->Pop();
			return;
		}
		// 全局：Enter → 关闭所有菜单
		if (GT::KeyJustDown(VK_RETURN))
		{
			GT::PlaySE(SE_MENU_BACK);
			mMenuStack->Clear();
		}
	}
}

void EquipMenu::OnKeyDown(WPARAM key)
{
}
