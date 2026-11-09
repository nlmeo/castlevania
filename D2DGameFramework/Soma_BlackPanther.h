#pragma once
#include "SoulState.h"

class Soma_BlackPanther : public SoulState
{
	int mOriginalAttack = 5;
public:
	virtual void OnEnter() override;
	virtual void OnUpdate(float delta_time) override;
	virtual void OnQuit() override;
};
