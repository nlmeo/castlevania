#include "SomaStates.h"
#include "./GamePlay/GameObject.h"
#include "BackPack.h"
void SomaStates::SetOwner(GamePlay::GameObject* pObj)
{
	mOwner = static_cast<Soma*>(pObj);
	InitCachedComponents();
}

void SomaStates::InitCachedComponents()
{
	if (mOwner)
	{
		mFSM = mOwner->GetComponent<GamePlay::FSM>();
		mAnimator = mOwner->GetComponent<GamePlay::Animator>();
		mTrail = mOwner->GetComponent<GamePlay::AfterImageTrail>();
		mAttributes = &mOwner->GetAttributes();
		mBpk = &BackPack;
	}
		
}
