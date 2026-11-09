#include "Soma_Hit_Air.h"
#include "Soma.h"
#include "GameTool.h"

void Soma_Hit_Air::OnEnter()
{
	mOwner->SetDebugColor({ 1.0f, 0.2f, 0.2f, 0.3f });
	mOwner->SetOnGround(false);
	mAttributes->cursp = 0;
	// 设置 dy=2 使 LaterUpdate 能跟踪 fall_height，
	// 这样从 hit_air → fall → land 时能正确判断下落高度
	mAttributes->dy = 2;

	// 击退速度：远离攻击方
	bool isFlipX = mOwner->transform().localScale().x < 0;
	bool isHitBack = mAttributes->is_hit_back;
	float knockDir = (!isFlipX && !isHitBack) || (isFlipX && isHitBack) ? -1.0f : 1.0f;
	mAttributes->cursp = mKnockbackSpeed;
	if (knockDir > 0)
		mAttributes->dx = 6;
	else
		mAttributes->dx = 4;

	mAttributes->curjp = mKnockbackUp;

	if (mAttributes->is_hit_back)
		mAnimator->Play(L"hit_from_behind");
	else
		mAnimator->Play(L"hit_in_the_air_front");
}

void Soma_Hit_Air::OnUpdate(float delta_time)
{
	if (mOwner->JustLanded() || mOwner->OnGround())
	{
		mFSM->SetNextState(L"land");
		return;
	}

	// 受击动画播放完毕后自动切回 fall，避免卡在 hit_air 状态（如掉入深渊）
	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{
		mFSM->SetNextState(L"fall");
	}
}

void Soma_Hit_Air::OnQuit()
{
	mAttributes->is_hit = false;
	mOwner->SetDebugColor({ 0.2f, 0.5f, 1.0f, 0.3f });
}
