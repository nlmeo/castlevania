#pragma once
#include "FlyStates.h"

class Fly_Dead : public FlyStates
{
public:
	virtual void OnEnter();
	virtual void OnUpdate(float delta_time = 0);
	virtual void OnQuit();
};
