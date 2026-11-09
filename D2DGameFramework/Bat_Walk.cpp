#include "Bat_Walk.h"
#include "GameTool.h"
#include "Soma.h"

void Bat_Walk::OnEnter()
{
	mOwner->SetDebugColor({ 1.0f, 0.2f, 0.2f, 0.3f });
	mAttributes->cursp = mAttributes->sp;
	mAnimator->Play(L"bat_fly");

	// Ĭ������
	mDir = { mAttributes->cursp, 0.0f };
	mOwner->transform().SetScale({ -1.0f, 1.0f });

	Soma* player = mOwner->FindPlayerInRange(Bat::GetDetectWidth(), Bat::GetDetectHeight());
	if (player)
	{
		MathType::vec2f playerPos = player->transform().position();
		MathType::vec2f selfPos = mOwner->transform().position();
		mDir = { playerPos.x - selfPos.x, playerPos.y - selfPos.y };
		float mag = sqrtf(mDir.x * mDir.x + mDir.y * mDir.y);
		if (mag > 0)
		{
			mDir.x = (mDir.x / mag) * mAttributes->cursp;
			mDir.y = (mDir.y / mag) * mAttributes->cursp;
		}
		mOwner->transform().SetScale({ (mDir.x > 0) ? -1.0f : 1.0f, 1.0f });
	}
}

void Bat_Walk::OnUpdate(float delta_time)
{
	// ����ҷ������
	MathType::vec2f delta = { mDir.x * delta_time * 60.0f, mDir.y * delta_time * 60.0f };
	mOwner->transform().translate(delta);

	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{
		mAnimator->RePlay();
	}
}

void Bat_Walk::OnQuit()
{
}
