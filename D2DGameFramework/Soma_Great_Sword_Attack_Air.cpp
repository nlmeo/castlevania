#include "Soma_Great_Sword_Attack_Air.h"
#include "GameTool.h"
#include "AttackEffect.h"
void Soma_Great_Sword_Attack_Air::OnEnter()
{

	mAnimator->Play(L"great_sword_attack_air");
	mOwner->GetAttackEffect()->Activate(mOwner->transform().localScale().x < 0);


	GT::PlaySE(SE_SOMA_ATK_GREAT_SWORD);


	switch (rand() % 10)
	{
	case 0:
		GT::PlayVoice(SE_SOMA_HENG);
		break;
	case 1:
		GT::PlayVoice(SE_V_SOMA_YIYA);
		break;
	case 2:
		GT::PlayVoice(SE_V_SOMA_CIYA);
		break;
	default:
		break;
	}
	mAttributes->is_hit_state_change = false;
}

void Soma_Great_Sword_Attack_Air::OnUpdate(float delta_time)
{
	if (!mOwner->OnGround())
	{
		if (GT::KeyDown('A'))
		{
			mAttributes->dx = 4;
			mAttributes->cursp = mAttributes->sp;
		}
		else if (GT::KeyDown('D'))
		{
			mAttributes->dx = 6;
			mAttributes->cursp = mAttributes->sp;
		}
		else
		{
			mAttributes->dx = 5;
			mAttributes->cursp = 0;
		}
	}
	

	mAttributes->dy = (mAttributes->curjp > 0) ? 8 : 2;



	if (mAttributes->curjp > 0 && !GT::KeyDown(32))
	{
		mAttributes->curjp = 0;
		mAttributes->dy = 2;
	}

	if (mOwner->JustLanded())
	{
		mAnimator->Continue(L"great_sword_attack_stand");
		mAttributes->dx = 5;
		mAttributes->dy = 5;
		mAttributes->cursp = 0;
	}

	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{
		if (mOwner->OnGround())
		{
			mFSM->SetNextState(L"idle");
		}
		else
		{
			mFSM->SetNextState(L"fall");
		}
		
	}

	if (mOwner->OnGround())
	{
		if (GT::KeyJustDown('U'))
			mFSM->SetNextState(L"great_sword_supper_attack");
		else if (mOwner->CanDash() && GT::KeyJustDown('L'))
			mFSM->SetNextState(L"dash_back");
	}
	
	
}

void Soma_Great_Sword_Attack_Air::OnQuit()
{
	mAttributes->is_hit_state_change = true;
	mAttributes->is_hit = false;
	mOwner->GetAttackEffect()->Deactivate();
}
