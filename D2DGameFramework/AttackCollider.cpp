#include "AttackCollider.h"
#include "Enemy.h"
#include "GameTool.h"
#include "./GamePlay/ChildObject.h"
#include "./GamePlay/GameObject.h"
#include "./GamePlay/CollisionMatrix.h"
#include "EffectsPool.h"
AttackCollider::AttackCollider(GamePlay::GameObject* pobj)
	: BoxCollider(pobj)
{
	mType = GamePlay::COMTYPE::ComType_AttackCollider;
	SetColliderSize(200, 50);
}

AttackCollider::~AttackCollider()
{
}

AttackCollider* AttackCollider::Create(GamePlay::GameObject* owner)
{
	auto* host = owner == nullptr ? GT::CreateGameObject<GamePlay::GameObject>() : owner;
	host->layer() = GamePlay::PlayerAtk; // PlayerAtk
	host->SetActive(false);
	return host->AddComponent<AttackCollider>();
}

// ---- 模式配置 ----

void AttackCollider::SetOneHit()
{
	mHitMode = HitMode::OneHit;
}

void AttackCollider::SetTimeInHit(float hitInterval, int maxHits)
{
	mHitMode = HitMode::TimeInHit;
	mHitInterval = hitInterval;
	mMaxHitsPerEnemy = maxHits;
}

// ---- 参数配置 ----

void AttackCollider::SetAttack(float atk, int atkType)
{
	mAtk = atk;
	mAtkType = atkType;
}

void AttackCollider::SetOffset(const MathType::vec2f& offset)
{
	mOffset = offset;
}

void AttackCollider::SetTarget(GamePlay::GameObject& target)
{
	mTarget = &target;
}

void AttackCollider::SetEffectKey(std::wstring& effectKey)
{
	mEffectKey = effectKey;
}

// ---- 生命周期 ----

void AttackCollider::syncPosition()
{
	if (!mTarget) return;
	float sx = mTarget->transform().localScale().x;
	bool flip = sx < 0;
	float x = flip
		? mTarget->transform().position().x - mOffset.x
		: mTarget->transform().position().x + mOffset.x;
	float y = mTarget->transform().position().y + mOffset.y;
	gameobject()->transform().SetPosition({ x, y });
	UpdateCollider();
}

void AttackCollider::Activate()
{
	mActive = true;
	mHitFlag = false;
	mElapsed = 0.0f;
	mHitEnemies.clear();
	mHitRecords.clear();
	mHitObjects.clear();
	syncPosition();
}

void AttackCollider::Deactivate()
{
	mActive = false;
	mHitFlag = false;
	mHitEnemies.clear();
	mHitRecords.clear();
	mHitObjects.clear();
}

void AttackCollider::Update(float deltaTime)
{
	// 未激活时也跟随目标，让调试框与实际目标位置保持一致。
	syncPosition();
	if (!mActive) return;

	// 绕过全局碰撞系统 sweep-line 的限制，直接做 AABB 重叠检测
	auto& myRect = CollitionAABBRect();
	for (auto* collider : GameObjectMgr.ColliderObjects())
	{
		if (!collider || collider->gameobject()->IsDestroyed()) continue;
		if (collider == this) continue;
		unsigned int targetBit = ToBit(collider->gameobject()->layer());
		if ((GamePlay::Mask_PlayerAtk & targetBit) == 0) continue;
		if (!collider->gameobject()->Active()) continue;

		auto& otherRect = collider->CollitionAABBRect();
		if (myRect.left < otherRect.right && myRect.right > otherRect.left &&
			myRect.top < otherRect.bottom && myRect.bottom > otherRect.top)
		{
			onAttackHit(collider);
		}
	}

	mElapsed += deltaTime;
}

// ---- 碰撞回调 ----

void AttackCollider::onAttackHit(GamePlay::Collider* c)
{
	static int i = 0;
	i++;
	

	GamePlay::GameObject* other = c->gameobject();

	// 处理 EnemyAtk 弹幕（如 AxeBulletE），通过其自身的碰撞回调处理
	if (other->layer() == GamePlay::GameLayer::Layer_14)
	{
		if (mHitObjects.count(other)) return;
		mHitObjects.insert(other);
		c->OnCollition(this);
		return;
	}

	// 处理 Enemy（Layer_13）：ChildObject → Enemy 链
	if (other->layer() != GamePlay::GameLayer::Layer_13)
		return;

	GamePlay::ChildObject* cp = dynamic_cast<GamePlay::ChildObject*>(other);
	if (!cp) return;

	Enemy* enemy = cp->GetOwner<Enemy>();
	if (!enemy) return;

	
	MathType::vec2f effectPos = other->transform().position();
	float dx = rand() % 50 - 25;
	float dy = rand() % 50 - 25;
	effectPos.x += dx;
	effectPos.y += dy;


	if (mHitMode == HitMode::OneHit)
	{
		if (mHitEnemies.count(enemy)) return;

		enemy->Hit(mAtk, mAtkType);
		mHitFlag = true;
		mHitEnemies.insert(enemy);


		if (mEffectKey == EFFECT_CLAIMH_SOLAIS)
			Effect::Spawn(EFFECT_CLAIMH_SOLAIS, effectPos, false, 0.7f);
		else if (mEffectKey == EFFECT_RED_HIT)
			Effect::Spawn(EFFECT_RED_HIT, effectPos, false, 0.7f);

	}
	else // TimeInHit
	{
		AttackCollider::HitRecord& record = mHitRecords[enemy];

		if (record.count >= mMaxHitsPerEnemy)
			return;

		if (mElapsed - record.lastHitTime < mHitInterval)
			return;

		record.lastHitTime = mElapsed;
		record.count++;
		enemy->Hit(mAtk, mAtkType);
		mHitFlag = true;


		if (mEffectKey == EFFECT_CLAIMH_SOLAIS)
			Effect::Spawn(EFFECT_CLAIMH_SOLAIS, effectPos, false, 0.7f);
		else if (mEffectKey == EFFECT_RED_HIT)
			Effect::Spawn(EFFECT_RED_HIT, effectPos, false, 0.7f);


	}
}
