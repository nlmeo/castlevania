#include "HitFlash.h"
#include "GameObject.h"
#include "SpriteRender.h"
#include "Animator.h"

namespace GamePlay
{
	HitFlash::HitFlash(GameObject* pObj)
		: Component(pObj)
	{
		mType = COMTYPE::ComType_HitFlash;

		mFlashColor = { 1.0f, 0.2f, 0.2f, 1.0f };

		mpAnimator = mOwner->GetComponent<Animator>();
		if (mpAnimator == nullptr)
		{
			mpSpriteRender = mOwner->GetComponent<SpriteRender>();
		}
	}

	void HitFlash::TriggerHit()
	{
		mIsFlashing = true;
		mFlashElapsed = 0.0f;
		mFlashDuration = mPeakTime + mFadeDuration;

		ApplyTint(mFlashColor.r, mFlashColor.g, mFlashColor.b, 0.0f);
	}

	void HitFlash::Update(float deltaTime)
	{
		if (!mIsFlashing)
			return;

		mFlashElapsed += deltaTime;

		float intensity; 

		if (mFlashElapsed <= mPeakTime)
		{
			intensity = (mFlashElapsed / mPeakTime) * mMaxAlpha;
		}
		else if (mFlashElapsed <= mFlashDuration)
		{
			float t = (mFlashElapsed - mPeakTime) / mFadeDuration;
			intensity = mMaxAlpha * (1.0f - t * t);
		}
		else
		{
			intensity = 0.0f;
			mIsFlashing = false;
		}

		ApplyTint(mFlashColor.r, mFlashColor.g, mFlashColor.b, intensity);
	}

	void HitFlash::ApplyTint(float r, float g, float b, float a)
	{
		if (mpAnimator != nullptr)
		{
			mpAnimator->SetTintColor(r, g, b, a);
		}
		else if (mpSpriteRender != nullptr)
		{
			mpSpriteRender->SetTintColor(r, g, b, a);
		}
	}
}
