#include "Sword_WithDraw.h"

void Sword_WithDraw::OnEnter()
{
	if (!mOwner || !mOwner->GetOwner()) return;

	mRotateDir = (mOwner->GetType() == 0) ? 1 : 0;

	float currentRot = mTransform->rotation();
	constexpr float TARGET_ROT = 180.0f;

	if (mRotateDir == 0)
	{
		if (currentRot < TARGET_ROT)
			mRemainRotate = TARGET_ROT - currentRot;
		else if (currentRot > TARGET_ROT)
			mRemainRotate = (360.0f - currentRot) + TARGET_ROT;
		else
			mRemainRotate = 0.0f;
	}
	else
	{
		if (currentRot > TARGET_ROT)
			mRemainRotate = currentRot - TARGET_ROT;
		else if (currentRot < TARGET_ROT)
			mRemainRotate = currentRot + (360.0f - TARGET_ROT);
		else
			mRemainRotate = 0.0f;
	}

	mPhase = MOVE_ABOVE;
}

void Sword_WithDraw::OnUpdate(float delta_time)
{
	if (!mOwner || !mOwner->GetOwner()) return;

	GamePlay::GameObject* fly = mOwner->GetOwner();
	float targetX = fly->transform().position().x + mOwner->GetFollowOffsetX();
	float targetY = fly->transform().position().y - Fly_Sword::FOLLOW_OFFSET_Y;
	MathType::vec2f targetPos = { targetX, targetY };

	if (mPhase == MOVE_ABOVE)
	{
		mTransform->SetPosition({ targetX, targetY - ABOVE_OFFSET });
		mPhase = MOVE_TO_POS;
	}
	else if (mPhase == MOVE_TO_POS)
	{
		MathType::vec2f curPos = mTransform->position();
		MathType::vec2f toTarget = targetPos - curPos;
		float dist = toTarget.magnitude();

		float moveStep = MOVE_SPEED * delta_time;
		if (moveStep >= dist)
			mTransform->SetPosition(targetPos);
		else
			mTransform->translate(toTarget.normalized() * moveStep);

		if (mRemainRotate > 0.01f)
		{
			float rotateStep = ROTATE_SPEED * delta_time;
			if (rotateStep > mRemainRotate)
				rotateStep = mRemainRotate;

			mTransform->rotate(mRotateDir == 0 ? rotateStep : -rotateStep);
			mRemainRotate -= rotateStep;
		}

		dist = (mTransform->position() - targetPos).magnitude();

		if (mRemainRotate <= 0.01f && dist < 1.0f)
		{
			mFSM->SetNextState(L"idle");
		}
	}
}

void Sword_WithDraw::OnQuit()
{
	mOwner->SetAttacking(false);
}
