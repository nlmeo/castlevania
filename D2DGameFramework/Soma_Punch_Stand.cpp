#include "Soma_Punch_Stand.h"
#include "GameTool.h"

void Soma_Punch_Stand::OnEnter()
{
	mAttributes->dx = 5;
	mAttributes->dy = 5;
	mAttributes->cursp = 0;

	mOwner->EnableSlideAtkCollider(true);

	mAnimator->Play(L"punch_attack_stand");

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

void Soma_Punch_Stand::OnUpdate(float delta_time)
{
	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{
		mFSM->SetNextState(L"idle");
	}

	if (mOwner->CanDash() && GT::KeyJustDown('L'))
		mFSM->SetNextState(L"dash_back");
}

void Soma_Punch_Stand::OnQuit()
{
	mOwner->EnableSlideAtkCollider(false);
}