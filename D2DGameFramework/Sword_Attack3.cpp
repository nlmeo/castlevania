#include "Sword_Attack3.h"

void Sword_Attack3::OnEnter()
{
	const MathType::vec2f& atkDir = mOwner->GetAtkDir();

	mAimRotationAngle = MathType::vec2f::down().SignedAngleDegree(atkDir);
	mAimRotationType = (mAimRotationAngle > 0) ? 0 : 1;

	mTransform->SetRotation(180.0f);
	mAimTimer = 0.0f;
	mPhase = PHASE_AIM;
}

void Sword_Attack3::OnUpdate(float delta_time)
{
	if (!mOwner) return;

	if (mPhase == PHASE_AIM)
	{

		float rAngle = AIM_ROT_SPEED * delta_time;
		bool aimDone = false;

		if (mAimRotationType == 0)
		{
			if (mAimRotationAngle - rAngle <= 0)
			{
				rAngle = mAimRotationAngle;
				aimDone = true;
			}
			mAimRotationAngle -= rAngle;
			mTransform->rotate(rAngle);
		}
		else
		{
			if (mAimRotationAngle + rAngle >= 0)
			{
				rAngle = -mAimRotationAngle;
				aimDone = true;
			}
			mAimRotationAngle += rAngle;
			mTransform->rotate(-rAngle);
		}

		mAimTimer += delta_time;
		if (mAimTimer >= MAX_AIM_TIME)
			aimDone = true;

		if (aimDone)
			mPhase = PHASE_FLY;
	}
	else if (mPhase == PHASE_FLY)
	{

		if (mAnimator->GetState() == GamePlay::AS::AS_PLAYING)
			return;

		mFlyTimer += delta_time;

		mTransform->translate(mOwner->GetAtkDir() * delta_time * FLY_SPEED);

		if (mFlyTimer >= MAX_FLY_TIME)
			mPhase = PHASE_WITHDRAW;
	}
	else if (mPhase == PHASE_WITHDRAW)
	{

		if (!mOwner->GetOwner()) return;
		GamePlay::GameObject* fly = mOwner->GetOwner();

		float targetX = fly->transform().position().x + mOwner->GetFollowOffsetX();
		float targetY = fly->transform().position().y - Fly_Sword::FOLLOW_OFFSET_Y;

		if (!mWithdrawAboveDone)
		{
		
			mTransform->SetPosition({ targetX, targetY - ABOVE_OFFSET });

			mWithdrawRotDir = (mOwner->GetType() == 0) ? 1 : 0;

			float curRot = mTransform->rotation();
			constexpr float TARGET = 180.0f;

			if (mWithdrawRotDir == 0)
			{
				if (curRot < TARGET)
					mWithdrawRotRemain = TARGET - curRot;
				else if (curRot > TARGET)
					mWithdrawRotRemain = (360.0f - curRot) + TARGET;
				else
					mWithdrawRotRemain = 0.0f;
			}
			else
			{
				if (curRot > TARGET)
					mWithdrawRotRemain = curRot - TARGET;
				else if (curRot < TARGET)
					mWithdrawRotRemain = curRot + (360.0f - TARGET);
				else
					mWithdrawRotRemain = 0.0f;
			}

			mWithdrawAboveDone = true;
		}
		else
		{
			MathType::vec2f targetPos = { targetX, targetY };
			MathType::vec2f curPos = mTransform->position();
			MathType::vec2f toTarget = targetPos - curPos;
			float dist = toTarget.magnitude();

			float moveStep = WITHDRAW_MOVE_SPEED * delta_time;
			if (moveStep >= dist)
				mTransform->SetPosition(targetPos);
			else
				mTransform->translate(toTarget.normalized() * moveStep);

			if (mWithdrawRotRemain > 0.01f)
			{
				float rotStep = WITHDRAW_ROT_SPEED * delta_time;
				if (rotStep > mWithdrawRotRemain)
					rotStep = mWithdrawRotRemain;

				mTransform->rotate(mWithdrawRotDir == 0 ? rotStep : -rotStep);
				mWithdrawRotRemain -= rotStep;
			}

			dist = (mTransform->position() - targetPos).magnitude();
			if (mWithdrawRotRemain <= 0.01f && dist < 1.0f)
			{
				mTransform->SetRotation(180.0f);
				mFSM->SetNextState(L"idle");
			}
		}
	}
}

void Sword_Attack3::OnQuit()
{
	mOwner->SetAttacking(false);
}
