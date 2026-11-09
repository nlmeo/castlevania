#pragma once
#include "./GamePlay/GameObject.h"
#include "Attributes.h"
#include "GamePlay/CollisionMatrix.h"
#include "./TypeDefine/Color.h"

class Fly_Sword : public GamePlay::GameObject
{
public:
	Fly_Sword();
	void SetOwner(GamePlay::GameObject* owner) { mOwner = owner; }
	void SetType(int t) { type = t; }
	int GetType() const { return type; }
	void OnDebugRender();
	Attributes& GetAttributes() { return mAttributes; }
	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update(float deltaTime) override;
	virtual void OnDestroy() override;
	void Attack(MathType::vec2f target,int atkType = 1); //1,投掷 2，旋转
	const MathType::vec2f GetAtkDir() const { return mAtkDir; }
	static float GetDetectWidth() { return 800.0f; }
	static float GetDetectHeight() { return 600.0f; }
	static constexpr float FOLLOW_OFFSET_X = 128.0f;   // X 轴偏移（左/右）
	static constexpr float FOLLOW_OFFSET_Y = 120.0f;   // Y 轴偏移（上方）
	static constexpr float FOLLOW_SPEED = 5.0f;        // 跟随平滑速度
	float GetFollowOffsetX() const { return type == 0 ? -FOLLOW_OFFSET_X : FOLLOW_OFFSET_X; }
	void SetDebugColor(const ColorType::COLORF& c) { mDebugColor = c; }
	bool IsAttacking() const { return mIsAttacking; }
	void SetAttacking(bool attacking) { mIsAttacking = attacking; }
	void SetAttackActive(bool active) { if (mAttackCollider) mAttackCollider->SetActive(active); }
	GameObject* GetOwner() const { return mOwner; }
	int GetAtkType() const { return mAtkType; }
private:
	bool mIsInit = false;
	bool mIsAttacking = false;
	Attributes mAttributes;
	MathType::vec2f mAtkDir;
	int type = 0;
	int mAtkType = 1; //1,投掷 2，旋转
	GameObject* mOwner = nullptr;
	GamePlay::Animator* mAni = nullptr;
	GamePlay::GameObject* mAttackCollider = nullptr;
	ColorType::COLORF mDebugColor = { 0.2f, 0.5f, 1.0f, 0.3f };
	void onCollision(GamePlay::Collider* c);
	void initAnim();
	void initFSM();
};