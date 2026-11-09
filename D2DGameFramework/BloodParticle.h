#pragma once
#ifndef __BLOOD_PARTICLE_H__
#define __BLOOD_PARTICLE_H__

#include "./GamePlay/GameObject.h"
#include "./TypeDefine/Vector2.h"

/**
 * @brief 血液粒子对象
 *
 * 参考 ZombiPart 碎块的参数风格：
 * - 强向上初速度 + 大重力形成清晰抛物线轨迹
 * - 水平速度 150~300，沿 baseDir 方向飞溅
 * - 每个粒子独立随机重力 ±20%，速度 ±20%，各走不同弧线
 * - 约 1 秒后缩小并透明消失
 *
 * 使用：
 *   BloodParticle::Emit(spawnPos, { 1.0f, 0.0f });     // 向右喷6粒
 *   BloodParticle::Emit(spawnPos, { -1.0f, 0.0f }, 4); // 向左喷4粒
 */
class BloodParticle final : public GamePlay::GameObject
{
public:
	BloodParticle();

	/** @brief 每帧更新：抛物线运动 + 缩小透明衰减 */
	void Update(float deltaTime) override;

	/**
	 * @brief 生成一组血液粒子
	 *
	 * @param spawnPos  出生点（世界坐标）
	 * @param baseDir   喷射方向（单位向量）
	 * @param count     粒子数量（默认6）
	 */
	static void Emit(const MathType::vec2f& spawnPos,
		const MathType::vec2f& baseDir,
		int count = 6);

private:
	void DestroyThis();
	void Init(const MathType::vec2f& spawnPos,
		const MathType::vec2f& vel,
		float dissolveTime);

	GamePlay::SpriteRender* mpSprite = nullptr;

	MathType::vec2f mVelocity;
	float mElapsed = 0.0f;
	float mLifeTime = 1.0f;
	float mGravity = 600.0f;     ///< 每粒子独立随机重力加速度

	float mInitialScale = 1.0f;
	float mFinalScale = 0.01f;

	// === 参数常量，对标 ZombiPart ===
	static constexpr float VX_MIN = 200.0f;       ///< 水平速度下界
	static constexpr float VX_MAX = 600.0f;       ///< 水平速度上界
	static constexpr float VY_MIN = 150.0f;       ///< 垂直初速度下界（负=上）
	static constexpr float VY_MAX = 450.0f;       ///< 垂直初速度上界（负=上）
	static constexpr float LIFE_MIN = 1.0f;         ///< 最短消散时间
	static constexpr float LIFE_MAX = 3.0f;         ///< 最长消散时间
	static constexpr float GRAVITY_BASE = 600.0f;   ///< 基础重力
	static constexpr float GRAVITY_VAR = 0.2f;      ///< 重力浮动比例
	static constexpr float SCATTER_ANGLE = 22.5f;    ///< 方向散射半角（度）
};
#endif
