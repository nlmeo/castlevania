#include "Fly_Dead.h"
#include "GameTool.h"
#include "Fly_Sword.h"
void Fly_Dead::OnEnter()
{
	mAttributes->cursp = 0;
	mAttributes->is_corpse = true;

	// Boss 死亡立即关闭所有攻击碰撞
	mOwner->SetAttackActive(false);
	mOwner->GetSwordLeft()->SetAttackActive(false);
	mOwner->GetSwordRight()->SetAttackActive(false);

	mAnimator->Play(L"fly_dead");
}

void Fly_Dead::OnUpdate(float delta_time)
{
	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{
		mOwner->SetActive(false);
		mOwner->GetSwordLeft()->SetActive(false);
		mOwner->GetSwordRight()->SetActive(false);
		mOwner->GetCloak()->SetActive(false);
	}
}

void Fly_Dead::OnQuit()
{
}
