#pragma once
#include "GamePlay/GameObject.h"

class LeftSlop : public GamePlay::GameObject
{
public:
	LeftSlop();
	void SetSize(float w, float h);
	void SetPosition(float x, float y);
	void OnDebugRender();
};


class RightSlop : public GamePlay::GameObject
{
public:
	RightSlop();
	void SetSize(float w, float h);
	void SetPosition(float x, float y);
	void OnDebugRender();
};