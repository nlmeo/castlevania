#include "Soma_Punch_Crouch.h"
#include "GameTool.h"

void Soma_Punch_Crouch::OnEnter()
{
	mAnimator->Play(L"punch_attack_crouch");

	mOwner->EnableSlideAtkCollider(true);

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

void Soma_Punch_Crouch::OnUpdate(float delta_time)
{
	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{
		mFSM->SetNextState(L"crouch");
	}
}

void Soma_Punch_Crouch::OnQuit()
{
	mOwner->EnableSlideAtkCollider(false);
}