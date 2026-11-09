#pragma once
#include "AxeStates.h"

class Axe_Alert : public AxeStates
{
public:
	virtual void OnEnter();
	virtual void OnUpdate(float delta_time = 0);
	virtual void OnQuit();
private:
	int mAlertCD = 300;
	int mFirstLockTime = 0;
};

