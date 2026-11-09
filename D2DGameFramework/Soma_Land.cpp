#include "Soma_Land.h"
#include "GameTool.h"
#include "EffectsPool.h"
void Soma_Land::OnEnter()
{
	// 直接从数据源判断是否为高下落，避免字符串比较不可靠的问题
	//bool canChange = false;
	if (mFSM->GetCurrentState() == mFSM->GetState(L"hit_air"))
	{
		mAnimator->Play(L"land_large_height");
		canChange = false;
		mAttributes->cursp = mAttributes->sp * 0.3f;
	}
	else if (mFSM->GetCurrentState() == mFSM->GetState(L"strike_air"))
	{
		float fallHeight = mAttributes->landing_fall_height;
		if (fallHeight > 250)
		{
			canChange = false;
			if (mAttributes->cursp != 0)
			{
				mAnimator->Play(L"land_diagonal");
				mAttributes->cursp = mAttributes->sp * 0.3;
			}
			else
			{
				mAnimator->Play(L"land_large_height");
			}
		}
		else
		{
			canChange = true;
			if (mAttributes->cursp != 0)
				mAnimator->Play(L"walk_stop");
			else
			{
				if (fallHeight < 100)
					mAnimator->Play(L"land_small_height");
				else//(fallHeight <= 300)
					mAnimator->Play(L"land_medium_height");
			}
		}
			
	}
	else
	{
		float fallHeight = mAttributes->landing_fall_height;
		if (fallHeight > 300)
		{
			mAnimator->Play(L"land_large_height");
			canChange = false;
			mAttributes->cursp = mAttributes->cursp * 0.3f;
		}
		else
		{
			canChange = true;
			
			if (fallHeight < 100)
				mAnimator->Play(L"land_small_height");
			else//(fallHeight <= 300)
				mAnimator->Play(L"land_medium_height");
			
		}
		
	}

	GT::PlaySE(SE_SOMA_LAND);

	mOwner->InitIgnoredCollider();


	mTimePass = 0.0f;
	mCurEffectCount = 0;


	mAttributes->dy = 5;
	mAttributes->curjp = 0;
}

void Soma_Land::OnUpdate(float delta_time)
{
	if (!mOwner->OnGround()) { mFSM->SetNextState(L"fall"); return; }
	if (mOwner->OnGround())
	{

		if (mCurEffectCount < mMaxEffectCount)
		{

			if (mTimePass > mTimeGap)
			{
				MathType::vec2f playerPos = mOwner->transform().position();
				playerPos.y += 50;
				switch (HelpTool::RandomInt(0, 1))
				{
				case 0:
					Effect::Spawn(EFFECT_DUST_01, playerPos, false, 0.4f);
					break;
				case 1:
					Effect::Spawn(EFFECT_DUST_02, playerPos, false, 0.4f);
					break;
				default:
					break;
				}
				mTimePass = 0;
				mCurEffectCount += 1;
			}
			mTimePass += delta_time;

		}
	}
	
	if (mAttributes->cursp > 0)
	{
		float targetSpd = mAttributes->cursp;
		targetSpd -= delta_time * 3;
		targetSpd = targetSpd < 0 ? 0 : targetSpd;
		mAttributes->cursp = targetSpd;
	}
	



	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{
		if (GT::KeyDown('S'))
		{
			mFSM->SetNextState(L"crouch");
		}
		else
		{
			mFSM->SetNextState(L"idle");
		}
		
		return;
	}

	if (canChange && mOwner->OnGround())
	{
		if (GT::KeyDown('S'))
		{
			mFSM->SetNextState(L"crouch");
		}
		else
		{
			if(GT::KeyDown('A') || GT::KeyDown('D'))
				mFSM->SetNextState(L"walk");
			else
				mFSM->SetNextState(L"idle");
		}
	}
	

}

void Soma_Land::OnQuit()
{
	
}
