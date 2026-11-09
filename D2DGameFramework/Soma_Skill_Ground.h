#pragma once
#include "SomaStates.h"

class Soma_Skill_Ground : public SomaStates
{
public:
	bool CanEnter() override { return mOwner->TryConsumeRedSoul(); }
	virtual void OnEnter();
	virtual void OnUpdate(float delta_time = 0);
	virtual void OnQuit();
};
