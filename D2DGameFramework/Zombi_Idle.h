#pragma once
#include "ZombiStates.h"


class Zombi_Idle : public ZombiStates
{
public:
	virtual void OnEnter();
	virtual void OnUpdate(float delta_time = 0);
	virtual void OnQuit();
private:
	float mIdleTimer = 0;       
	float mIdleDuration = 2.0f;
};
