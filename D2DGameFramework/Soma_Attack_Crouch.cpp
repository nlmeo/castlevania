#include "Soma_Attack_Crouch.h"
#include "GameTool.h"
#include "AttackEffect.h"

void Soma_Attack_Crouch::OnEnter()
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
		
	
	mAnimator->Play(L"sword_attack_crouch");

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
}

void Soma_Attack_Crouch::OnUpdate(float delta_time)
{
	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{
		mFSM->SetNextState(L"crouch");
	}

}

void Soma_Attack_Crouch::OnQuit()
{
	mOwner->GetAttackEffect()->Deactivate();
}
