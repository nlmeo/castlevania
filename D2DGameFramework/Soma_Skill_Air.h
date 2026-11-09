#pragma once
#include "SomaStates.h"

class Soma_Skill_Air : public SomaStates
{
	bool m_bulletFired = false;
public:
	bool CanEnter() override { return mOwner->TryConsumeRedSoul(); }
	virtual void OnEnter();
	virtual void OnUpdate(float delta_time = 0);
	virtual void OnQuit();
};
