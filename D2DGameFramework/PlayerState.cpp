#include "PlayerState.h"
#include "GameUI/UIImage.h"
#include "GameUI/UINumber.h"
#include "D2DCore/TextureManager.h"
#include "Player.h"
#include "Soma.h"
#include "GameUI/ResourceBar.h"
PlayerState::~PlayerState()
{
	// 指针由创建者管理，这里只置空
	mBackGround = nullptr;
	mHPBar = nullptr;
	mMPBar = nullptr;
	mLevelNumber = nullptr;
}

void PlayerState::Init(GameUI::UICanvas* p)
{
	if (p == nullptr) return;
	p->SetScaleWithCamera();

	// 创建血条背景
	mBackGround = new GameUI::UIImage();
	mBackGround->Create(20, 5, 333, 69, TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\01.bmp"));
	p->AddChild(mBackGround);

	// 创建血条本体
	mHPBar = new GameUI::UIImage();
	mHPBar->Create(113, 18, 192, 9, TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\05.bmp"));
	p->AddChild(mHPBar);

	// 创建蓝条
	mMPBar = new GameUI::UIImage();
	mMPBar->Create(113, 33, 192, 9, TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\06.bmp"));
	p->AddChild(mMPBar);

	// 创建等级数字
	mLevelNumber = new GameUI::UINumber();
	D2DCore::Texture* texs[10] = {};
	// ... 加载数字纹理 ...
	texs[0] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\07.bmp");
	texs[1] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\08.bmp");
	texs[2] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\09.bmp");
	texs[3] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\10.bmp");
	texs[4] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\11.bmp");
	texs[5] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\12.bmp");
	texs[6] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\13.bmp");
	texs[7] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\14.bmp");
	texs[8] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\15.bmp");
	texs[9] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\16.bmp");

	mLevelNumber->CreateWithTextures(40, 20, 15, 20, texs);
	mLevelNumber->SetDigitCount(3);
	mLevelNumber->SetNumber(120);
	p->AddChild(mLevelNumber);


	mIsInit = true;
	Update();
}

//int i = 0;
//int j = 0;

void PlayerState::Update()
{
	if (!mIsInit) return;
	auto* player = dynamic_cast<Soma*>(MainPlayer);
	if (!player) return;
	const auto& attr = player->GetAttributes();
	GameUI::SetResourceBarValue(mHPBar, attr.cur_hp, attr.hp);
	GameUI::SetResourceBarValue(mMPBar, attr.cur_mp, static_cast<float>(attr.mp));
	const int currentHP = static_cast<int>(attr.cur_hp);
	if (mLevelNumber->GetNumber() != currentHP) mLevelNumber->SetNumber(currentHP);
}
