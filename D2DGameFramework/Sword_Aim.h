#pragma once
#include "SwordStates.h"

class Sword_Aim : public SwordStates
{
public:
	virtual void OnEnter();
	virtual void OnUpdate(float delta_time = 0);
	virtual void OnQuit();
private:

	static constexpr float MAX_AIM_TIME = 2.0f; // 最大瞄准时间（秒）
	float rotationAngle = 0.0f;
	int rotationType = 0;
	float curAimTime = 0.0f;
	bool isAimEnd = false;
};
