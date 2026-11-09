#include "SoulSkill.h"
#include "./GamePlay/FiniteStateMachine.h"
#include "Player.h"
#include "Soma.h"
#include <cmath>

bool SoulSkill::TrySustain(float deltaTime)
{
	auto* player = dynamic_cast<Soma*>(MainPlayer);
	if (!player || !player->mBulueSoul || player->mBulueSoul->p_skill != this ||
		!std::isfinite(deltaTime) || deltaTime <= 0 || mStaminaExhausted) return false;
	if (!player->TryConsumeStamina(player->mBulueSoul->mp_cost * deltaTime))
	{
		mStaminaExhausted = true;
		return false;
	}
	return true;
}
void SoulSkill::EnterFsmState()
{
	if (mFsmStateKey.empty() || MainPlayer == nullptr) return;
	auto fsm = MainPlayer->GetComponent<GamePlay::FSM>();
	if (!fsm) return;
	if (fsm->GetCurrentState() != fsm->GetState(mFsmStateKey.c_str()))
		fsm->SetNextState(mFsmStateKey.c_str());
}
void SoulSkill::ExitFsmState()
{
	if (mFsmStateKey.empty() || MainPlayer == nullptr) return;
	auto fsm = MainPlayer->GetComponent<GamePlay::FSM>();
	if (!fsm) return;
	auto* player = dynamic_cast<Soma*>(MainPlayer);
	fsm->SetNextState(player && !player->OnGround() ? L"fall" : L"idle");
}


bool SoulSkill::IsFsmActive()
{
	if (mFsmStateKey.empty() || MainPlayer == nullptr) return false;
	auto fsm = MainPlayer->GetComponent<GamePlay::FSM>();
	if (!fsm) return false;
	auto cur = fsm->GetCurrentState();
	auto target = fsm->GetState(mFsmStateKey.c_str());
	return cur == target || fsm->GetNextState() == target;
}
