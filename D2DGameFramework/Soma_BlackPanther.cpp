#include "Soma_BlackPanther.h"
#include "Soma.h"
#include "GameTool.h"

void Soma_BlackPanther::OnEnter()
{
	mAttributes->is_hit_state_change = false; // 开启霸体
	mAnimator->Play(L"use_black_panther");
	mAttributes->dx = mOwner->transform().localScale().x < 0 ? 4 : 6;
	mAttributes->cursp = mAttributes->sp * 3.0f;

	mOriginalAttack = mAttributes->atk;
	mAttributes->atk = 10;
	mOwner->EnableSlideAtkCollider(true);
}

void Soma_BlackPanther::OnUpdate(float delta_time)
{

	if (mOwner->OnGround() && GT::KeyDown(VK_SPACE))
		mAttributes->curjp = mAttributes->jp * 5;


	if (!GT::KeyDown('K'))
	{
		BackToNormalState();
		mOwner->EnableSlideAtkCollider(false);
	}
		
}

void Soma_BlackPanther::OnQuit()
{
	mAttributes->cursp = 0;
	mAttributes->is_hit_state_change = true;
	mAttributes->is_hit = false;

	mAttributes->atk = mOriginalAttack;
 	mOwner->EnableSlideAtkCollider(false);
}
