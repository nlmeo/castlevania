#include "Soma_Crouch.h"
#include "GameTool.h"
void Soma_Crouch::OnEnter()
{
	mAttributes->dx = 5;
	mAttributes->dy = 5;
	mAttributes->use_g = true;
	mAttributes->cursp = 0;
	mAttributes->curjp = 0;
	// 只有完成重落地、末尾已是蹲姿时，才省略下蹲起始动画。
	const auto& landingAnimation = mAnimator->GetCurrentAniKey();
	const bool finishedCrouchedLanding =
		mFSM->GetCurrentState() == mFSM->GetState(L"land") &&
		mAnimator->GetState() == GamePlay::AS::AS_STOPPED &&
		(landingAnimation == L"land_large_height" || landingAnimation == L"land_diagonal");

	if (
		mFSM->GetCurrentState() == mFSM->GetState(L"slide_flat") ||
		mFSM->GetCurrentState() == mFSM->GetState(L"dash_back") ||
		mFSM->GetCurrentState() == mFSM->GetState(L"hit_stand")  || 
		finishedCrouchedLanding
		)
	{
		mAnimator->Play(L"crouch_loop");
	}
	else if (mFSM->GetCurrentState() == mFSM->GetState(L"attack_crouch"))
	{
		if (mAnimator->GetCurrentAniKey() == L"punch_attack_crouch")
			mAnimator->Play(L"punch_attack_crouch_end");
		else
			mAnimator->Play(L"sword_attack_crouch_end");
	}
	else if (mFSM->GetCurrentState() == mFSM->GetState(L"punch_attack_crouch"))
	{
		mAnimator->Play(L"punch_attack_crouch_end");
	}
	else
	{
		mAnimator->Play(L"crouch_start");
	}
	
}

void Soma_Crouch::OnUpdate(float delta_time)
{
	if (GT::KeyDown('A'))
	{
		mAttributes->dx = 4;
		mOwner->transform().SetScale({ -1, 1 });
	}
	else if (GT::KeyDown('D'))
	{
		mAttributes->dx = 6;
		mOwner->transform().SetScale({ 1, 1 });
	}


	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{
		if (mAnimator->GetCurrentAniKey() != L"crouch_loop")
			mAnimator->Play(L"crouch_loop");
		else
			mAnimator->RePlay();
	}
	if (GT::KeyJustDown('J'))
	{
		auto* curWeapon = mBpk->GetEquippedWeapon();
		if (curWeapon == nullptr)
			mFSM->SetNextState(L"punch_attack_crouch");
		else if (curWeapon->id == 4)
			mFSM->SetNextState(L"great_sword_attack_crouch");
		else
			mFSM->SetNextState(L"attack_crouch");
	}
	else if (GT::KeyDown('S') && GT::KeyJustDown(VK_SPACE))
	{
		if (mOwner->OnPlatformGround() && mOwner->canJumpBelow())
		{
			mOwner->DropFromPlatform();
			mFSM->SetNextState(L"fall");
		}
		else
		{
			mFSM->SetNextState(L"slide_flat");
		}

	}
	else if (GT::KeyDown('S'))
	{
		mFSM->SetNextState(L"crouch"); 	
	}
	else
		mFSM->SetNextState(L"idle");

}

void Soma_Crouch::OnQuit()
{
}
