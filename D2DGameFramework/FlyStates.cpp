#include "FlyStates.h"
#include "Soma.h"

void FlyStates::SetOwner(GamePlay::GameObject* pObj)
{
	mOwner = static_cast<Fly*>(pObj);
	InitCachedComponents();
}

void FlyStates::InitCachedComponents()
{
	if (mOwner)
	{
		mFSM = mOwner->GetComponent<GamePlay::FSM>();
		mAnimator = mOwner->GetComponent<GamePlay::Animator>();
		mAttributes = &mOwner->GetAttributes();
	}
}

float FlyStates::PlayerDistanceX(Soma* player) const
{
	if (!player || !mOwner) return 99999.0f;
	return player->transform().position().x - mOwner->transform().position().x;
}

void FlyStates::FacePlayer(Soma* player)
{
	if (!player || !mOwner) return;
	float dist = PlayerDistanceX(player);
	if (dist > 0)
	{
		mOwner->transform().SetScale({ -1.0f, 1.0f });
		mAttributes->dx = 6;
	}
	else
	{
		mOwner->transform().SetScale({ 1.0f, 1.0f });
		mAttributes->dx = 4;
	}
}
