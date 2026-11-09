#pragma once
#include "./GamePlay/GameObject.h"

class AxePart : public GamePlay::GameObject
{
public:
	AxePart();

	// 初始化尸块：frameKey 为纹理键，startPos 为生成位置，direction 为溅射方向（+1右/-1左）
	void Init(const wchar_t* frameKey, MathType::vec2f startPos, float direction = 1.0f);

	void Update(float deltaTime) override;

private:
	GamePlay::Animator* mAnimator = nullptr;
	GamePlay::BoxCollider* mCollider = nullptr;

	float mVx = 0;
	float mVy = 0;
	float mGravity = 800.0f;

	void createAnim(const wchar_t* frameKey);

	bool mOnGround = false;                    // 是否已落地
	static constexpr unsigned int mGroundMask = ToBit(GamePlay::Obstacle) | ToBit(GamePlay::Platform) |
		ToBit(GamePlay::EdgePlatform) | ToBit(GamePlay::LeftSlop) | ToBit(GamePlay::RightSlop);

	// 碰撞回调：玩家碰撞拾取 / 地形碰撞落地
	void onCollision(GamePlay::Collider* other);
};