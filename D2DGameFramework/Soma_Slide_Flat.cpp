#include "Soma_Slide_Flat.h"
#include "GameTool.h"
#include "EffectsPool.h"
void Soma_Slide_Flat::OnEnter()
{
	mAttributes->cursp = mInitSpeed;
	mAttributes->dx = (mAttributes->m_last_scale_x > 0) ? 6 : 4;
	mAttributes->dy = 5;

	mAnimator->Play(L"slide_on_flat_ground");

	//
	mOriginalAttack = mAttributes->atk;
	mAttributes->atk = 10;
	mOwner->EnableSlideAtkCollider(true);
	
	
	GT::PlaySE(SE_SOMA_SLIDE_FLAT);

	mTimePass = 0.0f;
	mCurEffectCount = 0;

}

void Soma_Slide_Flat::OnUpdate(float delta_time)
{
	mAttributes->cursp *= mDeceleration;

	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{
		mFSM->SetNextState(L"crouch");
	}
	else if (mOwner->CanDash() && GT::KeyJustDown('L'))
		mFSM->SetNextState(L"dash_back");

	

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

	
	if (!mOwner->OnGround())
	{
		mFSM->SetNextState(L"fall");
		return;
	}

}

void Soma_Slide_Flat::OnQuit()
{
	mAttributes->atk = mOriginalAttack;
	mOwner->EnableSlideAtkCollider(false);
}
