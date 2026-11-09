#pragma once
#include "SoulSkill.h"

class S_Fly : public SoulSkill
{
public:
	virtual void Init();
	virtual void Start();
	virtual void Update(float deltaTime);
	virtual void OnDestroy()  override;
private:
	GamePlay::SpriteRender* a1 = nullptr;
	GamePlay::SpriteRender* a2 = nullptr;
	GamePlay::SpriteRender* a3 = nullptr;
	GamePlay::Animator* a4 = nullptr;
	float mRotation = 0.0f;
};