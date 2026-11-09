#include "Soma_Great_Sword_Supper_Attack.h"
#include "GameTool.h"
#include "AttackEffect.h"
#include "Effect_SuperAttack.h"
void Soma_Great_Sword_Supper_Attack::OnEnter()
{
	// 依据朝向决定冲刺方向
	if (mOwner->transform().localScale().x < 0)
		mAttributes->dx = 4;	// 向左
	else
		mAttributes->dx = 6;	// 向右
	mAttributes->dy = 5;
	mAttributes->use_g = true;
	mAttributes->cursp = 8.0f;	// 攻击时给予一定冲刺速度

	mAnimator->Play(L"great_sword_attack_stand");	// 暂用普通站立攻击动画，稍后替换
	mOwner->GetAttackEffect()->Activate(mOwner->transform().localScale().x < 0,true);

	GT::PlaySE(SE_SOMA_SUPER_ATK_GREAT_SWORD);

	switch (rand() % 10)
	{
	case 0:
		GT::PlayVoice(SE_SOMA_HENG);
		break;
	case 1:
		GT::PlayVoice(SE_V_SOMA_YIYA);
		break;
	case 2:
		GT::PlayVoice(SE_V_SOMA_CIYA);
		break;
	default:
		break;
	}
	mAttributes->is_hit_state_change = false;

	bool isFlipx = mOwner->transform().localScale().x > 0;
	MathType::vec2f targetPos = mOwner->transform().position();
	targetPos.x += isFlipx ? 20 : -20;
	Effect_SuperAttack::Spawn(targetPos, false);
}

void Soma_Great_Sword_Supper_Attack::OnUpdate(float delta_time)
{
	if (mAttributes->cursp > 0)
	{
		mAttributes->cursp -= delta_time * 20;
		if(mAttributes->cursp < 0)
			mAttributes->cursp = 0;
	}

	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{
		mFSM->SetNextState(L"idle");
	}

	if (mOwner->CanDash() && GT::KeyJustDown('L'))
	{
		//攻击途中
		mFSM->SetNextState(L"dash_back");
	}
		
}

void Soma_Great_Sword_Supper_Attack::OnQuit()
{
	mAttributes->is_hit_state_change = true;
	mAttributes->is_hit = false;
	mOwner->GetAttackEffect()->Deactivate();
}
