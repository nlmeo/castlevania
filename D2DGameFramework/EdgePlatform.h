#pragma once
#include "GamePlay/GameObject.h"

class EdgePlatform : public GamePlay::GameObject
{
public:
	EdgePlatform();
	void SetSize(float w, float h);
	void SetPosition(float x, float y);
	void OnDebugRender();
};