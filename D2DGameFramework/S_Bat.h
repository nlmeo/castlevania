#pragma once
#include "SoulSkill.h"
#include "AttackCollider.h"

class S_Bat : public SoulSkill
{
public:
	virtual void Init() override;
	virtual void Start() override;
	virtual void Update(float deltaTime) override;
	virtual void Awake() override;
	virtual void OnDestroy()  override;
	void OnDebugRender();
	/** @brief 房间进入完成后同步编队，保留开关与体力状态。 */
	void ResetForRoom();
private:
	GamePlay::Animator* a1 = nullptr;
	GamePlay::Animator* a2 = nullptr;
	GamePlay::Animator* a3 = nullptr;
	float rotationA = 0;
	MathType::vec2f curCen;
	bool isOpen = false;

	float atkCD = 0.5f;
	float atkTimer = 0.0f;
	int curAtkObj = 0;
	MathType::vec2f a1_targetPos;
	bool a1_isAtkFinish = true;
	bool a1_isReturning = false;
	MathType::vec2f a2_targetPos;
	bool a2_isAtkFinish = true;
	bool a2_isReturning = false;
	MathType::vec2f a3_targetPos;
	bool a3_isAtkFinish = true;
	bool a3_isReturning = false;

	AttackCollider* mCollider1 = nullptr;
	AttackCollider* mCollider2 = nullptr;
	AttackCollider* mCollider3 = nullptr;

	float mAtkMoveSpeed = 2000.0f;
};
