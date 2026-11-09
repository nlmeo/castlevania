#include "Soma_Dead.h"
#include "AttackEffect.h"

void Soma_Dead::OnEnter()
{
	mAttributes->is_dead = true;
	mAttributes->is_hit = false;
	mAttributes->is_hit_state_change = false;
	mAttributes->cursp = 0;
	mAttributes->curjp = 0;
	mAttributes->dx = 5;
	mAttributes->dy = mOwner->OnGround() ? 5 : 2;
	mOwner->EnableSlideAtkCollider(false);
	if (auto* effect = mOwner->GetAttackEffect()) effect->Deactivate();
	mAnimator->Play(L"dead");
}

void Soma_Dead::OnUpdate(float)
{
	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED) mAttributes->is_corpse = true;
}
