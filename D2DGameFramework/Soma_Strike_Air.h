#pragma once
#include "SomaStates.h"

// 空中飞踢：二段跳中按 S+SPACE（竖直下踢）或 S+D+SPACE（斜向下踢）进入
class Soma_Strike_Air : public SomaStates
{
public:
	virtual void OnEnter();
	virtual void OnUpdate(float delta_time = 0);
	virtual void OnQuit();

	// 进入状态前由二段跳设置：true=斜向，false=竖直向下
	void SetDiagonal(bool diagonal) { mDiagonal = diagonal; }

private:
	bool mDiagonal = false;
};
