#pragma once
#include "GamePlay/GameObject.h"

class Platform : public GamePlay::GameObject
{
public:
	Platform();
	void SetSize(float w, float h);
	void SetPosition(float x, float y);
	void OnDebugRender();
};