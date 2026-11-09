#pragma once
#include "Enemy.h"
#include "GamePlay/ChildObject.h"
#include "GamePlay/Animator.h"
#include <string>
#include <functional>

// 场景内可破坏物体（蜡烛、雕像、箱子等）：
// - 持有 Animator，播放自身待机动画（动画 clip 可替换）
// - 受到玩家攻击后：生成掉落道具 -> 生成 part 碎片特效 -> 销毁自身
// - 预留接口：掉落道具 / part 特效 / 自身动画 均可替换
class BreakableObject : public Enemy
{
public:
	BreakableObject();
	virtual ~BreakableObject() override = default;

	virtual void Awake() override;
	virtual void Start() override;
	virtual void OnDestroy() override;
	virtual void Hit(float atk, int atkType) override;

	// ---- 预留接口 ----
	// 破坏后掉落的道具（itemId 物品ID，count 数量）
	void SetDropItem(int itemId, int count = 1);
	// 需要被攻击几次才破坏（默认 1 次）
	void SetHitPoints(int hitPoints) { mHitPoints = hitPoints; }
	// 破坏时生成的 part 碎片特效 key（可替换）
	void SetBreakEffectKey(const std::wstring& effectKey) { mBreakEffectKey = effectKey; }
	// 替换自身 animator 播放的动画 clip（clipKey 为播放用 key）
	void SetAnimation(const wchar_t* clipKey, const GamePlay::AnimationClip& clip);
	// 直接访问 animator（供外部自由替换动画）
	GamePlay::Animator* GetAnimator() { return mAnimator; }
	// 破坏时的回调（用于外部在物体销毁后置空自身指针，避免悬挂指针）
	void SetOnBreakCallback(std::function<void()> cb) { mOnBreakCallback = std::move(cb); }

private:
	void initAnim();
	void Break();

	GamePlay::ChildObject* mBodyCollider = nullptr;
	GamePlay::Animator*    mAnimator = nullptr;

	int          mHitPoints = 1;                   // 破坏所需命中次数
	int          mDropItemId = 1;                  // 掉落道具ID（默认小红瓶）
	int          mDropItemCount = 1;               // 掉落数量
	std::wstring mIdleClipKey = L"breakable_idle"; // 自身动画 clip key
	std::wstring mBreakEffectKey = L"";            // 破坏时生成的 part 特效 key

	std::function<void()> mOnBreakCallback;        // 破坏时回调
};
