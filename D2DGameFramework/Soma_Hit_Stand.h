#pragma once
#include "SomaStates.h"

class Soma_Hit_Stand : public SomaStates
{
	bool isCrouch = false;
public:
	virtual void OnEnter();
	virtual void OnUpdate(float delta_time = 0);
	virtual void OnQuit();

};
