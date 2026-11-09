#include "S_Axe.h"
#include "Player.h"
#include "Soma.h"
#include "GameTool.h"
void S_Axe::Init()
{
}

void S_Axe::Start()
{
}

void S_Axe::Update(float deltaTime)
{
	if (MainPlayer == nullptr) { return; }

	Soma* pSoma = dynamic_cast<Soma*>(MainPlayer);

	if (GT::KeyJustDown('I'))
	{
		if (!IsFsmActive())
		{
			// Only allow in IDLE / WALK / DASH_BACK / JUMP / FALL states
			auto fsm = MainPlayer->GetComponent<GamePlay::FSM>();
			if (fsm)
			{
				auto cur = fsm->GetCurrentState();
				bool canUse = cur == fsm->GetState(L"idle")
					|| cur == fsm->GetState(L"walk")
					|| cur == fsm->GetState(L"dash_back")
					|| cur == fsm->GetState(L"jump")
					|| cur == fsm->GetState(L"fall");
				if (!canUse) { return; }
			}

			if (pSoma->OnGround())
			{
				mFsmStateKey = L"using_axe_bullet_on_ground";
			}
			else
			{
				mFsmStateKey = L"using_axe_bullet_in_air";
			}

			EnterFsmState();
		}

	}
}
