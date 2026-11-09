#include "Soma_Hit_Stand.h"
#include "Soma.h"
#include "GameTool.h"

void Soma_Hit_Stand::OnEnter()
{
	mOwner->SetDebugColor({ 1.0f, 0.2f, 0.2f, 0.3f });

	mAttributes->cursp = 0;
	mAttributes->curjp = 0;

	if (mFSM->GetCurrentState() == mFSM->GetState(L"crouch") ||
		mFSM->GetCurrentState() == mFSM->GetState(L"slide_flat")
		)
	{
		mAnimator->Play(L"hit_while_crouch");
		isCrouch = true;
	}
	else// if (mFSM->GetCurrentState() == mFSM->GetState(L"idle") || mFSM->GetCurrentState() == mFSM->GetState(L"walk"))
	{
		if(mAttributes->is_hit_back)
			mAnimator->Play(L"hit_from_behind");
		else
			mAnimator->Play(L"hit_in_the_groud_front");
	}
		 

	
}

void Soma_Hit_Stand::OnUpdate(float delta_time)
{
	
	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{
		if (isCrouch)
			mFSM->SetNextState(L"crouch");
		else
		mFSM->SetNextState(L"idle");
		
	}
}

void Soma_Hit_Stand::OnQuit()
{
	if (isCrouch) { isCrouch = false;}
	mAttributes->is_hit = false;
	mOwner->SetDebugColor({ 0.2f, 0.5f, 1.0f, 0.3f });
}
