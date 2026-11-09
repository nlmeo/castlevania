#pragma once
#include "./GamePlay/Collider2d.h"
#include "./TypeDefine/Vector2.h"
#include <unordered_map>
#include <unordered_set>

class Enemy;

// 通用攻击碰撞器，继承 BoxCollider，支持两种命中模式
// OneHit:   Activate() 后对同一敌人仅命中一次即失效
// TimeInHit:Activate() 期间持续检测，可配置间隔和最大次数
// 通过 SetTarget + SetOffset 自动跟随目标位置
class AttackCollider : public GamePlay::BoxCollider
{
public:
	enum class HitMode { OneHit, TimeInHit };

	AttackCollider(GamePlay::GameObject* pobj);
	~AttackCollider();

	// 便捷工厂：自动创建宿主 GameObject (layer=PlayerAtk)
	static AttackCollider* Create(GamePlay::GameObject* owner = nullptr);

	// ---- 模式配置 ----
	void SetOneHit();
	void SetTimeInHit(float hitInterval, int maxHits);

	// ---- 参数配置 ----
	void SetAttack(float atk, int atkType = 1);
	void SetOffset(const MathType::vec2f& offset);
	void SetTarget(GamePlay::GameObject& target);
	void SetEffectKey(std::wstring& effectKey);
	// ---- 生命周期 ----
	void Activate();
	void Deactivate();
	void Update(float deltaTime);
	bool IsActive() const { return mActive; }

	// 是否发生过命中（一次性读取，供技能判断命中，如飞踢命中后反弹）
	bool ConsumeHit() { bool h = mHitFlag; mHitFlag = false; return h; }

private:
	struct HitRecord { float lastHitTime = 0.0f; int count = 0; };

	void onAttackHit(GamePlay::Collider* c);
	void syncPosition();

	GamePlay::GameObject* mTarget = nullptr;
	MathType::vec2f        mOffset = { 0, 0 };

	HitMode mHitMode = HitMode::OneHit;
	bool    mActive  = false;
	bool    mHitFlag = false;  // 本轮激活期间是否发生过命中

	// OneHit：记录本轮已命中的敌人，防止同帧重复命中
	std::unordered_set<Enemy*> mHitEnemies;

	// 记录已命中的 EnemyAtk 对象（如 AxeBulletE 弹幕），防止重复命中
	std::unordered_set<GamePlay::GameObject*> mHitObjects;

	// TimeInHit
	float mHitInterval    = 0.0f;
	int   mMaxHitsPerEnemy = 0;
	float mElapsed        = 0.0f;
	std::unordered_map<Enemy*, HitRecord> mHitRecords;

	// 伤害
	float mAtk     = 1;
	int mAtkType = 1;
	std::wstring mEffectKey = L"";
};
