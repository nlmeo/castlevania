#include "Soma_Strike_Air.h"
#include "Soma.h"
#include "GameTool.h"

void Soma_Strike_Air::OnEnter()
{
	mAttributes->dy = 2;

	if (mDiagonal)
	{
		// 斜向：根据进入状态时的朝向叠加水平速度
		mAttributes->dx = (mOwner->transform().localScale().x < 0) ? 4 : 6;
		mAttributes->cursp = mAttributes->sp * 4.0f;
		mAnimator->Play(L"air_strike_diagonal");
	}
	else
	{
		// 竖直向下：无水平速度
		mAttributes->dx = 5;
		mAttributes->cursp = 0;
		mAnimator->Play(L"air_strike_straight");
	}

	// 开启身体攻击判定（飞踢，参考黑豹）
	mOwner->EnableSlideAtkCollider(true);

}

void Soma_Strike_Air::OnUpdate(float delta_time)
{
	// 维持恒定竖直向下速度（每帧重置，抵消重力累加）
	mAttributes->curjp = -mAttributes->jp * 20.0f;

	if (mDiagonal)
	{
		mAttributes->dx = (mOwner->transform().localScale().x < 0) ? 4 : 6;
		mAttributes->cursp = mAttributes->sp * 4.0f;
	}

	// 命中敌人：给予跳跃力并重新解锁二段跳，实现循环
	if (mOwner->ConsumeSlideAttackHit())
	{
		mAttributes->curjp = mAttributes->jp * 10.0f;
		mAttributes->air_jump_remain = 1;
		mFSM->SetNextState(L"fall");
		return;
	}

	// 落地
	if (mOwner->JustLanded() || mOwner->OnGround())
	{
		mFSM->SetNextState(L"land");
		return;
	}
}

void Soma_Strike_Air::OnQuit()
{
	mOwner->EnableSlideAtkCollider(false);
}
