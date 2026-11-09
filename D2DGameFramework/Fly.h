#pragma once
#ifndef __GAME_CONTENT_OBJ_FLY_H__
#define __GAME_CONTENT_OBJ_FLY_H__

#include "Enemy.h"
#include "Attributes.h"
#include "GamePlay/CollisionMatrix.h"
#include "GamePlay/ChildObject.h"

class Soma;
class Fly_Sword;

class Fly : public Enemy
{
public:
	Fly();
	void OnDebugRender();
	Attributes& GetAttributes() { return m_attributes; }

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update(float deltaTime) override;
	virtual void OnDestroy() override;
	virtual void Hit(float atk,int atkType);
	Soma* FindPlayerInRange(float rangeX, float rangeY);
	bool IsWithdraw() const { return m_is_withdraw; }
	void SetWithdraw(bool b) { m_is_withdraw = b; }
	int GetAtkCd() const { return m_attributes.atk_cd; }
	int GetCurAtkCd() const { return m_attributes.cur_atk_cd; }
	void SetCurAtkCd(int v) { m_attributes.cur_atk_cd = v; }
	GamePlay::GameObject* GetCloak() const { return mCloak; }
	Fly_Sword* GetSwordLeft() const { return mSwordLeft; }
	Fly_Sword* GetSwordRight() const { return mSwordRight; }
	void SetAttackActive(bool active) { if (mAttackCollider) mAttackCollider->SetActive(active); }
	static float GetDetectWidth() { return 800.0f; }
	static float GetDetectHeight() { return 600.0f; }
	void SetDebugColor(const ColorType::COLORF& c) { mDebugColor = c; }
	void SetBodyColliderPos(const MathType::vec2f& pos) {
		if (mBodyCollider) mBodyCollider->transform().SetPosition(pos);
	}

	static constexpr float ARMOR_FIXED_Y = 200.0f; 
	static constexpr float BOUND_MARGIN = 100.0f;   
	static constexpr float MAX_Y_FLOAT = 15.0f;     
	static constexpr int MOVE_CD = 120;             
	static constexpr int ATTACK_INTERVAL = 180;     

	void move();  
	MathType::vec2f ClampFlightPosition(MathType::vec2f position) const;
private:
	Attributes m_attributes;
	GamePlay::ChildObject* mBodyCollider = nullptr;
	GamePlay::ChildObject* mAttackCollider = nullptr;
	GamePlay::ChildObject* mCloak = nullptr;
	Fly_Sword* mSwordLeft = nullptr;
	Fly_Sword* mSwordRight = nullptr;
	float m_target_x = 0;
	float m_target_y = 0;
	int m_cur_move_cd = 0;
	int m_attack_timer = 0;
	bool m_is_withdraw = false;
	ColorType::COLORF mDebugColor = { 0.2f, 0.5f, 1.0f, 0.3f };
	void onCollision(GamePlay::Collider* c);
	void onAttackCollision(GamePlay::Collider* c);
	void initAnim();
	void initFSM();
};
#endif
