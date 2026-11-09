#include "Bat_Dead.h"
#include "GameTool.h"
#include "EffectsPool.h"
#include "./HelpTool/RandomMaker.h"
#include "BloodParticle.h"
void Bat_Dead::OnEnter()
{
	mAttributes->cursp = 0;
	mAttributes->is_corpse = true;

	float attackDir = (mOwner->PlayerDistanceX() > 0) ? -1.0f : 1.0f;

	MathType::vec2f pos = mOwner->transform().position();

	MathType::vec2f dir = { attackDir,1 };
	dir.Normalize();

	BloodParticle::Emit(pos, dir, 50);

	mOwner->SetActive(false);
}

void Bat_Dead::OnUpdate(float delta_time)
{
}

void Bat_Dead::OnQuit()
{
}
