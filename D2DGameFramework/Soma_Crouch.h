#pragma once
#include "SomaStates.h"

class Soma_Crouch : public SomaStates
{
public:
	virtual void OnEnter();
	virtual void OnUpdate(float delta_time = 0);
	virtual void OnQuit();
};