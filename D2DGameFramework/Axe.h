#pragma once
#ifndef __GAME_CONTENT_OBJ_AXE_H__
#define __GAME_CONTENT_OBJ_AXE_H__

#include "Enemy.h"
#include "GamePlay/CollisionMatrix.h"
#include "GamePlay/ChildObject.h"

class Soma;

class Axe : public Enemy
{
public:
	Axe();
	void OnDebugRender();
	void SavePos();
	virtual void Awake() override;
	virtual void OnEnable()  override;
	virtual void Start()  override;
	virtual void Update(float deltaTime)  override;
	virtual void LaterUpdate(float deltaTime) override;
	virtual void OnDisable()  override;
	virtual void OnDestroy()  override;

	virtual void FacePlayer() override;

	virtual void Hit(float atk,int atkType) override;

	bool OnSlop() { return mOnSlop; }
	bool OnGround() { return mOnGround; }
	bool OnCollideTop() { return mOnCollideTop; }
	void SetOnSlop(bool b) { mOnSlop = b; }
	void SetOnGround(bool b) { mOnGround = b; }
	void SetOnCollideTop(bool b) { mOnCollideTop = b; }
	void UpdatePhysics(float deltaTime);
	void EvaluateTransitions();
	void CheckGroundBelow();

private:
	bool mOnSlop = false;
	bool mOnGround = true;
	bool mOnCollideTop = false;
	bool m_wasOnGround = false;
	bool m_wasOnSlop = false;
	bool m_wasOnCollideTop = false;
	GamePlay::ChildObject* mBodyCollider = nullptr;
	MathType::RectF mLastPos;
	void onCollision(GamePlay::Collider* c);
	void initAnim();
	void initFSM();

	static constexpr unsigned int mCollisionFilter =
		ToBit(GamePlay::Obstacle) | ToBit(GamePlay::LeftSlop) |
		ToBit(GamePlay::RightSlop) | ToBit(GamePlay::Platform) | ToBit(GamePlay::EdgePlatform);
};
#endif
