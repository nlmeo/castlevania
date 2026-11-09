#include "Zombi_Idle.h"
#include "GameTool.h"

void Zombi_Idle::OnEnter()
{
	mOwner->SetDebugColor({ 0.2f, 0.5f, 1.0f, 0.3f });

	mAttributes->dx = 5;
	mAttributes->dy = 5;
	mAttributes->cursp = 0;
	mIdleTimer = 0;
	mAnimator->Play(L"zombi_idle");
}

void Zombi_Idle::OnUpdate(float delta_time)
{
	//mIdleTimer += delta_time;

	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{
		mFSM->SetNextState(L"patrol");
	}

	//if (mOwner->PlayerInRange())
	//{
	//	mOwner->SetDebugColor({ 1.0f, 0.2f, 0.2f, 0.3f });
	//	mFSM->SetNextState(L"alert");
	//}
	//
	//if (mIdleTimer >= mIdleDuration)
	//{
	//	
	//	return;
	//}
}

void Zombi_Idle::OnQuit()
{
}
