#pragma once
#include "FlyStates.h"
class Fly_Attack : public FlyStates
{
public:
    void SetOwner(GamePlay::GameObject* obj) override;
    void OnEnter() override;
    void OnUpdate(float dt = 0) override;
    void OnQuit() override;
private:
    enum class Phase { Spin, Volley, Retreat, Recover, Chase };
    Phase mPhase = Phase::Spin;
    class Fly_Sword* mSwordLeft = nullptr;
    class Fly_Sword* mSwordRight = nullptr;
    int mVolleyCount = 0;
    float mPhaseTime = 0;
    bool mAttackIssued = false;
    MathType::vec2f mRetreatDirection = {1, 0};
    MathType::vec2f mRetreatStart;
    MathType::vec2f mRetreatTarget;
    MathType::vec2f mIdleAnchor;
    float mIdleTime = 0;
    bool SwordsReady() const;
    bool IssueAttack(int type);
    bool PlayerInDetectRange() const;
    void BeginRetreat();
    void MoveBody(MathType::vec2f offset);
    static constexpr float APPROACH_SPEED = 110.0f;
    static constexpr float CHASE_SPEED = 65.0f;
    static constexpr float APPROACH_DISTANCE = 100.0f;
    static constexpr float RETREAT_DISTANCE = 360.0f;
    static constexpr float RETREAT_MIN_RISE = 40.0f;
    static constexpr float RETREAT_DURATION = 1.6f;
    static constexpr float RECOVERY_DURATION = 2.0f;
};

