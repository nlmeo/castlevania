#include "Fly_Attack.h"
#include "GameTool.h"
#include "Fly_Sword.h"
#include "Player.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>

void Fly_Attack::SetOwner(GamePlay::GameObject* obj) { FlyStates::SetOwner(obj); }

bool Fly_Attack::SwordsReady() const
{
    auto ready = [](Fly_Sword* sword) {
        if (!sword || sword->IsDestroyed() || sword->IsAttacking()) return false;
        auto* fsm = sword->GetComponent<GamePlay::FSM>();
        return fsm && fsm->GetCurrentState() == fsm->GetState(L"idle") && !fsm->GetNextState();
    };
    return ready(mSwordLeft) && ready(mSwordRight);
}
bool Fly_Attack::IssueAttack(int type)
{
    if (!MainPlayer || MainPlayer->IsDestroyed() || !SwordsReady()) return false;
    const auto target = MainPlayer->transform().position();
    mSwordLeft->Attack(target, type);
    mSwordRight->Attack(target, type);
    return mSwordLeft->IsAttacking() && mSwordRight->IsAttacking();
}
bool Fly_Attack::PlayerInDetectRange() const
{
    if (!MainPlayer || MainPlayer->IsDestroyed() || !MainPlayer->Active()) return false;
    const auto delta = MainPlayer->transform().position() - mOwner->transform().position();
    return std::abs(delta.x) <= Fly::GetDetectWidth() * 0.5f &&
        std::abs(delta.y) <= Fly::GetDetectHeight() * 0.5f;
}
void Fly_Attack::OnEnter()
{
    mOwner->SetDebugColor({1, 0.2f, 0.2f, 0.3f});
    mAttributes->cursp = 0;
    mSwordLeft = mOwner->GetSwordLeft();
    mSwordRight = mOwner->GetSwordRight();
    mPhase = Phase::Spin;
    mPhaseTime = 0;
    mVolleyCount = 0;
    mAttackIssued = false;
}
void Fly_Attack::MoveBody(MathType::vec2f offset)
{
    auto position = mOwner->transform().position() + offset;
    if (MainCamera)
    {
        const auto bounds = MainCamera->CameraBound();
        const float margin = Fly::BOUND_MARGIN;
        if (bounds.Width() > margin * 2)
            position.x = std::clamp(position.x, bounds.left + margin, bounds.right - margin);
        if (bounds.Height() > margin * 2)
            position.y = std::clamp(position.y, bounds.top + margin, bounds.bottom - margin);
    }
    mOwner->transform().SetPosition(mOwner->ClampFlightPosition(position));
}
void Fly_Attack::BeginRetreat()
{
    mPhase = Phase::Retreat;
    mPhaseTime = 0;
    const auto position = mOwner->transform().position();
    float direction = MainPlayer && !MainPlayer->IsDestroyed()
        ? position.x - MainPlayer->transform().position().x : 1.0f;
    if (MainCamera)
    {
        const auto bounds = MainCamera->CameraBound();
        if (direction < 0 && position.x - bounds.left < Fly::BOUND_MARGIN + 100) direction = 1;
        else if (direction >= 0 && bounds.right - position.x < Fly::BOUND_MARGIN + 100) direction = -1;
    }
    mRetreatDirection = {direction < 0 ? -1.0f : 1.0f, 0};
    mRetreatStart = position;
    mRetreatTarget = mOwner->ClampFlightPosition(position + mRetreatDirection * RETREAT_DISTANCE);
    // Pick once per retreat so the smoothed path has a stable, elevated destination.
    if (MainCamera)
    {
        const float middleY = MainCamera->transform()->position().y;
        const float screenTop = middleY - MainCamera->SizeY() * 0.5f;
        const float topY = (std::min)(middleY, (std::max)(screenTop,
            MainCamera->CameraBound().top) + Fly::BOUND_MARGIN);
        const float bottomY = (std::max)(topY, (std::min)(middleY, position.y - RETREAT_MIN_RISE));
        const float random = (static_cast<float>(std::rand()) + 0.5f) / (static_cast<float>(RAND_MAX) + 1.0f);
        mRetreatTarget.y = topY + (bottomY - topY) * random;
    }
    else mRetreatTarget.y -= RETREAT_MIN_RISE;
    mRetreatTarget = mOwner->ClampFlightPosition(mRetreatTarget);
    if (MainCamera)
    {
        const auto bounds = MainCamera->CameraBound();
        if (bounds.Width() > Fly::BOUND_MARGIN * 2)
            mRetreatTarget.x = std::clamp(mRetreatTarget.x, bounds.left + Fly::BOUND_MARGIN,
                bounds.right - Fly::BOUND_MARGIN);
    }
}
void Fly_Attack::OnUpdate(float dt)
{
    if (!std::isfinite(dt) || dt <= 0 || mAttributes->is_dead) return;
    if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED) mAnimator->RePlay();
    if (!mSwordLeft || !mSwordRight) { mFSM->SetNextState(L"idle"); return; }
    if (mPhase == Phase::Spin)
    {
        if (!mAttackIssued) { mAttackIssued = IssueAttack(2); return; }
        if (MainPlayer && !MainPlayer->IsDestroyed())
        {
            auto target = MainPlayer->transform().position();
            target.y -= 60;
            const auto delta = target - mOwner->transform().position();
            const float distance = delta.magnitude();
            if (distance > APPROACH_DISTANCE)
                MoveBody(delta.normalized() * (std::min)(APPROACH_SPEED * dt, distance - APPROACH_DISTANCE));
        }
        if (SwordsReady()) { mPhase = Phase::Volley; mAttackIssued = false; }
    }
    else if (mPhase == Phase::Volley)
    {
        if (!mAttackIssued) { mAttackIssued = IssueAttack(1); return; }
        if (SwordsReady())
        {
            ++mVolleyCount;
            mAttackIssued = false;
            if (mVolleyCount >= 2) BeginRetreat();
        }
    }
    else if (mPhase == Phase::Retreat)
    {
        const auto before = mOwner->transform().position();
        const float t = (std::min)(1.0f, (mPhaseTime + dt) / RETREAT_DURATION);
        const float eased = t * t * (3.0f - 2.0f * t);
        const auto target = mRetreatStart + (mRetreatTarget - mRetreatStart) * eased;
        MoveBody(target - before);
        const auto displacement = mOwner->transform().position() - before;
        for (auto* sword : {mSwordLeft, mSwordRight})
            sword->transform().translate(displacement);
        mPhaseTime += dt;
        if (mPhaseTime >= RETREAT_DURATION)
        {
            mPhase = PlayerInDetectRange() ? Phase::Recover : Phase::Chase;
            mPhaseTime = 0;
            mIdleAnchor = mOwner->transform().position();
            mIdleTime = 0;
            mVolleyCount = 0;
            mAttackIssued = false;
        }
    }
    else if (mPhase == Phase::Recover || mPhase == Phase::Chase)
    {
        // Cooldown runs alongside locomotion, including switches between idle and chase.
        mPhaseTime = (std::min)(RECOVERY_DURATION, mPhaseTime + dt);
        const bool inRange = PlayerInDetectRange();
        if (inRange && mPhaseTime >= RECOVERY_DURATION)
        {
            mPhase = Phase::Spin;
            mAttackIssued = false;
            return;
        }
        if (inRange || !MainPlayer || MainPlayer->IsDestroyed() || !MainPlayer->Active())
        {
            mPhase = Phase::Recover;
            mIdleTime += dt;
            // Gentle upward bob keeps the pivot above the screen midpoint.
            auto target = mIdleAnchor;
            target.y -= 4.0f * (1.0f - std::cos(mIdleTime * 5.026548f));
            MoveBody(target - mOwner->transform().position());
            return;
        }
        mPhase = Phase::Chase;
        auto target = MainPlayer->transform().position();
        target.y -= 60;
        target = mOwner->ClampFlightPosition(target);
        const auto delta = target - mOwner->transform().position();
        const float distance = delta.magnitude();
        if (distance > 0.001f)
            MoveBody(delta.normalized() * (std::min)(CHASE_SPEED * dt, distance));
        mIdleAnchor = mOwner->transform().position();
        mIdleTime = 0;
    }
}
void Fly_Attack::OnQuit() {}

