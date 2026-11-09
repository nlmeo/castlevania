#pragma once
#include "./GamePlay/GameObject.h"


class SoulSkill : public GamePlay::GameObject
{
public:
	int mSoulId; //唯一id,在特殊处理中用于识别并进行相应操作
	int mInputType; //触发类型： 1-开关类型 2-持续按压
	int mRunType; //运行类型: 1-不需要接管角色状态（附加效果） 2-临时角色状态（类似枪兵旋转技能）
	virtual void Init() {};
	virtual void Start() {};
	virtual void Update(float deltaTime) {};
	virtual void End() {};

protected:
	bool TrySustain(float deltaTime);
	void ResetStaminaUsage() { mStaminaExhausted = false; }
	bool mStaminaExhausted = false;
	std::wstring mFsmStateKey; // 子类在 Init() 中指定要接管的 FSM 状态键

	// 将角色 FSM 推入当前技能的状态
	void EnterFsmState();
		

	// 退出当前技能状态，恢复到正常状态
	void ExitFsmState();
		
	// 当前是否处于技能接管状态（含排队中）
	bool IsFsmActive();
	
};
