#include "FloatingNumberPool.h"
#include "../D2DCore/TextureManager.h"
FloatingNumberPool::FloatingNumberPool()
{
}

void FloatingNumberPool::InitWithTextures(D2DCore::Texture* digitTexs[10])
{
	if (mInitialized) return;

	for (int i = 0; i < 10; ++i)
	{
		mDmgPool[i] = new GameUI::UIFloatingNumber();
		mDmgPool[i]->CreateWithTextures(0, 0, 18, 24, digitTexs, 0);
		mDmgPool[i]->SetActive(false);
	}

	mInitialized = true;
}

void FloatingNumberPool::InitWithStrip(D2DCore::Texture* stripTex, float digitW, float digitH)
{
	if (mInitialized) return;

	for (int i = 0; i < 10; ++i)
	{
		mDmgPool[i] = new GameUI::UIFloatingNumber();
		mDmgPool[i]->Create(0, 0, digitW, digitH, stripTex, 0);
		mDmgPool[i]->SetActive(false);
	}

	mInitialized = true;
}

void FloatingNumberPool::Spawn(int number, float startX, float startY,
	float floatHeight, float duration, bool isUseWordPos)
{
	if (!mInitialized) return;

	auto* fn = mDmgPool[mPoolIdx];
	mPoolIdx = (mPoolIdx + 1) % 10;

	fn->Spawn(number, startX, startY, floatHeight, duration,isUseWordPos);
}

void FloatingNumberPool::Update(float dt)
{
	if (!mInitialized) return;

	for (int i = 0; i < 10; ++i)
	{
		if (mDmgPool[i]->IsActive())
		{
			mDmgPool[i]->Update(dt);
		}
	}
}

void FloatingNumberPool::Clear()
{
	for (auto* number : mDmgPool) if (number) number->SetActive(false);
	mPoolIdx = 0;
}

void FloatingNumberPool::Render()
{
	if (!mInitialized) return;

	for (int i = 0; i < 10; ++i)
	{
		if (mDmgPool[i]->IsActive() && !mDmgPool[i]->IsFinished())
		{
			mDmgPool[i]->Render();
		}
	}
}
