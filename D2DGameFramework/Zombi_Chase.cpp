#include "Zombi_Chase.h"
#include "GameTool.h"

void Zombi_Chase::OnEnter()
{
	mOwner->SetDebugColor({ 1.0f, 0.2f, 0.2f, 0.3f });
	mAttributes->cursp = mAttributes->sp;
	mTurnCooldown = 0;

	mAnimator->Play(L"zombi_walk");
}

void Zombi_Chase::OnUpdate(float delta_time)
{
	if (!mOwner->PlayerInRange())
	{
		mFSM->SetNextState(L"patrol");
		return;
	}

	float dist = mOwner->PlayerDistanceX();
	bool facingRight = mOwner->transform().localScale().x < 0;

	bool passedPlayer = (facingRight && dist < -mPassThreshold) || (!facingRight && dist > mPassThreshold);

	if (passedPlayer)
	{
		mTurnCooldown += mTurnDelay;
		if (mTurnCooldown >= mTurnDelay)
		{
			mOwner->FacePlayer();
			mTurnCooldown = 0;
		}
	}
	else
	{
		mTurnCooldown = 0;
	}

	// ???????????
	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{
		mAnimator->RePlay();
	}
}

void Zombi_Chase::OnQuit()
{
}
