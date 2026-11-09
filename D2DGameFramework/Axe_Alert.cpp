#include "Axe_Alert.h"
#include "GameTool.h"
#include "Soma.h"
void Axe_Alert::OnEnter()
{
	mOwner->SetDebugColor({ 1.0f, 0.2f, 0.2f, 0.3f });

	mAttributes->dx = 5;
	mAttributes->dy = 5;
	mAttributes->cursp = 0;
	mAlertCD = 300;

	GamePlay::FSM* fsm = mOwner->GetComponent<GamePlay::FSM>();
	GamePlay::LPSN curState = fsm->GetCurrentState();
	if (curState == fsm->GetState(L"patrol") || curState == fsm->GetState(L"idle"))
		mFirstLockTime = 60;
	else
		mFirstLockTime = 30;

	mAnimator->Play(L"axe_idle");
	mOwner->FacePlayer();

	
}

void Axe_Alert::OnUpdate(float delta_time)
{
	mAttributes->cur_atk_cd -= 1;
	if (mAttributes->cur_atk_cd < 0)
		mAttributes->cur_atk_cd = 0;

	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{
		mAnimator->RePlay();
	}

	if (mOwner->PlayerInRange())
	{
		if (mFirstLockTime > 0)
		{
			mFirstLockTime -= 1;
		}
		else
		{
			mFSM->SetNextState(L"chase");	
		}
	}
	else 
	{
		mAlertCD -= 1;
		if (mAlertCD <= 0)
			mFSM->SetNextState(L"patrol");
	}


}

void Axe_Alert::OnQuit()
{
}
