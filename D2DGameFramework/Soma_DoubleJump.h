#pragma once
#include "SomaStates.h"

class Soma_DoubleJump : public SomaStates
{
public:
	bool CanEnter() override { return !mOwner->IsDead() && !mOwner->OnGround() && mAttributes->air_jump_remain > 0; }
	virtual void OnEnter();
	virtual void OnUpdate(float delta_time = 0);
	virtual void OnQuit();
};
