#include "Axe_Patrol.h"
#include "GameTool.h"
#include "./HelpTool/RandomMaker.h"

void Axe_Patrol::OnEnter()
{
	mOwner->SetDebugColor({ 0.2f, 0.5f, 1.0f, 0.3f });

	if (mOwner->transform().localScale().x < 0)
		mAttributes->dx = 6;
	else
		mAttributes->dx = 4;
	mAttributes->cursp = mAttributes->sp;
	mAttributes->cur_atk_cd = mAttributes->atk_cd;

	mAnimator->Play(L"axe_walk");
}


void Axe_Patrol::OnUpdate(float delta_time)
{
	if (mOwner->PlayerInRange())
	{
		mOwner->SetDebugColor({ 1.0f, 0.2f, 0.2f, 0.3f });
		mFSM->SetNextState(L"alert");
	}
	else
	{
		if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
		{
			if ((rand() % 4) == 0)
			{
				mFSM->SetNextState(L"idle");
			}
			else
			{
				mAnimator->RePlay();
			}
		}
	}
	
}

void Axe_Patrol::OnQuit()
{
}
