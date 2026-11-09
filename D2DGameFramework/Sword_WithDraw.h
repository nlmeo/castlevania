#pragma once
#ifndef __SWORD_WITHDRAW_H__
#define __SWORD_WITHDRAW_H__
#include "SwordStates.h"

/**
 * @brief 飞剑收刀状态
 *
 * 第一阶段：瞬移到宿主悬挂点上方，不改变当前旋转角度
 * 第二阶段：平滑移动到悬挂点，同时旋转回正到180度（刀尖朝上）
 *           左剑(type=0)逆时针旋转回正，右剑顺时针旋转回正
 *           到位且回正后切换到 idle
 */
class Sword_WithDraw : public SwordStates
{
public:
	virtual void OnEnter();
	virtual void OnUpdate(float delta_time = 0);
	virtual void OnQuit();
private:
	enum Phase { MOVE_ABOVE, MOVE_TO_POS };
	Phase mPhase = MOVE_ABOVE;
	int mRotateDir = 0;             // 0: 顺时针(右剑), 1: 逆时针(左剑)
	float mRemainRotate = 0.0f;     // 剩余需旋转角度（度）

	static constexpr float ABOVE_OFFSET = 100.0f;   // 悬挂点上方偏移
	static constexpr float MOVE_SPEED = 200.0f;      // 第二段移动速度
	static constexpr float ROTATE_SPEED = 360.f;    // 旋转速度（度/秒）
};
#endif
