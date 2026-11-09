#include "Soma_Punch_Air.h"
#include "GameTool.h"

void Soma_Punch_Air::OnEnter()
{
	mAnimator->Play(L"punch_attack_air");

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

void Soma_Punch_Air::OnUpdate(float delta_time)
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
		mAttributes->dx = 5;
		mAttributes->dy = 5;
		mAttributes->cursp = 0;
		return;
	}

	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{
		mFSM->SetNextState(L"fall");
	}
}

void Soma_Punch_Air::OnQuit()
{
	mOwner->EnableSlideAtkCollider(false);
}