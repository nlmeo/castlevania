#pragma once
#include "SomaStates.h"

class Soma_Dead : public SomaStates
{
public:
	bool CanEnter() override { return true; }
	void OnEnter() override;
	void OnUpdate(float deltaTime) override;
	void OnQuit() override {}
};
