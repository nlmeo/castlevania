#include "Axe_Attack.h"
#include "GameTool.h"
#include "AxeBulletE.h"
#include "./HelpTool/RandomMaker.h"

void Axe_Attack::OnEnter()
{
	mOwner->SetDebugColor({ 1.0f, 0.2f, 0.2f, 0.3f });

	mBulletSpawned = false;
	mSpawnTimer = 0;
	mAttributes->dx = 5;
	mAttributes->dy = 5;
	mAttributes->cursp = 0;
	if (rand() % 2)
	{
		mIsAttack1 = true;
		mAnimator->Play(L"axe_attack_1");
	}
	else
	{
		mIsAttack1 = false;
		mAnimator->Play(L"axe_attack_2");
	}
}

void Axe_Attack::OnUpdate(float delta_time)
{
	mSpawnTimer += delta_time;

	if (!mBulletSpawned && mSpawnTimer >= mSpawnTime)
	{
		mBulletSpawned = true;

		MathType::vec2f spawnPos = mOwner->transform().position();
		bool flipX = mOwner->transform().localScale().x < 0;

		if (mIsAttack1)
		{
			AxeBulletE* bullet = GT::CreateGameObject<AxeBulletE>();
			bullet->Init(
				2,
				flipX ? 6 : 4,
				spawnPos + MathType::vec2f{ 0, +30 },
				10.0f,
				30.0f,
				L"resource\\skills\\p_axe\\frame003.bmp"
			);
			bullet->SetAtkType(2);
		}
		else
		{

			if (rand() % 2)
			{
				AxeBulletE* bullet = GT::CreateGameObject<AxeBulletE>();
				bullet->Init(
					2,
					flipX ? 6 : 4,
					spawnPos + MathType::vec2f{ 0, -80 },
					10.0f,
					30.0f,
					L"resource\\skills\\p_axe\\frame003.bmp"
				);
				bullet->SetAtkType(2);
			}
			else
			{
				AxeBulletE* bullet = GT::CreateGameObject<AxeBulletE>();
				bullet->Init(
					1,
					flipX ? 6 : 4,
					spawnPos + MathType::vec2f{ 0, -50 },
					4.0f,
					50.0f,
					L"resource\\skills\\p_axe\\frame003.bmp"
				);
			}
		}
	}

	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{

		mFSM->SetNextState(L"chase");
	}
}

void Axe_Attack::OnQuit()
{

	mAttributes->cur_atk_cd = mAttributes->atk_cd;
}
