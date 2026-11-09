#pragma once
#include "./GamePlay/GameObject.h"
#include "AttackCollider.h"

class Soma;

class AttackEffect : public GamePlay::GameObject
{
public:
	AttackEffect();
	void Update(float deltaTime) override;
	void Activate(bool flipX,bool isSuperAttack = false);
	void Deactivate();
	virtual void SetOwner(Soma* obj) { mOwner = obj; };
	virtual void OnDebugRender();

	void SetOneHit(int triggerFrame);
	void SetTimeInHit(int startFrame, int endFrame, float hitInterval, int maxHitsPerEnemy);

private:
	GamePlay::SpriteRender* mSp = nullptr;
	GamePlay::Animator* mAnimator = nullptr;
	AttackCollider* mAttackCollider = nullptr;
	Soma* mOwner = nullptr;

	AttackCollider::HitMode mHitMode = AttackCollider::HitMode::OneHit;

	// 帧窗口参数
	int mOneHitFrame = 3;
	int mHitStartFrame = 0;
	int mHitEndFrame = 0;
	float mHitInterval = 0.0f;
	int mMaxHitsPerEnemy = 0;
	bool mColliderInWindow = false;

	float mCurrentRotation = -120.0f;
	float mRotationSpeed = 280.0f;

	bool mIsSuperAttack = false;
	void initAnim();
};
