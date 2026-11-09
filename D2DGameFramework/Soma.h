#pragma once
#ifndef __GAME_CONTENT_OBJ_SOMA_H__
#define __GAME_CONTENT_OBJ_SOMA_H__

#include "Player.h"
#include "Attributes.h"
#include "./TypeDefine/Color.h"
#include "GamePlay/ChildObject.h"
#include "HelpTool/Timer.h"

namespace HelpTool { class INIConfig; }

class Soma : public Player
{
public:
	Soma();
	void OnDebugRender();
	Attributes& GetAttributes() { return m_attributes; }
	void SavePos();
	// 存档序列化：写入/回读属性与坐标
	void SaveTo(HelpTool::INIConfig& ini);
	void LoadFrom(HelpTool::INIConfig& ini);
	virtual void Awake() override;
	virtual void OnEnable()  override;
	virtual void Start()  override;
	virtual void Update(float deltaTime)  override;
	virtual void LaterUpdate(float deltaTime) override;
	virtual void OnDisable()  override;
	virtual void OnDestroy()  override;
	void SetOnSlop(bool b) { mOnSlop = b; }
	void SetOnGround(bool b) { mOnGround = b; if (!b) m_justLanded = false; }
	void SetOnCollideTop(bool b) { mOnCollideTop = b; }
	bool OnSlop() { return mOnSlop; }
	bool OnGround() { return mOnGround; }
	bool OnCollideTop() { return mOnCollideTop; }
	void UpdatePhysics(float deltaTime);
	void EvaluateTransitions();
	void OnRoomEntered();
	void CheckGroundBelow();
	bool IsTurnDir();
	bool canJumpBelow() { return mOnPlatform && !mOnObstacle && !mOnSlop && !mOnEdgePlatform; }
	bool OnPlatformGround() { return m_currentPlatformCollider != nullptr && m_ignoredCollider == nullptr; }
	void DropFromPlatform() { m_ignoredCollider = m_currentPlatformCollider; SetOnGround(false); }
	void InitIgnoredCollider() { m_ignoredCollider = nullptr; }
	bool JustLanded() { return m_justLanded; }
	void Hit(int atk);
	bool IsInvulnerable() const { return m_attributes.is_invulnurable || m_attributes.is_dead; }
	bool IsDead() const { return m_attributes.is_dead; }
	void SetDebugColor(const ColorType::COLORF& c) { mDebugColor = c; }
	class AttackEffect* GetAttackEffect() { return mAttackEffect; }
	void EnableSlideAtkCollider(bool enable);
	bool ConsumeSlideAttackHit();
	bool CanDash() { return mCanDash; }
	void StartDashCooldown();
	bool TryConsumeStamina(float amount);
	bool TryConsumeSuperAttack();
	bool TryConsumeRedSoul();
	void UpdateStamina(float deltaTime);
private:
	bool mOnPlatform = false;
	bool mOnEdgePlatform = false;
	bool mOnObstacle = false;
	bool mOnSlop = false;
	bool mOnGround = true;
	bool mOnCollideTop = false;
	bool m_wasOnGround = false;
	bool m_wasOnSlop = false;
	bool m_wasOnCollideTop = false;
	bool m_justLanded = false;
	GamePlay::Collider* m_ignoredCollider = nullptr;
	GamePlay::Collider* m_currentPlatformCollider = nullptr;
	Attributes m_attributes{};
	bool mCanDash = true;
	HelpTool::CTimer mDashCdTimer;
	HelpTool::CTimer mDeathTimer;   // 死亡后延迟切换场景
	bool mDeathTriggered = false;   // 是否已触发死亡切换（防止重复）
	GamePlay::ChildObject* mBodyCollider;
	class AttackEffect* mAttackEffect = nullptr;
	class AttackCollider* mSlideAtkCollider = nullptr;
	MathType::RectF mLastPos;
	ColorType::COLORF mDebugColor = { 0.2f, 0.5f, 1.0f, 0.3f };
	void onCollision(GamePlay::Collider* c);
	void initAnim();
	void initFSM();
};
#endif
