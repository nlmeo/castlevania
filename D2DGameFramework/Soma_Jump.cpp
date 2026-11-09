#include "Soma_Jump.h"
#include "Soma.h"
#include "GameTool.h"

void Soma_Jump::OnEnter()
{
	mAttributes->dy = 8;
	mAttributes->curjp = mAttributes->jp * 12.0f;
	//mAttributes->cur_jp_height = 0;
	mAttributes->cur_g = 0;
	mOwner->SetOnGround(false);
	mOwner->transform().translate({0, -16.0f});
	GT::PlaySE(SE_SOMA_JUMP);

	if (mAttributes->dx == 5)
		mAnimator->Play(L"jump_straight");
	else
		mAnimator->Play(L"jump_diagonal");

}

void Soma_Jump::OnUpdate(float delta_time)
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
	if (!GT::KeyDown(32))
		mAttributes->curjp = 0;


	if (mAttributes->dx == 5)
		mAnimator->Continue(L"jump_straight");
	else
		mAnimator->Continue(L"jump_diagonal");

	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
		mAnimator->RePlay();

	if (GT::KeyJustDown(VK_SPACE) && mAttributes->air_jump_remain > 0)
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
	else if (mOwner->OnCollideTop() || mAttributes->curjp <= 0)
		mFSM->SetNextState(L"fall");
	else
		mFSM->SetNextState(L"jump");
}

void Soma_Jump::OnQuit()
{
}
