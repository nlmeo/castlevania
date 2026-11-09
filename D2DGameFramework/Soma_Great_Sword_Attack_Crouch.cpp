#include "Soma_Great_Sword_Attack_Crouch.h"
#include "GameTool.h"
#include "AttackEffect.h"
void Soma_Great_Sword_Attack_Crouch::OnEnter()
{
	mAttributes->dy = 5;

	if (mAttributes->dx == 4)
	{
		mAttributes->dx = 6;
	}
	else
	{
		mAttributes->dx = 4;
	}


	mAnimator->Play(L"great_sword_attack_crouch");

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

void Soma_Great_Sword_Attack_Crouch::OnUpdate(float delta_time)
{
	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{
		mFSM->SetNextState(L"crouch");
	}

}

void Soma_Great_Sword_Attack_Crouch::OnQuit()
{
	mAttributes->is_hit_state_change = true;
	mAttributes->is_hit = false;
	mOwner->GetAttackEffect()->Deactivate();
}
