#pragma once
#include "AxeStates.h"

class Axe_Idle : public AxeStates
{
public:
	virtual void OnEnter();
	virtual void OnUpdate(float delta_time = 0);
	virtual void OnQuit();
private:
	float mIdleTimer = 0;
	float mIdleDuration = 0;
};
