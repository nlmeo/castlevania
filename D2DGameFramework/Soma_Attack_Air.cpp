#include "Soma_Attack_Air.h"
#include "GameTool.h"
#include "AttackEffect.h"
void Soma_Attack_Air::OnEnter()
{
	mOwner->GetAttackEffect()->Activate(mOwner->transform().localScale().x < 0);
	
	mAnimator->Play(L"sword_attack_air");
	mOwner->GetAttackEffect()->Activate(mOwner->transform().localScale().x < 0);

	if (rand() % 2)
		GT::PlaySE(SE_SOMA_ATK_JAPANIZE_SWORD_1);
	else
		GT::PlaySE(SE_SOMA_ATK_JAPANIZE_SWORD_2);

	switch (rand() % 10)
	{
	case 0:
		GT::PlayVoice(SE_V_SOMA_HA);
		break;
	case 1:
		GT::PlayVoice(SE_V_SOMA_CE);
		break;
	case 2:
		GT::PlayVoice(SE_V_SOMA_TU);
		break;
	default:
		break;
	}
	
	if(rand()%2)
		GT::PlaySE(SE_SOMA_ATK_JAPANIZE_SWORD_1);
	else 
		GT::PlaySE(SE_SOMA_ATK_JAPANIZE_SWORD_2);

	switch (rand()%10)
	{
	case 0:
		GT::PlayVoice(SE_V_SOMA_HA);
		break;
	case 1:
		GT::PlayVoice(SE_V_SOMA_CE);
		break;
	case 2:
		GT::PlayVoice(SE_V_SOMA_TU);
		break;
	default:		   
			break;		  
	}

}

void Soma_Attack_Air::OnUpdate(float delta_time)
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

	mAttributes->dy = (mAttributes->curjp > 0) ? 8 : 2;



	if (mAttributes->curjp > 0 && !GT::KeyDown(32))
	{
		mAttributes->curjp = 0;
		mAttributes->dy = 2;
	}

	
	

	if (mOwner->JustLanded() || mOwner->OnGround())
	{
		mFSM->SetNextState(L"land");
		return;
	}

	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{
		mFSM->SetNextState(L"fall");
	}
	
}

void Soma_Attack_Air::OnQuit()
{
	mOwner->GetAttackEffect()->Deactivate();
}
