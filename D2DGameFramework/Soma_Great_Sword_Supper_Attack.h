#pragma once
#include "SomaStates.h"

class Soma_Great_Sword_Supper_Attack : public SomaStates
{
public:
	bool CanEnter() override { return mOwner->TryConsumeSuperAttack(); }
	virtual void OnEnter();
	virtual void OnUpdate(float delta_time = 0);
	virtual void OnQuit();
};
