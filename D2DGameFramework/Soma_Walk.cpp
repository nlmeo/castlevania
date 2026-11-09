#include "Soma_Walk.h"
#include "Soma.h"
#include "GameTool.h"

void Soma_Walk::OnEnter()
{

	//mAttributes->dy = 0;
	mAttributes->use_g = true;
	mAttributes->cursp = mAttributes->sp;
	
	if (mFSM->GetCurrentState() == mFSM->GetState(L"idle"))
	{
		mAnimator->Play(L"walk_start");
	}
	else if (mFSM->GetCurrentState() == mFSM->GetState(L"land"))
	{
		mAnimator->Play(L"land_while_move");
	}
}


void Soma_Walk::OnUpdate(float delta_time)
{
	if (GT::KeyDown('A'))
	{
		mAttributes->dx = 4;
		mAttributes->cursp = mAttributes->sp;
		mOwner->transform().SetScale({ -1, 1 });
	}
	else if (GT::KeyDown('D'))
	{
		mAttributes->dx = 6;
		mAttributes->cursp = mAttributes->sp;
		mOwner->transform().SetScale({ 1, 1 });
	}

	if (mOwner->IsTurnDir())
	{
		mAnimator->Play(L"walk_turn");
		mAttributes->m_last_scale_x = mOwner->transform().localScale().x;
		mAttributes->m_is_turn = false;
	}

	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{
		if (mAnimator->GetCurrentAniKey() != L"walk_loop")
			mAnimator->Play(L"walk_loop");
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
	else
	{
		if (!GT::KeyDown('A') && !GT::KeyDown('D'))
			mFSM->SetNextState(L"idle");
		else 
			mFSM->SetNextState(L"walk");
	}
}

void Soma_Walk::OnQuit()
{
}
