#include "Zombi_Alert.h"
#include "GameTool.h"

void Zombi_Alert::OnEnter()
{
	mOwner->SetDebugColor({ 1.0f, 0.2f, 0.2f, 0.3f });

	mAttributes->cursp = 0;
	mAlertTimer = 0;
	mAnimator->Play(L"zombi_walk");
	
	//mOwner->FacePlayer();
}

void Zombi_Alert::OnUpdate(float delta_time)
{
	mAlertTimer += delta_time;

	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{
		mAnimator->RePlay();
	}

	if (mAlertTimer >= mAlertDuration)
	{
		mFSM->SetNextState(L"chase");
		return;
	}

	if (!mOwner->PlayerInRange())
	{
		mFSM->SetNextState(L"patrol");
		return;
	}
}

void Zombi_Alert::OnQuit()
{
}
