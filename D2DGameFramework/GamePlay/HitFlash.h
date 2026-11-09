#pragma once
#ifndef __GAME_PLAY_HIT_FLASH_H__
#define __GAME_PLAY_HIT_FLASH_H__

#include "Component.h"
#include "../TypeDefine/Color.h"

namespace GamePlay
{
	
	typedef class HitFlash : public Component
	{
	public:
		HitFlash(class GameObject* pObj);

		void Update(float deltaTime);

		
		void TriggerHit();

		
		void SetFlashColor(float r, float g, float b)
		{
			mFlashColor = { r, g, b, 1.0f };
		}

		
		void SetFlashTiming(float peakTime, float fadeTime, float maxAlpha)
		{
			mPeakTime = peakTime;
			mFadeDuration = fadeTime;
			mMaxAlpha = maxAlpha;
		}

		
		bool IsFlashing() const { return mIsFlashing; }

	private:
		
		void ApplyTint(float r, float g, float b, float a);

		class SpriteRender* mpSpriteRender = nullptr;
		class Animator* mpAnimator = nullptr;

		ColorType::COLORF mFlashColor;
		float mPeakTime = 0.05f;
		float mFadeDuration = 0.3f;
		float mMaxAlpha = 0.8f;

		float mFlashElapsed = 0.0f;
		float mFlashDuration = 0.0f;
		bool mIsFlashing = false;
	} HitFlash, * LPHitFlash;
}

#endif
