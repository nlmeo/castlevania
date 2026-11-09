#pragma once
#ifndef __FLY_IDLE_H__
#define __FLY_IDLE_H__
#include "FlyStates.h"

/**
 * @brief Fly 待机游荡状态 —— 三层运动叠加架构
 *
 * 最终渲染坐标 = 锚点 + 悬浮偏移 × 过渡修正系数
 *
 * 锚点层承担全部逻辑判定，悬浮层仅为 Y 轴视觉表现。
 * TRANS_ENTER/POST_ENTER 期间不写入 Transform，
 * 确保场景中 SetPosition 的初始坐标不被覆盖。
 */
class Fly_Idle : public FlyStates
{
public:
	virtual void OnEnter();
	virtual void OnUpdate(float delta_time = 0);
	virtual void OnQuit();
private:
	// ===== 锚点层（逻辑位移核心） =====
	enum MovePhase { PHASE_WAIT, PHASE_MOVE };
	MovePhase mMovePhase = PHASE_WAIT;

	float mAnchorX = 0.0f;              // 逻辑锚点 X
	float mAnchorY = 0.0f;              // 逻辑锚点 Y
	float mSpawnY = 0.0f;               // 出生 Y 基准（高度带中心）

	float mWaitTimer = 0.0f;            // 停留计时器
	float mWaitDuration = 0.0f;         // 本次停留时长 1.2~2.0 秒

	float mMoveTargetX = 0.0f;          // 当前移动目标 X
	float mMoveTargetY = 0.0f;          // 当前移动目标 Y
	float mMoveSpeedX = 0.0f;           // 当前水平速度 px/s
	float mMoveSpeedY = 0.0f;           // 当前垂直速度 px/s

	float mSampleTimer = 0.0f;          // 目标重采样计时器（10 帧 / 次）

	// ===== 悬浮层（纯视觉，仅 Y 轴） =====
	float mHoverTimer = 0.0f;           // 悬浮正弦计时器
	float mHoverAmplitude = 0.0f;       // 悬浮振幅 8~10 px（OnEnter 随机）
	float mHoverPhase = 0.0f;           // 悬浮随机初始相位 0~2PI
	float mJitterTimer = 0.0f;          // 噪声更新计时器
	float mJitterValue = 0.0f;          // 当前噪声偏移量 px

	// ===== 过渡状态 =====
	enum TransState { TRANS_ENTER, TRANS_POST_ENTER, TRANS_EXIT, TRANS_NORMAL };
	TransState mTransState = TRANS_ENTER;
	float mTransTimer = 0.0f;
	float mTransSpeedMult = 0.0f;       // 锚点速度修正系数，0→1 平滑过渡
	float mTransHoverMult = 0.0f;       // 悬浮振幅修正系数，0→1 平滑过渡

	// ===== 辅助函数 =====
	void PickNewTarget();               // 采样新目标点（含弱追踪逻辑）
	void UpdateAnchorNormal(float delta_time);  // 锚点常速平移 + 软边界衰减
	float CalcHover(float delta_time);  // 正弦悬浮 + 微颤噪声
	class Soma* FindPlayerGlobal() const;  // 全局查找玩家

	// ===== 可配置常量（基于 60fps delta-time 标定） =====
	static constexpr float WAIT_MIN = 1.2f;             // 最短停留时长
	static constexpr float WAIT_MAX = 2.0f;             // 最长停留时长
	static constexpr float H_SPEED_MIN = 24.0f;         // 水平最低速度（0.8*30）
	static constexpr float H_SPEED_MAX = 30.0f;         // 水平最高速度（1.0*30）
	static constexpr float V_SPEED_MIN = 9.0f;          // 垂直最低速度（0.3*30）
	static constexpr float V_SPEED_MAX = 15.0f;         // 垂直最高速度（0.5*30）
	static constexpr float ARRIVE_DIST = 5.0f;          // 到达判定（曼哈顿距离）
	static constexpr float OFFSET_X_MIN = 80.0f;        // X 随机偏移下限
	static constexpr float OFFSET_X_MAX = 120.0f;       // X 随机偏移上限
	static constexpr float OFFSET_Y_MIN = 20.0f;        // Y 随机偏移下限
	static constexpr float OFFSET_Y_MAX = 30.0f;        // Y 随机偏移上限
	static constexpr float TRACK_CLOSE = 60.0f;         // 距离玩家 < 此值 → 远离
	static constexpr float TRACK_FAR = 180.0f;          // 距离玩家 > 此值 → 靠近
	static constexpr float TRACK_PROB = 0.7f;           // 弱追踪概率
	static constexpr float BOUND_SOFT = 20.0f;          // 软边界距离
	static constexpr float HOVER_AMP_MIN = 8.0f;        // 悬浮振幅下限
	static constexpr float HOVER_AMP_MAX = 10.0f;       // 悬浮振幅上限
	static constexpr float HOVER_FREQ = 0.8f;           // 悬浮频率 Hz
	static constexpr float JITTER_AMP = 1.0f;           // 微颤噪声振幅
	static constexpr float JITTER_INTERVAL = 0.05f;     // 噪声更新间隔（3 帧）
	static constexpr float SAMPLE_INTERVAL = 0.167f;    // 目标采样间隔（10 帧）
	static constexpr float ENTER_LEAP_DUR = 0.042f;     // 进入过渡时长（2.5 帧）
	static constexpr float ENTER_WAIT_DUR = 0.5f;       // 进入后强制停留
	static constexpr float EXIT_DUR = 0.067f;           // 退出过渡时长（4 帧）
	static constexpr float EXIT_HOVER_RATIO = 0.5f;     // 退出时悬浮残留比例
};
#endif
