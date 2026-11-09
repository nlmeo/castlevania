#pragma once
#include "ZombiStates.h"

class Zombi_Patrol : public ZombiStates
{
public:
	virtual void OnEnter();
	virtual void OnUpdate(float delta_time = 0);
	virtual void OnQuit();
private:
	float mPatrolTimer = 0;      
	float mPatrolDuration = 3.0f;
	float mDetectRange = 300.0f; 
};
