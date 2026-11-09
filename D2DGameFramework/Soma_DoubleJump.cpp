#include "Soma_DoubleJump.h"
#include "Soma.h"
#include "Soma_Strike_Air.h"
#include "GameTool.h"

void Soma_DoubleJump::OnEnter()
{
	// 消耗一次空中跳跃次数
	mAttributes->air_jump_remain--;
	
	mAttributes->dy = 8;
	mAttributes->curjp = mAttributes->jp * 10.0f;
	mAttributes->cur_jp_height = 0;
	mAttributes->cur_g = 0;
	mOwner->SetOnGround(false);
	mOwner->transform().translate({ 0, -16.0f });
	GT::PlaySE(SE_SOMA_JUMP);


	mAnimator->Play(L"double_jump");
	
}

void Soma_DoubleJump::OnUpdate(float delta_time)
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

	
	if (mOwner->OnCollideTop())
		mAttributes->curjp = 0;

	
	if (mAttributes->curjp > 0 && !GT::KeyDown(32))
	{
		mAttributes->curjp = 0;
	}


	if (mOwner->JustLanded() || mOwner->OnGround())
	{
		mFSM->SetNextState(L"land");
		return;
	}

	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{
		mFSM->SetNextState(L"fall");
		return;
	}
		

	// 飞踢：二段跳中按 S+SPACE（竖直下踢）或 S+D+SPACE（斜向下踢）
	
	if (GT::KeyJustDown(32) && (GT::KeyDown('D') || GT::KeyDown('A')))
	{
		auto* strike = dynamic_cast<Soma_Strike_Air*>(mFSM->GetState(L"strike_air"));
		strike->SetDiagonal(true);
		mFSM->SetNextState(L"strike_air");
		return;
	}
	else if (GT::KeyJustDown(32) && GT::KeyDown('S'))
	{
		//bool diagonal = GT::KeyDown('D');
		auto* strike = dynamic_cast<Soma_Strike_Air*>(mFSM->GetState(L"strike_air"));
		strike->SetDiagonal(false);
		mFSM->SetNextState(L"strike_air");
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
	
}

void Soma_DoubleJump::OnQuit()
{
}
