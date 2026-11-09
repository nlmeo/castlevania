#include "Soma_Fall.h"
#include "Soma.h"
#include "GameTool.h"

void Soma_Fall::OnEnter()
{
	mAttributes->dy = 2;
	mAnimator->Play(L"fall");
	//mAttributes->fall_height = 0;

}

void Soma_Fall::OnUpdate(float delta_time)
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
	else
	{
		mAttributes->dx = 5;
		mAttributes->cursp = 0;
	}

	if (mAttributes->dx != 5)
		mAttributes->cursp = mAttributes->sp;

	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
		mAnimator->RePlay();


	if (mOwner->JustLanded() || mOwner->OnGround())
	{
		mFSM->SetNextState(L"land");
		return;
	}

	// 二段跳：空中再次按下跳跃键且还有剩余次数
	if (GT::KeyJustDown(32) && mAttributes->air_jump_remain > 0)
	{
		mFSM->SetNextState(L"double_jump");
		return;
	}

	if (GT::KeyJustDown('J'))
	{
		auto* curWeapon = mBpk->GetEquippedWeapon();
		if (curWeapon == nullptr)
			mFSM->SetNextState(L"punch_attack_air");
		else if (curWeapon->id == 4)
			mFSM->SetNextState(L"great_sword_attack_air");
		else
			mFSM->SetNextState(L"attack_air");
	}
	//else if (GT::KeyJustDown('U'))
	//	mFSM->SetNextState(L"using_skill_in_air");

}

void Soma_Fall::OnQuit()
{
}
