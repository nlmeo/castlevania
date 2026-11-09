#pragma once
#include "Fly.h"

class Soma;

class FlyStates : public GamePlay::StateNode
{
public:
	virtual void OnEnter() = 0;
	virtual void OnUpdate(float delta_time = 0) = 0;
	virtual void OnQuit() = 0;
	virtual void SetOwner(class GamePlay::GameObject* pObj);
protected:
	void InitCachedComponents();
	float PlayerDistanceX(Soma* player) const;
	void FacePlayer(Soma* player);
	Fly* mOwner = nullptr;
	Attributes* mAttributes = nullptr;
	GamePlay::FSM* mFSM = nullptr;
	GamePlay::Animator* mAnimator = nullptr;
};
