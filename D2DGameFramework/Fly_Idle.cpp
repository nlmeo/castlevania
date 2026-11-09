#include "Fly_Idle.h"
#include "GameTool.h"
#include "GameCore/Application.h"
#include "Soma.h"
#include <cmath>

void Fly_Idle::OnEnter()
{
	mOwner->SetDebugColor({ 0.2f, 0.5f, 1.0f, 0.3f });
	mAttributes->dx = 5;
	mAttributes->dy = 5;
	mAnimator->Play(L"fly_idle");

	// 以当前 transform 位置作为锚点起点
	// 注意：构造函数中 initFSM 会立即触发此函数，此时位置可能尚未被场景设置
	// 真正的同步在 TRANS_ENTER 结束时完成
	mAnchorX = mOwner->transform().position().x;
	mAnchorY = mOwner->transform().position().y;
	mSpawnY = mAnchorY;

	// 随机化悬浮参数（振幅 8~10px，相位 0~2PI）
	float range = HOVER_AMP_MAX - HOVER_AMP_MIN;
	mHoverAmplitude = HOVER_AMP_MIN + (float)(rand() % (int)(range + 1));
	mHoverPhase = (float)(rand() % 628) / 100.0f;
	mHoverTimer = 0.0f;
	mJitterTimer = 0.0f;
	mJitterValue = 0.0f;

	// 进入过渡：速度 / 悬浮系数从 0 线性升至 1
	mTransState = TRANS_ENTER;
	mTransTimer = 0.0f;
	mTransSpeedMult = 0.0f;
	mTransHoverMult = 0.0f;

	// 移动循环初始化
	mMovePhase = PHASE_WAIT;
	mWaitTimer = 0.0f;
	mWaitDuration = 0.0f;
	mSampleTimer = 0.0f;
}

void Fly_Idle::OnUpdate(float delta_time)
{
	// ===== 1. 过渡状态机 =====
	if (mTransState == TRANS_ENTER)
	{
		// ENTER：速度 / 悬浮系数 0→1 线性 lerp，时长 ~2.5 帧
		mTransTimer += delta_time;
		float t = (mTransTimer >= ENTER_LEAP_DUR) ? 1.0f : (mTransTimer / ENTER_LEAP_DUR);
		mTransSpeedMult = t;
		mTransHoverMult = t;

		if (t >= 1.0f)
		{
			// ENTER 结束时从 transform 重新同步锚点
			// 覆盖构造函数中可能错误的默认原点坐标
			mAnchorX = mOwner->transform().position().x;
			mAnchorY = mOwner->transform().position().y;
			mSpawnY = mAnchorY;

			mTransState = TRANS_POST_ENTER;
			mTransTimer = 0.0f;
		}
	}
	else if (mTransState == TRANS_POST_ENTER)
	{
		// 进入后强制停留 0.5 秒，给玩家视觉暂停
		mTransTimer += delta_time;
		if (mTransTimer >= ENTER_WAIT_DUR)
		{
			mTransState = TRANS_NORMAL;
			mMovePhase = PHASE_WAIT;
			mWaitTimer = 0.0f;
			mWaitDuration = WAIT_MIN + (float)(rand() % (int)((WAIT_MAX - WAIT_MIN) * 10.0f + 1)) / 10.0f;
		}
	}
	else if (mTransState == TRANS_EXIT)
	{
		// EXIT：速度衰减至 0，悬浮衰减至 50%，时长 ~4 帧
		mTransTimer += delta_time;
		float t = (mTransTimer >= EXIT_DUR) ? 1.0f : (mTransTimer / EXIT_DUR);
		mTransSpeedMult = 1.0f - t;
		mTransHoverMult = 1.0f - t * (1.0f - EXIT_HOVER_RATIO);

		if (t >= 1.0f)
		{
			mFSM->SetNextState(L"attack");
			return;
		}
	}

	// ===== 2. 锚点层移动（仅 NORMAL / EXIT 中执行） =====
	if (mTransState == TRANS_NORMAL || mTransState == TRANS_EXIT)
	{
		UpdateAnchorNormal(delta_time);
	}

	// ===== 3. 悬浮偏移计算 =====
	float hoverY = CalcHover(delta_time) * mTransHoverMult;

	// ===== 4. 渲染坐标 = 锚点 + 悬浮偏移 =====
	// TRANS_ENTER/POST_ENTER 期间不写入位置，保持场景初始坐标不变
	if (mTransState != TRANS_ENTER && mTransState != TRANS_POST_ENTER)
	{
		const auto position = mOwner->ClampFlightPosition({ mAnchorX, mAnchorY + hoverY });
		mOwner->transform().SetPosition(position);
		mOwner->SetBodyColliderPos(position);
	}

	// ===== 5. 动画循环播放 =====
	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
		mAnimator->RePlay();

	// ===== 6. 攻击 CD 递减（按帧计数） =====
	if (mAttributes->cur_atk_cd > 0)
		mAttributes->cur_atk_cd -= 1;

	// ===== 7. 检测玩家 → 触发退出过渡 =====
	if (mTransState == TRANS_NORMAL)
	{
		//Soma* player = mOwner->FindPlayerInRange(Fly::GetDetectWidth(), Fly::GetDetectHeight());
		
		if (!MainPlayer) { return; }
		Soma* player = dynamic_cast<Soma*>(MainPlayer);
		if (player && mAttributes->cur_atk_cd <= 0)
		{
			mTransState = TRANS_EXIT;
			mTransTimer = 0.0f;
		}
	}
}

void Fly_Idle::OnQuit()
{
}

// ===== 私有函数实现 =====

void Fly_Idle::PickNewTarget()
{
	Soma* player = FindPlayerGlobal();
	float halfW = (float)App.GetWidth() * 0.5f;

	// === X 轴目标：玩家坐标 + 随机偏移 ±80~120px ===
	float offsetMagnitude = OFFSET_X_MIN + (float)(rand() % (int)(OFFSET_X_MAX - OFFSET_X_MIN + 1));
	if (rand() % 2 == 0) offsetMagnitude = -offsetMagnitude;

	if (player)
	{
		float playerX = player->transform().position().x;
		float dx = playerX - mAnchorX;
		float absDist = fabsf(dx);

		// 弱追踪修正：远→靠近，近→远离
		if (absDist > TRACK_FAR)
		{
			// 距离 > 180px：70% 概率向玩家倾斜
			if ((float)rand() / RAND_MAX < TRACK_PROB)
				offsetMagnitude = fabsf(offsetMagnitude) * (dx > 0 ? 1.0f : -1.0f);
		}
		else if (absDist < TRACK_CLOSE)
		{
			// 距离 < 60px：70% 概率远离玩家
			if ((float)rand() / RAND_MAX < TRACK_PROB)
				offsetMagnitude = fabsf(offsetMagnitude) * (dx > 0 ? -1.0f : 1.0f);
		}

		mMoveTargetX = playerX + offsetMagnitude;
	}
	else
	{
		// 无玩家：在屏幕中段随机游荡
		mMoveTargetX = halfW + (float)(rand() % (int)(halfW)) * ((rand() % 2) ? 1.0f : -1.0f);
	}

	// 边界裁剪
	float leftBound = MainCamera->CameraBound().left + BOUND_SOFT;
	float rightBound = MainCamera->CameraBound().right - BOUND_SOFT;
	if (mMoveTargetX < leftBound) mMoveTargetX = leftBound;
	if (mMoveTargetX > rightBound) mMoveTargetX = rightBound;

	// === Y 轴目标：出生高度 ±20~30px，严格限制高度带 ===
	float yOffset = OFFSET_Y_MIN + (float)(rand() % (int)(OFFSET_Y_MAX - OFFSET_Y_MIN + 1));
	if (rand() % 2 == 0) yOffset = -yOffset;
	mMoveTargetY = mSpawnY + yOffset;

	// === 速度随机化（每次采样独立随机） ===
	mMoveSpeedX = H_SPEED_MIN + (float)(rand() % (int)((H_SPEED_MAX - H_SPEED_MIN) * 10.0f + 1)) / 10.0f;
	mMoveSpeedY = V_SPEED_MIN + (float)(rand() % (int)((V_SPEED_MAX - V_SPEED_MIN) * 10.0f + 1)) / 10.0f;
}

void Fly_Idle::UpdateAnchorNormal(float delta_time)
{
	float leftBound = MainCamera->CameraBound().left + BOUND_SOFT;
	float rightBound = MainCamera->CameraBound().right - BOUND_SOFT;

	if (mMovePhase == PHASE_WAIT)
	{
		// 停留阶段：等待随机时长后采样新目标
		mWaitTimer += delta_time;
		if (mWaitTimer >= mWaitDuration)
		{
			PickNewTarget();
			mMovePhase = PHASE_MOVE;
			mSampleTimer = 0.0f;
		}
	}
	else if (mMovePhase == PHASE_MOVE)
	{
		// 目标重采样：每 SAMPLE_INTERVAL 秒更新一次，兼顾性能
		mSampleTimer += delta_time;
		if (mSampleTimer >= SAMPLE_INTERVAL)
		{
			mSampleTimer = 0.0f;
			PickNewTarget();
		}

		// 常速平移（无加速 / 减速），速度受过渡系数修正
		float dx = mMoveTargetX - mAnchorX;
		float dy = mMoveTargetY - mAnchorY;
		float stepX = (dx > 0 ? 1.0f : -1.0f) * mMoveSpeedX * delta_time * mTransSpeedMult;
		float stepY = (dy > 0 ? 1.0f : -1.0f) * mMoveSpeedY * delta_time * mTransSpeedMult;

		// 防止 overshoot
		if (fabsf(stepX) > fabsf(dx)) stepX = dx;
		if (fabsf(stepY) > fabsf(dy)) stepY = dy;

		// 软边界衰减：距边界 < BOUND_SOFT 时线性降速
		float distToLeft = mAnchorX - leftBound;
		float distToRight = rightBound - mAnchorX;

		if (distToLeft < BOUND_SOFT && stepX < 0)
			stepX *= (distToLeft / BOUND_SOFT);
		if (distToRight < BOUND_SOFT && stepX > 0)
			stepX *= (distToRight / BOUND_SOFT);

		mAnchorX += stepX;
		mAnchorY += stepY;

		// 到达判定：曼哈顿距离 < ARRIVE_DIST → 进入停留
		float manhattan = fabsf(mMoveTargetX - mAnchorX) + fabsf(mMoveTargetY - mAnchorY);
		if (manhattan < ARRIVE_DIST)
		{
			mAnchorX = mMoveTargetX;
			mAnchorY = mMoveTargetY;
			mMovePhase = PHASE_WAIT;
			mWaitTimer = 0.0f;
			mWaitDuration = WAIT_MIN + (float)(rand() % (int)((WAIT_MAX - WAIT_MIN) * 10.0f + 1)) / 10.0f;
		}
	}
}

float Fly_Idle::CalcHover(float delta_time)
{
	mHoverTimer += delta_time;

	const float twoPI = 6.283185307f;

	// 基础正弦悬浮：振幅 8~10px，频率 0.8Hz
	float hover = sinf(mHoverTimer * twoPI * HOVER_FREQ + mHoverPhase) * mHoverAmplitude;

	// 微随机噪声：每 0.05 秒更新一次 ±1px 偏移，打破完美正弦
	mJitterTimer += delta_time;
	if (mJitterTimer >= JITTER_INTERVAL)
	{
		mJitterTimer -= JITTER_INTERVAL;
		mJitterValue = ((float)(rand() % 3) - 1.0f) * JITTER_AMP;  // -1 / 0 / +1
	}

	return hover + mJitterValue;
}

Soma* Fly_Idle::FindPlayerGlobal() const
{
	for (auto* collider : GameObjectMgr.ColliderObjects())
	{
		GamePlay::GameObject* obj = collider->gameobject();
		if (!obj->Active() || obj->layer() != GamePlay::Player)
			continue;
		return static_cast<Soma*>(obj);
	}
	return nullptr;
}
