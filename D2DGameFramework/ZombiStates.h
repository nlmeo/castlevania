#pragma once
#include "Zombi.h"

class Soma;

/**
 * @brief ??? FSM ??????
 *
 * ?? SetOwner ????? static_cast ??? Zombi ???
 * ?????? mOwner / mAttributes / mFSM / mAnimator ???????????
 * ????????????????????
 */
class ZombiStates : public GamePlay::StateNode
{
public:
	virtual void OnEnter() = 0;
	virtual void OnUpdate(float delta_time = 0) = 0;
	virtual void OnQuit() = 0;
	virtual void SetOwner(class GamePlay::GameObject* pObj);
protected:
	void InitCachedComponents();
	Zombi* mOwner = nullptr;
	Attributes* mAttributes = nullptr;
	GamePlay::FSM* mFSM = nullptr;
	GamePlay::Animator* mAnimator = nullptr;
};
