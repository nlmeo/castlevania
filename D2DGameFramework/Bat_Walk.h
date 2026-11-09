#pragma once
#include "BatStates.h"
#include "TypeDefine/Vector2.h"

class Bat_Walk : public BatStates
{
public:
	virtual void OnEnter();
	virtual void OnUpdate(float delta_time = 0);
	virtual void OnQuit();
private:
	MathType::vec2f mDir;
};
