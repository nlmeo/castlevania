#include "Soma_Attack_Stand.h"
#include "GameTool.h"
#include "AttackEffect.h"
void Soma_Attack_Stand::OnEnter()
{
	mAttributes->dx = 5;
	mAttributes->dy = 5;
	mAttributes->cursp = 0;

	mAnimator->Play(L"sword_attack_stand");
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

void Soma_Attack_Stand::OnUpdate(float delta_time)
{
	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{
		mFSM->SetNextState(L"idle");
	}

	if (GT::KeyJustDown('U'))
		mFSM->SetNextState(L"sword_supper_attack");
	else if (mOwner->CanDash() && GT::KeyJustDown('L'))
		mFSM->SetNextState(L"dash_back");

}

void Soma_Attack_Stand::OnQuit()
{
	mOwner->GetAttackEffect()->Deactivate();
}
