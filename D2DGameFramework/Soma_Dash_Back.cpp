#include "Soma_Dash_Back.h"
#include "GameTool.h"
#include "EffectsPool.h"
void Soma_Dash_Back::OnEnter()
{
	// 从攻击状态触发后退时，进入冷却
	GamePlay::LPSN cur = mFSM->GetCurrentState();
	if (cur == mFSM->GetState(L"attack_stand") ||
		cur == mFSM->GetState(L"great_sword_attack_air") ||
		cur == mFSM->GetState(L"great_sword_attack_stand") ||
		cur == mFSM->GetState(L"great_sword_supper_attack") ||
		cur == mFSM->GetState(L"punch_attack_stand") ||
		cur == mFSM->GetState(L"sword_supper_attack"))
		//||
		//cur == mFSM->GetState(L"axe_bullet_ground"))
	{
		mOwner->StartDashCooldown();
	}

	mAttributes->cursp = mInitSpeed;
	mAttributes->dx = (mAttributes->m_last_scale_x > 0) ? 4 : 6;
	mAttributes->dy = 5;

	if (mFSM->GetCurrentState() == mFSM->GetState(L"slide_flat"))
		mAnimator->Play(L"crouch_loop");
	else
		mAnimator->Play(L"dash_back");
	GT::PlaySE(SE_SOMA_DASH_BACK);

	mTimePass = 0.0f;
	mCurEffectCount = 0;

}

void Soma_Dash_Back::OnUpdate(float delta_time)
{

	mAttributes->cursp *= mDeceleration;

	if (mCurEffectCount < mMaxEffectCount)
	{

		if (mTimePass > mTimeGap)
		{
			MathType::vec2f playerPos = mOwner->transform().position();
			playerPos.y += 50;
			switch (HelpTool::RandomInt(0, 1))
			{
			case 0:
				Effect::Spawn(EFFECT_DUST_01, playerPos, false, 0.4f);
				break;
			case 1:
				Effect::Spawn(EFFECT_DUST_02, playerPos, false, 0.4f);
				break;
			default:
				break;
			}
			mTimePass = 0;
			mCurEffectCount += 1;
		}
		mTimePass += delta_time;

	}

	if (!mOwner->OnGround())
	{
		mFSM->SetNextState(L"fall");
		return;
	}


	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{

		if (mAnimator->GetCurrentAniKey() == L"crouch_loop")
			mFSM->SetNextState(L"crouch");
		else
			mFSM->SetNextState(L"idle");
		return;
	}

	if (GT::KeyJustDown('J'))
	{
		auto* curWeapon = mBpk->GetEquippedWeapon();
		if (curWeapon == nullptr)
			mAnimator->GetCurrentAniKey() == L"crouch_loop" ? mFSM->SetNextState(L"punch_attack_crouch") : mFSM->SetNextState(L"punch_attack_stand");
		else if (curWeapon->id == 4)
			mAnimator->GetCurrentAniKey() == L"crouch_loop" ? mFSM->SetNextState(L"great_sword_attack_crouch") : mFSM->SetNextState(L"great_sword_attack_stand");
		else
			mAnimator->GetCurrentAniKey() == L"crouch_loop" ? mFSM->SetNextState(L"attack_crouch") : mFSM->SetNextState(L"attack_stand");

	}
	else if (GT::KeyJustDown(32))
		mFSM->SetNextState(L"jump");
}

void Soma_Dash_Back::OnQuit()
{
}
