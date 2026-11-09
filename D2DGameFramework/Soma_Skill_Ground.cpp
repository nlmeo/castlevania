#include "Soma_Skill_Ground.h"
#include "Soma.h"
#include "GameTool.h"
#include "AxeBulletP.h"

void Soma_Skill_Ground::OnEnter()
{
	
	mAttributes->cursp = 0;
	mAttributes->dx = 5;
	mAttributes->dy = 5;
	mAnimator->Play(L"using_skill_on_ground");


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

void Soma_Skill_Ground::OnUpdate(float delta_time)
{
	
	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{
		mFSM->SetNextState(L"idle");
		return;
	}

	if (GT::KeyJustDown('L'))
		mFSM->SetNextState(L"dash_back");

}

void Soma_Skill_Ground::OnQuit()
{
}
