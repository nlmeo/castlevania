#pragma once
#include "SoulSkill.h"
class S_Axe : public SoulSkill
{
public:
	virtual void Init();
	virtual void Start();
	virtual void Update(float deltaTime);
};