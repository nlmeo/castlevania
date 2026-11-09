#pragma once
#ifndef __GAME_CONTENT_OBJ_AXE_BULLET_H__
#define __GAME_CONTENT_OBJ_AXE_BULLET_H__
#include "./GamePlay/GameObject.h"
#include "Attributes.h"
#include <unordered_map>

class Enemy;

class AxeBulletP : public GamePlay::GameObject
{
public:
	AxeBulletP(); // 等级 123
	virtual void Awake() override;
	virtual void Update(float deltaTime) override;
	virtual void OnDestroy() override;
	virtual void OnDebugRender() override;
	void UpdatePhysics(float deltaTime);
	void SetDirection(int dx, int dy);
	void SetSpeed(float spd);
	void SetJumpPower(float jp);
	void SetGravity(float g);
	void SetPosition(float x, float y);
	void SetLevel(int level);
private:
	void onCollision(GamePlay::Collider* c);
	GamePlay::GameObject* mBodyCollider;
	float m_angle = 0;
	Attributes mAttributes;

	// TimeInHit 模式 - 默认
	struct HitRecord
	{
		float lastHitTime = 0.0f;
		int   count = 0;
	};
	std::unordered_map<Enemy*, HitRecord> mHitRecords;
	float mElapsed = 0.0f;
	int   mMaxHitsPerEnemy = 2;
	float mHitInterval = 0.2f;
};
#endif
