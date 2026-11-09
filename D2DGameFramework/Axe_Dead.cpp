#include "Axe_Dead.h"
#include "GameTool.h"
#include "AxePart.h"
#include "ZombiPart.h"
void Axe_Dead::OnEnter()
{
	mAttributes->cursp = 0;
	mAnimator->Play(L"axe_dead");
}

void Axe_Dead::OnUpdate(float delta_time)
{
	mFSM->SetNextState(L"dead");

	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{
		

		MathType::vec2f spawnPos = mOwner->transform().position();

		const float attackDir = mOwner->DeathScatterDirection();


		static const wchar_t* corpseFrames[] = {
			L"resource\\enemies\\p_axe00\\frame028.bmp",
			L"resource\\enemies\\p_axe00\\frame029.bmp",
			L"resource\\enemies\\p_axe00\\frame030.bmp",
			L"resource\\enemies\\p_axe00\\frame031.bmp",
			L"resource\\enemies\\p_axe00\\frame032.bmp",
			L"resource\\enemies\\p_axe00\\frame033.bmp",
			L"resource\\enemies\\p_axe00\\frame034.bmp",
			L"resource\\enemies\\p_axe00\\frame035.bmp",
			L"resource\\enemies\\p_axe00\\frame036.bmp",
			L"resource\\enemies\\p_axe00\\frame037.bmp",
			L"resource\\enemies\\p_axe00\\frame038.bmp",
			L"resource\\enemies\\p_axe00\\frame039.bmp",
			L"resource\\enemies\\p_axe00\\frame040.bmp",
			L"resource\\enemies\\p_axe00\\frame041.bmp",
			L"resource\\enemies\\p_axe00\\frame042.bmp",
			L"resource\\enemies\\p_axe00\\frame043.bmp",
		};

		for (int i = 0; i < 16; ++i)
		{
			AxePart* part = GT::CreateGameObject<AxePart>();
			part->Init(corpseFrames[i], spawnPos, attackDir);
		}

		mAttributes->is_corpse = true;
		mOwner->SetActive(false);
	}
}

void Axe_Dead::OnQuit()
{
}
