#include "Zombi_Patrol.h"
#include "GameTool.h"

void Zombi_Patrol::OnEnter()
{
	mOwner->SetDebugColor({ 0.2f, 0.5f, 1.0f, 0.3f });

	if (mOwner->transform().localScale().x < 0)
		mAttributes->dx = 6;
	else
		mAttributes->dx = 4;

	mAttributes->cursp = mAttributes->sp;
	mPatrolTimer = 0;
	mAnimator->Play(L"zombi_walk");
}

void Zombi_Patrol::OnUpdate(float delta_time)
{
	mPatrolTimer += delta_time;

	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{
		mAnimator->RePlay();
	}

	if (mPatrolTimer >= mPatrolDuration)
	{
		mOwner->transform().SetScale({
			-mOwner->transform().localScale().x,
			1.0f
		});
		mAttributes->dx = (mAttributes->dx == 6) ? 4 : 6;
		mPatrolTimer = 0;
	}

	if (mOwner->PlayerInRange())
	{
		mOwner->SetDebugColor({ 1.0f, 0.2f, 0.2f, 0.3f });
		mFSM->SetNextState(L"alert");
		return;
	}
}

void Zombi_Patrol::OnQuit()
{
}
