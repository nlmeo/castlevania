#pragma once
#include "./GamePlay/GameObject.h"

class ZombiPart : public GamePlay::GameObject
{
public:
	ZombiPart();

	void Init(const wchar_t* frameKey, MathType::vec2f startPos, float direction = 1.0f);

	void Update(float deltaTime) override;
	void OnDebugRender();
private:
	GamePlay::Animator* mAnimator = nullptr;    
	GamePlay::BoxCollider* mCollider = nullptr; 

	float mVx = 0;           
	float mVy = 0;           
	float mGravity = 800.0f; 

	bool mOnGround = false;                    // 是否已落地
	static constexpr unsigned int mGroundMask = ToBit(GamePlay::Obstacle) | ToBit(GamePlay::Platform) |
		ToBit(GamePlay::EdgePlatform) | ToBit(GamePlay::LeftSlop) | ToBit(GamePlay::RightSlop);
	void createAnim(const wchar_t* frameKey); 

	// 碰撞回调：玩家碰撞拾取 / 地形碰撞落地
	void onCollision(GamePlay::Collider* other);
};
