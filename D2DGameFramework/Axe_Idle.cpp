#include "Axe_Idle.h"
#include "GameTool.h"
#include "./HelpTool/RandomMaker.h"

void Axe_Idle::OnEnter()
{
	mOwner->SetDebugColor({ 0.2f, 0.5f, 1.0f, 0.3f });
	mAttributes->dx = 5;
	mAttributes->dy = 5;
	mAttributes->cursp = 0;

	mIdleDuration = (HelpTool::RandomFloat(0, 121) + 120) / 60.0f;
	mIdleTimer = 0;
	mAnimator->Play(L"axe_idle");
}

void Axe_Idle::OnUpdate(float delta_time)
{
	mIdleTimer += delta_time;

	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{
		mAnimator->RePlay();
	}

	if (mOwner->PlayerInRange())
	{
		mOwner->SetDebugColor({ 1.0f, 0.2f, 0.2f, 0.3f });
		mFSM->SetNextState(L"alert");
		return;
	}
	

	if (mIdleTimer >= mIdleDuration)
	{
		mFSM->SetNextState(L"patrol");
		return;
	}
}

void Axe_Idle::OnQuit()
{
}
