#pragma once
#include "Axe.h"

class AxeStates : public GamePlay::StateNode
{
public:
	virtual void OnEnter() = 0;
	virtual void OnUpdate(float delta_time = 0) = 0;
	virtual void OnQuit() = 0;
	virtual void SetOwner(class GamePlay::GameObject* pObj);
protected:
	void InitCachedComponents();
	Axe* mOwner = nullptr;
	Attributes* mAttributes = nullptr;
	GamePlay::FSM* mFSM = nullptr;
	GamePlay::Animator* mAnimator = nullptr;
};
