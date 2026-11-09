#include "ZombiStates.h"
#include "./GamePlay/GameObject.h"
#include "Soma.h"

void ZombiStates::SetOwner(GamePlay::GameObject* pObj)
{
	mOwner = static_cast<Zombi*>(pObj);
	InitCachedComponents();
}

void ZombiStates::InitCachedComponents()
{
	if (mOwner)
	{
		mFSM = mOwner->GetComponent<GamePlay::FSM>();
		mAnimator = mOwner->GetComponent<GamePlay::Animator>();
		mAttributes = &mOwner->GetAttributes();
	}
}
