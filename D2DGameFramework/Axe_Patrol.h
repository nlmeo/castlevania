#pragma once
#include "AxeStates.h"

class Axe_Patrol : public AxeStates
{
public:
	virtual void OnEnter();
	virtual void OnUpdate(float delta_time = 0);
	virtual void OnQuit();
};
