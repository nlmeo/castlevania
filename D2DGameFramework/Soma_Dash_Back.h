#pragma once
#include "SomaStates.h"

class Soma_Dash_Back : public SomaStates
{
public:
	virtual void OnEnter();
	virtual void OnUpdate(float delta_time = 0);
	virtual void OnQuit();
private:
	float mInitSpeed = 15.0f;
	float mDeceleration = 0.9f;
	float mMinSpeed = 1.0f;
	float mTimePass = 0.0f;
	float mTimeGap = 0.01f;
	int mMaxEffectCount = 20;
	int mCurEffectCount = 0;
};
