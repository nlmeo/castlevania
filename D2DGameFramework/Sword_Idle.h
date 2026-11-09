#pragma once
#include "SwordStates.h"

class Sword_Idle : public SwordStates
{
public:
	virtual void OnEnter();
	virtual void OnUpdate(float delta_time = 0);
	virtual void OnQuit();
};
