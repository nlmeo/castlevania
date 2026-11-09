#include "Soma_Idle.h"
#include "Soma.h"
#include "GameTool.h"

void Soma_Idle::OnEnter()
{
	mAttributes->dx = 5;
	mAttributes->dy = 5;
	mAttributes->use_g = true;
	mAttributes->cursp = 0;
	mAttributes->curjp = 0;
	if (mFSM->GetCurrentState() == mFSM->GetState(L"walk") || 
		mFSM->GetCurrentState() == mFSM->GetState(L"black_panther"))
	{
		mAnimator->Play(L"walk_stop");
	}
	else if (mFSM->GetCurrentState() == mFSM->GetState(L"crouch") ||
		(mFSM->GetCurrentState() == mFSM->GetState(L"land") &&
		 mAnimator->GetCurrentAniKey() == L"land_large_height"))
	{
		mAnimator->Play(L"crouch_stop");
	}
	else if (mFSM->GetCurrentState() == mFSM->GetState(L"attack_stand"))
	{
		if (mAnimator->GetCurrentAniKey() == L"sword_attack_stand")
			mAnimator->Play(L"sword_attack_stand_end");
	}
	else if (mFSM->GetCurrentState() == mFSM->GetState(L"punch_attack_stand"))
	{
		if (mAnimator->GetCurrentAniKey() == L"punch_attack_stand")
			mAnimator->Play(L"punch_attack_stand_end");
	}
	else if (mAnimator->GetCurrentAniKey().empty())
	{
		mAnimator->Play(L"idle");
	}

}

void Soma_Idle::OnUpdate(float delta_time)
{
	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{
		if (mAnimator->GetCurrentAniKey() != L"idle")
			mAnimator->Play(L"idle");
		else
			mAnimator->RePlay();
	}


	if (!mOwner->OnGround())
	{
		mFSM->SetNextState(L"fall");
		return;
	}

	if (GT::KeyJustDown('J'))
	{
		auto* curWeapon = mBpk->GetEquippedWeapon();
		if (curWeapon == nullptr)
			mFSM->SetNextState(L"punch_attack_stand");
		else if (curWeapon->id == 4)
			mFSM->SetNextState(L"great_sword_attack_stand");
		else 
			mFSM->SetNextState(L"attack_stand");
	}
	//else if (GT::KeyJustDown('U'))
	//	mFSM->SetNextState(L"using_skill_on_ground");
	else if (GT::KeyJustDown(32) && GT::KeyDown('S') && mOwner->OnPlatformGround())
	{
		mOwner->DropFromPlatform();
		mFSM->SetNextState(L"fall");
	}
	else if (GT::KeyJustDown(32))
		mFSM->SetNextState(L"jump");
	else if (mOwner->CanDash() && GT::KeyJustDown('L'))
		mFSM->SetNextState(L"dash_back");
	else if (GT::KeyDown('S'))
		mFSM->SetNextState(L"crouch");
	else if (GT::KeyDown('A') || GT::KeyDown('D'))
		mFSM->SetNextState(L"walk");
	//else 
	//	mFSM->SetNextState(L"idle");
}

void Soma_Idle::OnQuit()
{
}
