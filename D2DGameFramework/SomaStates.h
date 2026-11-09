#pragma once

#include "Soma.h"
#include "BackPack.h"

class SomaStates : public GamePlay::StateNode
{
public:
	bool CanEnter() override { return mOwner && !mOwner->IsDead(); }
	virtual void OnEnter() = 0;
	virtual void OnUpdate(float delta_time = 0) = 0;
	virtual void OnQuit() = 0;
	virtual void SetOwner(class GamePlay::GameObject* pObj);
protected:
	void InitCachedComponents();
	Soma* mOwner = nullptr;
	Attributes* mAttributes = nullptr;
	GamePlay::FSM* mFSM = nullptr;
	GamePlay::Animator* mAnimator = nullptr;
	GamePlay::AfterImageTrail* mTrail = nullptr;
	CBackPackManager* mBpk = nullptr;
};
