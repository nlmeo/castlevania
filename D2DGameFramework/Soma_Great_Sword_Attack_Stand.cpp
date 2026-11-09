#include "Soma_Great_Sword_Attack_Stand.h"
#include "GameTool.h"
#include "AttackEffect.h"
void Soma_Great_Sword_Attack_Stand::OnEnter()
{
	mAttributes->dx = 5;
	mAttributes->dy = 5;
	mAttributes->cursp = 0;

	mAnimator->Play(L"great_sword_attack_stand");
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

void Soma_Great_Sword_Attack_Stand::OnUpdate(float delta_time)
{
	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{
		mFSM->SetNextState(L"idle");
	}

	if (GT::KeyJustDown('U'))
		mFSM->SetNextState(L"great_sword_supper_attack");
	else if (mOwner->CanDash() && GT::KeyJustDown('L'))
		mFSM->SetNextState(L"dash_back");
}

void Soma_Great_Sword_Attack_Stand::OnQuit()
{
	mAttributes->is_hit_state_change = true;
	mAttributes->is_hit = false;
	mOwner->GetAttackEffect()->Deactivate();
}
