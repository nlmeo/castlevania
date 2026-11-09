#include "Soma_Skill_Air.h"
#include "Soma.h"
#include "GameTool.h"
#include "AxeBulletP.h"

void Soma_Skill_Air::OnEnter()
{
	mAnimator->Play(L"using_skill_in_air");
	mOwner->SetOnGround(false);
	
	{
		bool flipX = (mOwner->transform().localScale().x < 0);
		AxeBulletP* bullet = GT::CreateGameObject<AxeBulletP>();
		int dirX = flipX ? 4 : 6;
		int dirY = 8;
		bullet->SetLevel(3);
		bullet->SetDirection(dirX, dirY);
		bullet->SetSpeed(3.0f);
		bullet->SetJumpPower(100.0f);
		bullet->SetGravity(3.0f);
		float posX = flipX ? mOwner->transform().position().x - 20 : mOwner->transform().position().x + 20;
		float posY = mOwner->transform().position().y - 50;
		bullet->SetPosition(posX, posY);
	}

	if (rand() % 2)
		GT::PlaySE(SE_SOMA_THROW_1);
	else
		GT::PlaySE(SE_SOMA_THROW_2);

}

void Soma_Skill_Air::OnUpdate(float delta_time)
{

	if (GT::KeyDown('A'))
	{
		mAttributes->dx = 4;
		mAttributes->cursp = mAttributes->sp;
	}
	else if (GT::KeyDown('D'))
	{
		mAttributes->dx = 6;
		mAttributes->cursp = mAttributes->sp;
	}
	else
	{
		mAttributes->dx = 5;
		mAttributes->cursp = 0;
	}

	mAttributes->dy = (mAttributes->curjp > 0) ? 8 : 2;


	if (mAttributes->curjp > 0 && !GT::KeyDown(VK_SPACE))
	{
		mAttributes->curjp = 0;
		mAttributes->dy = 2;
	}
		
	if (mOwner->JustLanded() || mOwner->OnGround())
	{
		mFSM->SetNextState(L"land");
		return;
	}

	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
		mFSM->SetNextState(L"fall");
}

void Soma_Skill_Air::OnQuit()
{
}
