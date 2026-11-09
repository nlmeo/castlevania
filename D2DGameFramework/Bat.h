#pragma once
#ifndef __GAME_CONTENT_OBJ_BAT_H__
#define __GAME_CONTENT_OBJ_BAT_H__

#include "Enemy.h"
#include "Attributes.h"
#include "GamePlay/CollisionMatrix.h"
#include "GamePlay/ChildObject.h"

class Soma;

class Bat : public Enemy
{
public:
	Bat();
	void OnDebugRender();
	Attributes& GetAttributes() { return m_attributes; }
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update(float deltaTime) override;
	virtual void OnDestroy() override;
	virtual void Hit(float atk, int atkType);
	void SetDebugColor(const ColorType::COLORF& c) { mDebugColor = c; }
	Soma* FindPlayerInRange(float rangeX, float rangeY);
	static float GetDetectWidth() { return 768.0f; }
	static float GetDetectHeight() { return 192.0f; }
private:
	Attributes m_attributes;
	GamePlay::ChildObject* mBodyCollider = nullptr;
	ColorType::COLORF mDebugColor = { 0.2f, 0.5f, 1.0f, 0.3f };
	void onCollision(GamePlay::Collider* c);
	void initAnim();
	void initFSM();
};
#endif
