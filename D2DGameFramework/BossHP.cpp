#include "BossHP.h"
#include "D2DCore/TextureManager.h"
BossHP::~BossHP()
{
	mBackGround = nullptr;
	mHPBar = nullptr;
}

void BossHP::Init(GameUI::UICanvas* p, float maxHP)
{
	if (p == nullptr) return;
	p->SetScaleWithCamera();

	// 创建血条背景
	mBackGround = new GameUI::UIImage();
	mBackGround->Create(84, 80, 600, 20, TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\bosshp.png", 0xffffff));
	mBackGround->SetOpacity(0.8f);
	p->AddChild(mBackGround);

	// 创建血条本体
	mHPBar = new GameUI::UIImage();
	mHPBar->Create(84 + 2, 80 + 2, 600 - 2 - 2, 16, TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\hpbar.png", 0xffffff));
	mHPBar->SetOpacity(0.8f);
	p->AddChild(mHPBar);

	mBufferBlock.Init(p);

	mLastHP = maxHP;

	mIsInit = true;
}

void BossHP::Update(float currentHP, float maxHP,float deltaTime)
{
	if (!mIsInit) return;
	if (currentHP <= 0 || maxHP <= 0)
	{
		Disappear();
		return;
	}
	mBackGround->SetActive(true);
	mHPBar->SetActive(true);
	currentHP = currentHP < 0 ? 0 : currentHP;
	float curSize = currentHP / maxHP * 596;
	mHPBar->SetSize(curSize, 16);

	if (currentHP != mLastHP)
	{
		
		float blockSize = (mLastHP - currentHP) / maxHP * 596;
		float blockPosX = 86 + curSize;

		if(blockSize >1.0f)
			mBufferBlock.Spawn(blockPosX, 82, blockSize, 16);
	}




	
	mBufferBlock.Update(deltaTime);
	mLastHP = currentHP;

}

void BossHP::Disappear()
{
	if (!mIsInit) return;
	mBackGround->SetActive(false);
	mHPBar->SetActive(false);
	mBufferBlock.Clear();
	mLastHP = -1;
}
