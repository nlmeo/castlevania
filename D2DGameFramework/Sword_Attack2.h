#pragma once
#include "SwordStates.h"
// Both swords orbit 50 pixels above the owner's pivot, using the same timing profile.
class Sword_Attack2 : public SwordStates
{
public:
    void OnEnter() override;
    void OnUpdate(float dt = 0) override;
    void OnQuit() override;
private:
    enum class Phase { Enter, Orbit, Exit };
    Phase mPhase = Phase::Enter;
    MathType::vec2f mStartOffset;
    float mStartRotation = 180;
    float mAngle = 0;
    float mTime = 0;
    float mRotated = 0;
    static constexpr float RADIUS = 100;
    static constexpr float ORBIT_CENTER_OFFSET_Y = -70.0f;
    static constexpr float TRANSITION_TIME = 0.4f;
    static constexpr float START_SPEED = 40;
    static constexpr float MAX_SPEED = 200;
    static constexpr float RAMP_TIME = 1.5f;
    MathType::vec2f OrbitOffset(float angle) const;
};

