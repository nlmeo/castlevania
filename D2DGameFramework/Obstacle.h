#pragma once
#include "GamePlay/GameObject.h"

class Obstacle : public GamePlay::GameObject
{
public:
	Obstacle();
	void SetSize(float w, float h);
	void SetPosition(float x, float y);
	void OnDebugRender();
};