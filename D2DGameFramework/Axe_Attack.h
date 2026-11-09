#pragma once
#include "AxeStates.h"

class Axe_Attack : public AxeStates
{
public:
	virtual void OnEnter();
	virtual void OnUpdate(float delta_time = 0);
	virtual void OnQuit();
private:
	bool mBulletSpawned = false;
	float mSpawnTimer = 0;
	float mSpawnTime = 1.633f;
	bool mIsAttack1 = false;
};
