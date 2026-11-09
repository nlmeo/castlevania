#pragma once
#include "SomaStates.h"

class Soma_Land : public SomaStates
{
public:
	virtual void OnEnter();
	virtual void OnUpdate(float delta_time = 0);
	virtual void OnQuit();
private:
	bool canChange = false;
private:
	float mInitSpeed = 14.0f;
	float mDeceleration = 0.92f;
	float mTimePass = 0.0f;
	float mTimeGap = 0.03f;
	int mMaxEffectCount = 6;
	int mCurEffectCount = 0;
};