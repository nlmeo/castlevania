#include "BreakableObject.h"
#include "GameTool.h"
#include "EffectsPool.h"
#include "Drop.h"
#include "./D2DCore/TextureManager.h"

BreakableObject::BreakableObject()
{
	mName = "BreakableObject";
	mLayer = GamePlay::GameLayer::Layer_4; // 近景渲染层
	mBreakEffectKey = EFFECT_BLUE_FIRE_01;      // 默认破坏碎片特效（可替换）

	initAnim();
}

void BreakableObject::initAnim()
{
	mAnimator = AddComponent<GamePlay::Animator>();

	// 默认占位动画（单帧），后续可通过 SetAnimation / GetAnimator 替换
	GamePlay::AnimationClip clip;
	clip.AddFrames({
		{ L"resource\\ui\\Icons\\008.bmp", {0.5f, 0.5f}, 1, 0x00FF00 },
	});
	mAnimator->AddClip(mIdleClipKey.c_str(), clip);
}

void BreakableObject::Awake()
{
	// 创建独立碰撞器对象作为子对象，供玩家攻击碰撞检测命中
	mBodyCollider = GT::CreateGameObject<GamePlay::ChildObject>();
	mBodyCollider->SetOwner(this);
	mBodyCollider->layer() = GamePlay::Enemy;
	GamePlay::BoxCollider* c = mBodyCollider->AddComponent<GamePlay::BoxCollider>();
	c->SetColliderSize(40, 80);
}

void BreakableObject::Start()
{
	if (mBodyCollider)
	{
		mBodyCollider->transform().SetPosition(transform().position());
		mBodyCollider->_ComponentUpdate(0);
	}
	if (mAnimator)
	{
		mAnimator->Play(mIdleClipKey.c_str());
	}
}

void BreakableObject::Hit(float atk, int atkType)
{
	Enemy::Hit(atk, atkType); // 播放受击音效

	if (--mHitPoints > 0)
		return;

	Break();
}

void BreakableObject::Break()
{
	MathType::vec2f pos = transform().position();

	// 1. 生成掉落道具
	Drop::Spawn(mDropItemId, mDropItemCount, pos);

	// 2. 生成 part 碎片特效（可通过 SetBreakEffectKey 替换）
	if (!mBreakEffectKey.empty())
	{
		for (int i = 0; i < 3; ++i)
		{
			MathType::vec2f p = pos;
			p.x += (float)(rand() % 40 - 20);
			p.y += (float)(rand() % 40 - 20);
			Effect::Spawn(mBreakEffectKey, p, (rand() % 2) == 0);
		}
	}

	// 3. 通知外部（如场景）物体即将销毁，便于其置空指针，避免悬挂指针
	if (mOnBreakCallback)
		mOnBreakCallback();

	// 4. 销毁自身
	Destroy();
}

void BreakableObject::OnDestroy()
{
	if (mBodyCollider)
	{
		mBodyCollider->Destroy();
		mBodyCollider = nullptr;
	}
}

void BreakableObject::SetDropItem(int itemId, int count)
{
	mDropItemId = itemId;
	mDropItemCount = count > 0 ? count : 1;
}

void BreakableObject::SetAnimation(const wchar_t* clipKey, const GamePlay::AnimationClip& clip)
{
	if (!mAnimator) return;
	mIdleClipKey = clipKey;
	mAnimator->AddClip(clipKey, clip);
	mAnimator->Play(clipKey);
}
