#pragma once
#ifndef __GAME_CONTENT_OBJ_AXE_BULLET_E_H__
#define __GAME_CONTENT_OBJ_AXE_BULLET_E_H__
#include "./GamePlay/GameObject.h"
#include "Attributes.h"

class AxeBulletE : public GamePlay::GameObject
{
public:
	AxeBulletE();
	virtual void Awake() override;
	virtual void Update(float deltaTime) override;
	virtual void OnDestroy() override;
	virtual void OnDebugRender();
	void Init(int type, int dirX, MathType::vec2f startPos, float speed, float jumpPower, const wchar_t* textureKey);
	void UpdatePhysics(float deltaTime);
	void SetAtkType(int t);
private:
	void onCollision(GamePlay::Collider* c);
	int m_atkType = 1;
	int m_dirX = 6;
	GamePlay::GameObject* mBodyCollider = nullptr;
	float m_cursp = 5.0f;
	float m_curjp = 25.0f;
	float m_g = 1.0f;
	float m_angle = 0;
	bool mIsHited = false;
};
#endif
