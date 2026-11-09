#pragma once
#include "SoulSkill.h"

class S_BlackPather : public SoulSkill
{
public:
	virtual void Init();
	virtual void Start();
	virtual void Update(float deltaTime);
	void OnDestroy() override;
private:
	GamePlay::Animator* a1 = nullptr;
};
