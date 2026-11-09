#pragma once

#include "GameUI/UICanvas.h"
#include "GameUI/UIImage.h"
#include "BufferBlock.h"
class BossHP
{
	GameUI::UIImage* mBackGround = nullptr;
	GameUI::UIImage* mHPBar = nullptr;
	BufferBlock mBufferBlock;
	float mLastHP = -1;
	bool mIsInit = false;
public:
	BossHP() = default;
	~BossHP();
	BossHP(const BossHP&) = delete;
	BossHP& operator=(const BossHP&) = delete;
	void Init(GameUI::UICanvas* p, float maxHP);
	void Update(float currentHP, float maxHP,float deltaTime);
	bool IsInit() { return mIsInit; }

	void Intro() {};
	void Disappear();
};
