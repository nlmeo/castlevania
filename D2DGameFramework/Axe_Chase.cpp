#include "Axe_Chase.h"
#include "GameTool.h"

void Axe_Chase::OnEnter()
{
	mOwner->SetDebugColor({ 1.0f, 0.2f, 0.2f, 0.3f });

	mAttributes->cursp = mAttributes->sp;
	mAnimator->Play(L"axe_walk");


	mOwner->FacePlayer();
	float distX = mOwner->PlayerDistanceX();
	float absDistX = distX > 0 ? distX : -distX;
	if (absDistX < 192.0f)
	{
		mAttributes->dx = distX > 0 ? 4 : 6;
	}
	else
	{
		mAttributes->dx = distX > 0 ? 6 : 4;
	}

}

// 水平距离小于2格，面朝玩家后退，
// 玩家距离大于8格小于12格，面朝玩家追击
void Axe_Chase::OnUpdate(float delta_time)
{
	mOwner->FacePlayer();
	float distX = mOwner->PlayerDistanceX();
	float absDistX = distX > 0 ? distX : -distX;
	
	
	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{
		if (mOwner->PlayerInRange())
		{
			mFSM->SetNextState(L"attack");
		}
		else
		{
			mFSM->SetNextState(L"alert");
		}
	}

}

void Axe_Chase::OnQuit()
{
}
