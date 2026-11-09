#include "Sword_Idle.h"

void Sword_Idle::OnEnter()
{
	mAnimator->Play(L"idle");
	mTransform->SetRotation(180.0f);
}

void Sword_Idle::OnUpdate(float delta_time)
{
	if (!mOwner) return;

	if (mOwner->GetOwner())
	{
		GamePlay::GameObject* fly = mOwner->GetOwner();
		float targetX = fly->transform().position().x + mOwner->GetFollowOffsetX();
		float targetY = fly->transform().position().y - Fly_Sword::FOLLOW_OFFSET_Y;

		MathType::vec2f targetPos(targetX, targetY);
		targetPos = targetPos.Lerp(mTransform->position(), delta_time * 4);
		

		mTransform->SetPosition(targetPos);

	}

	if (mOwner->IsAttacking())
	{
		if (mOwner->GetAtkType() == 1)
			mFSM->SetNextState(L"aim");
		else
			mFSM->SetNextState(L"attack2");
		
	}
}

void Sword_Idle::OnQuit()
{
}
