#pragma once
#include "SomaStates.h"

class Soma_Hit_Air : public SomaStates
{
public:
	virtual void OnEnter();
	virtual void OnUpdate(float delta_time = 0);
	virtual void OnQuit();
private:
	float mKnockbackSpeed = 8.0f;
	float mKnockbackUp = 5.0f;
};
