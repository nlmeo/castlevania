#pragma once
#include "SwordStates.h"

/**
 * @brief 飞剑攻击状态
 *
 * 播放攻击动画后沿攻击方向飞出，超出相机范围切 withdraw
 * 若动画播放后超过 MAX_ATK_TIME 未飞出边界则直接切 idle
 */
class Sword_Attack : public SwordStates
{
public:
	virtual void OnEnter();
	virtual void OnUpdate(float delta_time = 0);
	virtual void OnQuit();
private:
	float mAtkTimer = 0.0f;                   // 攻击计时器（动画播放完后开始累加）
	static constexpr float MAX_ATK_TIME = 2.0f; // 最大攻击时长（秒）
};
