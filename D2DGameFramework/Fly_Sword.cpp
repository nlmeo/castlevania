#include "Fly_Sword.h"
#include "GameTool.h"
#include "Sword_Idle.h"
#include "Sword_Aim.h"
#include "Sword_Attack.h"
#include "Sword_WithDraw.h"
#include "Sword_Attack2.h"
#include "./TypeDefine/Universal.h"
Fly_Sword::Fly_Sword()
{
	SetDestroyOnSceneExit();
	mIsAttacking = false;
}

void Fly_Sword::OnDebugRender()
{
	if (!mAttackCollider) return;

	float posx = transform().position().x;
	float posy = transform().position().y;

	float hw = GetDetectWidth() / 2;
	float hh = GetDetectHeight() / 2;
	Debug.DebugDrawLine(posx - hw, posy - hh, posx + hw, posy - hh, ColorType::COLORF::Red());
	Debug.DebugDrawLine(posx + hw, posy - hh, posx + hw, posy + hh, ColorType::COLORF::Red());
	Debug.DebugDrawLine(posx + hw, posy + hh, posx - hw, posy + hh, ColorType::COLORF::Red());
	Debug.DebugDrawLine(posx - hw, posy + hh, posx - hw, posy - hh, ColorType::COLORF::Red());

	GamePlay::RotationBoxCollider* bc = mAttackCollider->GetComponent<GamePlay::RotationBoxCollider>();
	if (bc != nullptr)
	{
		//获取旋转矩形的四个世界顶点，按顺序连线绘制，贴合RotationBox的旋转角度
		MathType::vec2f corners[4];
		bc->GetWorldCorners(corners);
		for (int i = 0; i < 4; ++i)
		{
			int j = (i + 1) % 4;
			Debug.DebugDrawLine(
				corners[i].x, corners[i].y,
				corners[j].x, corners[j].y,
				ColorType::COLORF::Red()
			);
		}
	}
}

void Fly_Sword::Awake()
{
	mAttackCollider = GT::CreateGameObject<GamePlay::GameObject>();
	mAttackCollider->layer() = GamePlay::EnemyAtk;
	GamePlay::RotationBoxCollider* ac = mAttackCollider->AddComponent<GamePlay::RotationBoxCollider>();
	ac->SetColliderSize(50, 200);
	ac->SetCollitionCallBack([&](GamePlay::Collider* c) { return onCollision(c); });
	initAnim();
	initFSM();
	mAttackCollider->transform().SetRotation(180.0f);
	
	
	if (mOwner)
	{
		MathType::vec2f ownerPos = mOwner->transform().position();
		ownerPos.x += GetFollowOffsetX();
		ownerPos.y -= FOLLOW_OFFSET_Y;
		transform().SetPosition(ownerPos);
		mAttackCollider->transform().SetPosition(ownerPos);
	}
	mIsInit = true;
}

void Fly_Sword::Start()
{
}

void Fly_Sword::Update(float deltaTime)
{
	if (!mIsInit) return;
	if (mAttackCollider && mAttackCollider->Active())
	{
		MathType::vec2f curPos = transform().position();
		float curRot = transform().rotation();
		float curRotDegree = D2R(curRot);
		
		//curPos.y -= 50;
		//curPos.y += 50.0f * sin(curRot + 90);
		//curPos.x -= curDir.x * 50;
		//curPos.y -= curDir.y * 50;
		curPos.y += 50.0f * cos(curRotDegree);
		curPos.x -= 50.0f * sin(curRotDegree);
		
		mAttackCollider->transform().SetPosition(curPos);

		//同步攻击碰撞器的旋转，使其随剑一起旋转
		mAttackCollider->transform().SetRotation(curRot);

		//if(mOwner)
		//	mAttackCollider->transform().SetRotation(0);
		mAttackCollider->_ComponentUpdate(0);
	}
}

void Fly_Sword::OnDestroy()
{
	if (mAttackCollider) { mAttackCollider->Destroy(); mAttackCollider = nullptr; }
}

void Fly_Sword::Attack(MathType::vec2f target, int atkType)
{
	if (!mIsAttacking && mIsInit)
	{
		MathType::vec2f curPos = transform().position();
		MathType::vec2f dir = (target - curPos).normalized();
		mAtkDir = dir.normalized();
		mIsAttacking = true;
		mAtkType = atkType;
	}
}

void Fly_Sword::onCollision(GamePlay::Collider* c)
{
}

void Fly_Sword::initAnim()
{
	mAni = AddComponent<GamePlay::Animator>();

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\enemies\\p_fly\\frame00C.bmp",{0.5f, 0.33f}, 3, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame00D.bmp",{0.5f, 0.33f}, 3, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame00E.bmp",{0.5f, 0.33f}, 3, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame00F.bmp",{0.5f, 0.33f}, 3, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame010.bmp",{0.5f, 0.33f}, 3, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame011.bmp",{0.5f, 0.33f}, 3, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame012.bmp",{0.5f, 0.33f}, 3, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame013.bmp",{0.5f, 0.33f}, 3, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame014.bmp",{0.5f, 0.33f}, 3, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame015.bmp",{0.5f, 0.33f}, 3, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame016.bmp",{0.5f, 0.33f}, 3, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame017.bmp",{0.5f, 0.33f}, 3, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame018.bmp",{0.5f, 0.33f}, 3, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame019.bmp",{0.5f, 0.33f}, 3, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame01A.bmp",{0.5f, 0.33f}, 3, 0x00FF00 },
			});
		mAni->AddClip(L"attack", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\enemies\\p_fly\\frame00C.bmp",{0.5f, 0.33f}, 3, 0x00FF00 },
			});
		mAni->AddClip(L"idle", clip);
	}
}

void Fly_Sword::initFSM()
{
	GamePlay::FSM* fsm = AddComponent<GamePlay::FSM>();

	{
		Sword_Idle* state = new Sword_Idle();
		state->SetOwner(this);
		fsm->RegisterState(L"idle", state);
	}
	{
		Sword_Aim* state = new Sword_Aim();
		state->SetOwner(this);
		fsm->RegisterState(L"aim", state);
	}
	{
		Sword_Attack* state = new Sword_Attack();
		state->SetOwner(this);
		fsm->RegisterState(L"attack", state);
	}
	{
		Sword_WithDraw* state = new Sword_WithDraw();
		state->SetOwner(this);
		fsm->RegisterState(L"withdraw", state);
	}
	{
		Sword_Attack2* state = new Sword_Attack2();
		state->SetOwner(this);
		fsm->RegisterState(L"attack2", state);
	}
	

	fsm->InitState(L"idle");
}
