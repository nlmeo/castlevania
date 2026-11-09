#pragma once
#include "SomaStates.h"

class Soma_Slide_Flat : public SomaStates
{
	int mOriginalAttack = 5;
public:
	virtual void OnEnter();
	virtual void OnUpdate(float delta_time = 0);
	virtual void OnQuit();
private:
	float mInitSpeed = 14.0f;
	float mDeceleration = 0.92f;
	float mTimePass = 0.0f;
	float mTimeGap = 0.01f;
	int mMaxEffectCount = 15;
	int mCurEffectCount = 0;
};
