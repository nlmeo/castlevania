#include "Bat_Idle.h"
#include "GameTool.h"

void Bat_Idle::OnEnter()
{
	mOwner->SetDebugColor({ 0.2f, 0.5f, 1.0f, 0.3f });

	mAttributes->dx = 5;
	mAttributes->dy = 5;
	mAttributes->cursp = 0;
	mAnimator->Play(L"bat_idle");
}

void Bat_Idle::OnUpdate(float delta_time)
{
	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{
		mAnimator->RePlay();
	}

	Soma* player = mOwner->FindPlayerInRange(Bat::GetDetectWidth(), Bat::GetDetectHeight());
	if (player)
	{
		mOwner->SetDebugColor({ 1.0f, 0.2f, 0.2f, 0.3f });
		mFSM->SetNextState(L"walk");
		return;
	}
}

void Bat_Idle::OnQuit()
{
}
