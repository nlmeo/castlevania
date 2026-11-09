#pragma once
#include "BatStates.h"

class Bat_Dead : public BatStates
{
public:
	virtual void OnEnter();
	virtual void OnUpdate(float delta_time = 0);
	virtual void OnQuit();
};
