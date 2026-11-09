#pragma once
#include "ZombiStates.h"

class Zombi_Chase : public ZombiStates
{
public:
	virtual void OnEnter();
	virtual void OnUpdate(float delta_time = 0);
	virtual void OnQuit();
private:
	float mDetectRange = 300.0f;     
	float mPassThreshold = 80.0f;    
	float mTurnCooldown = 0.0f;      
	float mTurnDelay = 0.5f;         
};
