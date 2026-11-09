#pragma once
#include "SomaStates.h"

// 灵魂技能 FSM 状态基类
// 所有 mRunType=2（接管角色状态）的魂技能 FSM 状态从此继承
class SoulState : public SomaStates
{
protected:
	// 恢复到正常状态（地面→idle，空中→fall）
	void BackToNormalState()
	{
		if (mOwner->OnGround())
			mFSM->SetNextState(L"idle");
		else
			mFSM->SetNextState(L"fall");
	}
};
