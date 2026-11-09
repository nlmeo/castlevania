#include "Zombi_Dead.h"
#include "GameTool.h"
#include "ZombiPart.h"
#include "Soma.h"
#include "BloodParticle.h"
void Zombi_Dead::OnEnter()
{
	mAttributes->cursp = 0;
	MathType::vec2f spawnPos = mOwner->transform().position();

	const float attackDir = mOwner->DeathScatterDirection();
	

	static const wchar_t* corpseFrames[] = {
		L"resource\\enemies\\p_zombi\\frame011.bmp",
		L"resource\\enemies\\p_zombi\\frame012.bmp",
		L"resource\\enemies\\p_zombi\\frame013.bmp",
		L"resource\\enemies\\p_zombi\\frame014.bmp",
		L"resource\\enemies\\p_zombi\\frame015.bmp",
		L"resource\\enemies\\p_zombi\\frame016.bmp",
	};

	for (int i = 0; i < 6; ++i)
	{
		ZombiPart* part = GT::CreateGameObject<ZombiPart>();
		part->Init(corpseFrames[i], spawnPos, attackDir);
	}

	spawnPos.y -= 30.0f;
	BloodParticle::Emit(spawnPos, { attackDir, 2 }, 50);


	mAttributes->is_corpse = true;
	mOwner->SetActive(false);
}

void Zombi_Dead::OnUpdate(float delta_time)
{
}

void Zombi_Dead::OnQuit()
{
}
