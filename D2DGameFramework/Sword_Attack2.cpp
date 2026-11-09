#include "Sword_Attack2.h"
#include <algorithm>
#include <cmath>

MathType::vec2f Sword_Attack2::OrbitOffset(float degrees) const
{
    const float radians = degrees * 3.1415926535f / 180.0f;
    return {-std::sin(radians) * RADIUS, std::cos(radians) * RADIUS + ORBIT_CENTER_OFFSET_Y};
}
namespace
{
    float BlendRotation(float start, float end, float t)
    {
        return start + std::remainder(end - start, 360.0f) * t;
    }
}
void Sword_Attack2::OnEnter()
{
    mAnimator->Play(L"attack");
    mPhase = Phase::Enter;
    mTime = 0;
    mRotated = 0;
    mAngle = mOwner->GetType() == 0 ? 90.0f : 270.0f;
    mStartRotation = mTransform->rotation();
    if (auto* owner = mOwner->GetOwner())
        mStartOffset = mTransform->position() - owner->transform().position();
}
void Sword_Attack2::OnUpdate(float dt)
{
    if (!mOwner->GetOwner() || !std::isfinite(dt) || dt <= 0) return;
    const auto center = mOwner->GetOwner()->transform().position();
    mTime += dt;
    if (mPhase == Phase::Enter)
    {
        const float t = (std::min)(1.0f, mTime / TRANSITION_TIME);
        const auto target = OrbitOffset(mAngle);
        mTransform->SetPosition(center + mStartOffset + (target - mStartOffset) * t);
        mTransform->SetRotation(BlendRotation(mStartRotation, mAngle, t));
        if (t >= 1) { mPhase = Phase::Orbit; mTime = 0; }
    }
    else if (mPhase == Phase::Orbit)
    {
        const float ramp = (std::min)(mTime, RAMP_TIME);
        const float rotated = (std::min)(360.0f, START_SPEED * ramp
            + (MAX_SPEED - START_SPEED) * ramp * ramp / (2 * RAMP_TIME)
            + MAX_SPEED * (std::max)(0.0f, mTime - RAMP_TIME));
        mAngle += rotated - mRotated;
        mRotated = rotated;
        mTransform->SetPosition(center + OrbitOffset(mAngle));
        mTransform->SetRotation(mAngle);
        if (mRotated >= 360)
        {
            mPhase = Phase::Exit;
            mTime = 0;
            mStartOffset = OrbitOffset(mAngle);
            mStartRotation = mAngle;
        }
    }
    else
    {
        const float t = (std::min)(1.0f, mTime / TRANSITION_TIME);
        const MathType::vec2f target = {mOwner->GetFollowOffsetX(), -Fly_Sword::FOLLOW_OFFSET_Y};
        mTransform->SetPosition(center + mStartOffset + (target - mStartOffset) * t);
        mTransform->SetRotation(BlendRotation(mStartRotation, 180, t));
        if (t >= 1) mFSM->SetNextState(L"idle");
    }
}
void Sword_Attack2::OnQuit() { mOwner->SetAttacking(false); }

