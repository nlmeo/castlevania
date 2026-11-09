#pragma once
#ifndef __MY_ENEMY_H__
#define __MY_ENEMY_H__

#include "Attributes.h"
#include "GamePlay/GameObject.h"
#include "./TypeDefine/Color.h"


class Enemy : public GamePlay::GameObject
{
public:
	virtual void Hit(float atk,int atkType); //1-肉感 2-硬物

	virtual void FacePlayer();

	float PlayerDistanceX();

	float PlayerDistanceY();
	float DeathScatterDirection() const { return mDeathScatterDirection; }

	bool PlayerInRange();

	float DetectWith() const;

	float DetectHeight() const;

	void SetDebugColor(const ColorType::COLORF& c) { mDebugColor = c; }

	Attributes& GetAttributes() { return m_attributes; }

protected:
	// 致命受击时锁定，死亡动画结束后不再根据玩家的新位置改变飞散方向。
	void CaptureDeathScatterDirection();
	float mDeathScatterDirection = 1.0f;
	Attributes m_attributes;                           
	MathType::vec2f mDetectRange = { 200.0f, 200.0f }; 
	ColorType::COLORF mDebugColor = { 0.2f, 0.5f, 1.0f, 0.3f }; 
};

#endif
