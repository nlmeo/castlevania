#include "Fly.h"
#include "./D2DCore/TextureManager.h"
#include "GameTool.h"
#include "GameCore/Application.h"
#include "Fly_Sword.h"
#include "Fly_Idle.h"
#include "Fly_Attack.h"
#include "Fly_Dead.h"
#include "Soma.h"
#include "GameUI/FloatingNumberPool.h"
#include <algorithm>

MathType::vec2f Fly::ClampFlightPosition(MathType::vec2f position) const
{
	if (MainCamera)
	{
		// Screen Y grows downward: the camera center is the lowest allowed pivot height.
		const float middleY = MainCamera->transform()->position().y;
		const float topY = (std::min)(MainCamera->CameraBound().top + BOUND_MARGIN, middleY);
		position.y = std::clamp(position.y, topY, middleY);
	}
	return position;
}
Fly::Fly()
{
	initAnim();
	initFSM();

	m_attributes.cur_hp = 160;
	m_attributes.hp = 160;
	m_attributes.atk = 1;
	m_attributes.sp = 0.2f;
	m_attributes.cursp = 0;
	m_attributes.curjp = 0.1f;
	m_attributes.atk_cd = 240;
	m_attributes.cur_atk_cd = m_attributes.atk_cd;
	m_attributes.dx = 5;
	m_attributes.dy = 5;

	m_attributes.is_hit = false;
	m_attributes.is_invulnurable = false;
	m_attributes.invulnurable_time = 0.0f;//20.0f;
	m_attributes.is_dead = false;
	m_attributes.is_corpse = false;

	m_is_withdraw = false;
	m_attack_timer = ATTACK_INTERVAL;


}

void Fly::OnDebugRender()
{
	if (!mBodyCollider) return;

	float posx = transform().position().x;
	float posy = transform().position().y;

	float hw = GetDetectWidth() / 2;
	float hh = GetDetectHeight() / 2;
	Debug.DebugDrawLine(posx - hw, posy - hh, posx + hw, posy - hh, ColorType::COLORF::Red());
	Debug.DebugDrawLine(posx + hw, posy - hh, posx + hw, posy + hh, ColorType::COLORF::Red());
	Debug.DebugDrawLine(posx + hw, posy + hh, posx - hw, posy + hh, ColorType::COLORF::Red());
	Debug.DebugDrawLine(posx - hw, posy + hh, posx - hw, posy - hh, ColorType::COLORF::Red());

	GamePlay::BoxCollider* bc = mBodyCollider->GetComponent<GamePlay::BoxCollider>();
	if (bc != nullptr)
	{
		Debug.DebugDrawLine(
			posx - bc->Width() / 2,
			posy - bc->Height() / 2,
			posx + bc->Width() / 2,
			posy - bc->Height() / 2,
			ColorType::COLORF::Red()
		);
		Debug.DebugDrawLine(
			posx - bc->Width() / 2,
			posy - bc->Height() / 2,
			posx - bc->Width() / 2,
			posy + bc->Height() / 2,
			ColorType::COLORF::Red()
		);
		Debug.DebugDrawLine(
			posx + bc->Width() / 2,
			posy - bc->Height() / 2,
			posx + bc->Width() / 2,
			posy + bc->Height() / 2,
			ColorType::COLORF::Red()
		);
		Debug.DebugDrawLine(
			posx - bc->Width() / 2,
			posy + bc->Height() / 2,
			posx + bc->Width() / 2,
			posy + bc->Height() / 2,
			ColorType::COLORF::Red()
		);
	}



	if (mSwordLeft != nullptr)
		mSwordLeft->OnDebugRender();

	if (mSwordRight != nullptr)
		mSwordRight->OnDebugRender();
}

void Fly::Awake()
{
	mBodyCollider = GT::CreateGameObject<GamePlay::ChildObject>();
	mBodyCollider->SetOwner(this);
	mBodyCollider->layer() = GamePlay::Enemy;
	GamePlay::BoxCollider* c = mBodyCollider->AddComponent<GamePlay::BoxCollider>();
	c->SetColliderSize(50, 150);
	c->SetCollitionCallBack([&](GamePlay::Collider* c) { return onCollision(c); });

	mAttackCollider = GT::CreateGameObject<GamePlay::ChildObject>();
	mAttackCollider->SetOwner(this);
	mAttackCollider->layer() = GamePlay::EnemyAtk;
	GamePlay::BoxCollider* ac = mAttackCollider->AddComponent<GamePlay::BoxCollider>();
	ac->SetColliderSize(50, 150);
	ac->SetCollitionCallBack([&](GamePlay::Collider* c) { return onAttackCollision(c); });

	int order = GameObjectMgr.GameObjectCount();

	mCloak = GT::CreateGameObject<GamePlay::ChildObject>();
	mCloak->SetOwner(this);
	mCloak->layer() = GamePlay::GameLayer::Layer_3;
	GamePlay::Animator* cloakAnim = mCloak->AddComponent<GamePlay::Animator>();
	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\enemies\\p_fly\\frame000.bmp",{0.5f, 0.5f}, 8, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame001.bmp",{0.5f, 0.5f}, 8, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame002.bmp",{0.5f, 0.5f}, 8, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame003.bmp",{0.5f, 0.5f}, 8, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame004.bmp",{0.5f, 0.5f}, 8, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame005.bmp",{0.5f, 0.5f}, 8, 0x00FF00 },
			});
		cloakAnim->AddClip(L"fly_cloak", clip);
	}
	cloakAnim->SetOrderInLayer(++order);
	cloakAnim->Play(L"fly_cloak");

	mSwordLeft = GT::CreateGameObject<Fly_Sword>();
	mSwordLeft->SetOwner(this);
	mSwordLeft->SetType(0);
	mSwordLeft->layer() = GamePlay::GameLayer::Layer_5;
	//mSwordLeft->GetComponent<GamePlay::Animator>()->SetOrderInLayer(++order);

	mSwordRight = GT::CreateGameObject<Fly_Sword>();
	mSwordRight->SetOwner(this);
	mSwordRight->SetType(1);
	mSwordRight->layer() = GamePlay::GameLayer::Layer_5;
	//mSwordRight->GetComponent<GamePlay::Animator>()->SetOrderInLayer(++order);

	auto flash = AddComponent<GamePlay::HitFlash>();
	flash->SetFlashColor(0.50f, 0.02f, 0.02f);
	flash->SetFlashTiming(0.06f, 0.35f, 0.85f);
}

void Fly::Start()
{
	if (!mBodyCollider) return;
	mBodyCollider->transform().SetPosition(transform().position());
	mBodyCollider->_ComponentUpdate(0);

	if (mAttackCollider)
	{
		mAttackCollider->transform().SetPosition(transform().position());
		mAttackCollider->_ComponentUpdate(0);
	}

	if (mCloak)
	{
		mCloak->transform().SetPosition(transform().position());
		mCloak->_ComponentUpdate(0);
	}
}

void Fly::Update(float deltaTime)
{
	if (m_attributes.is_corpse)
		return;
	transform().SetPosition(ClampFlightPosition(transform().position()));

	if (m_attributes.is_invulnurable)
	{
		m_attributes.cur_invulnurable_time -= 1;
		if (m_attributes.cur_invulnurable_time <= 0)
		{
			m_attributes.is_invulnurable = false;
			if (!mBodyCollider->Active())
			{
				mBodyCollider->SetActive(true);
			}
		}
	}

	if (m_attributes.is_dead)
	{
		GetComponent<GamePlay::FSM>()->SetNextState(L"dead");
		_ComponentUpdate(deltaTime);
		return;
	}

	mBodyCollider->transform().SetPosition(transform().position());
	mBodyCollider->_ComponentUpdate(0);

	if (mAttackCollider)
	{
		mAttackCollider->transform().SetPosition(transform().position());
		mAttackCollider->_ComponentUpdate(0);
	}

	if (mCloak)
	{
		mCloak->transform().SetPosition(transform().position());
		mCloak->_ComponentUpdate(deltaTime);

		GamePlay::Animator* cloakAni = mCloak->GetComponent<GamePlay::Animator>();
		if (cloakAni)
		{
			if (cloakAni->GetState() == GamePlay::AS::AS_STOPPED)
				cloakAni->RePlay();
		}
	}

}

void Fly::OnDestroy()
{
	if (mBodyCollider)
	{
		mBodyCollider->Destroy();
		mBodyCollider = nullptr;
	}
	if (mAttackCollider)
	{
		mAttackCollider->Destroy();
		mAttackCollider = nullptr;
	}
	if (mCloak)
	{
		mCloak->Destroy();
		mCloak = nullptr;
	}
	if (mSwordLeft)
	{
		mSwordLeft->Destroy();
		mSwordLeft = nullptr;
	}
	if (mSwordRight)
	{
		mSwordRight->Destroy();
		mSwordRight = nullptr;
	}
}

void Fly::Hit(float atk,int atkType)
{
	if (m_attributes.is_invulnurable || m_attributes.is_dead) return;

	if (atkType == 3)
	{
		GT::PlaySE(SE_HIT_GREAT_SWORD2);
	}
	else
	{
		GT::PlaySE(SE_ENEMY_HIT_SOLID);
	}
	GetComponent<GamePlay::HitFlash>()->TriggerHit();

	//int dmg = rand() % 100;
	MathType::vec2f curPos = transform().position();
	curPos.x += rand() % 50 - 25;
	curPos.y += rand() % 50 - 25;
	FLOATINGNUMS.Spawn(atk, curPos.x, curPos.y, 50.0f, 1.5f, true);


	m_attributes.cur_hp -= atk;
	if (m_attributes.cur_hp <= 0)
	{
		m_attributes.is_dead = true;
		m_attributes.cursp = 0;
		if (mBodyCollider != nullptr && mBodyCollider->Active())
			mBodyCollider->SetActive(false);
		if (mAttackCollider)
			mAttackCollider->SetActive(false);
		return;
	}

	m_attributes.is_invulnurable = true;
	m_attributes.cur_invulnurable_time = m_attributes.invulnurable_time; 
	
}

Soma* Fly::FindPlayerInRange(float rangeX, float rangeY)
{
	MathType::vec2f center = transform().position();
	float left = center.x - rangeX / 2;
	float right = center.x + rangeX / 2;
	float top = center.y - rangeY / 2;
	float bottom = center.y + rangeY / 2;

	for (auto* collider : GameObjectMgr.ColliderObjects())
	{
		GamePlay::GameObject* obj = collider->gameobject();
		if (!obj->Active() || obj == this) continue;
		if (obj->layer() != GamePlay::Player) continue;

		MathType::vec2f objPos = obj->transform().position();
		if (objPos.x >= left && objPos.x <= right && objPos.y >= top && objPos.y <= bottom)
			return static_cast<Soma*>(obj);
	}
	return nullptr;
}

void Fly::onCollision(GamePlay::Collider* c)
{
	
}

void Fly::onAttackCollision(GamePlay::Collider* c)
{
}

void Fly::move()
{
	if (m_cur_move_cd == 0)
	{
		float secure_left = BOUND_MARGIN;
		float rand_width = (float)App.GetWidth() - secure_left;
		m_target_x = secure_left + (float)(rand() % (int)rand_width);
		m_target_y = ARMOR_FIXED_Y + (float)(rand() % (int)(2 * MAX_Y_FLOAT)) - MAX_Y_FLOAT;
		m_cur_move_cd = MOVE_CD;
	}

	float cur_x = (transform().position().x > m_target_x)
		? transform().position().x - m_attributes.cursp
		: transform().position().x + m_attributes.cursp;

	float cur_y = (transform().position().y > m_target_y)
		? transform().position().y - m_attributes.curjp
		: transform().position().y + m_attributes.curjp;

	transform().SetPosition({ cur_x, cur_y });
	m_cur_move_cd--;
}

void Fly::initAnim()
{
	auto* pAnim = AddComponent<GamePlay::Animator>();

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\enemies\\p_fly\\frame006.bmp",{0.5f, 0.5f}, 20, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame007.bmp",{0.5f, 0.5f}, 14, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame008.bmp",{0.5f, 0.5f}, 14, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame009.bmp",{0.5f, 0.5f}, 20, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame00A.bmp",{0.5f, 0.5f}, 6,  0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame00B.bmp",{0.5f, 0.5f}, 14, 0x00FF00 },
			});
		pAnim->AddClip(L"fly_idle", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\enemies\\p_fly\\frame00C.bmp",{0.5f, 0.5f}, 2, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame00D.bmp",{0.5f, 0.5f}, 2, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame00E.bmp",{0.5f, 0.5f}, 2, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame00F.bmp",{0.5f, 0.5f}, 2, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame010.bmp",{0.5f, 0.5f}, 2, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame011.bmp",{0.5f, 0.5f}, 2, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame012.bmp",{0.5f, 0.5f}, 2, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame013.bmp",{0.5f, 0.5f}, 2, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame014.bmp",{0.5f, 0.5f}, 2, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame015.bmp",{0.5f, 0.5f}, 2, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame016.bmp",{0.5f, 0.5f}, 2, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame017.bmp",{0.5f, 0.5f}, 2, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame018.bmp",{0.5f, 0.5f}, 2, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame019.bmp",{0.5f, 0.5f}, 2, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame01A.bmp",{0.5f, 0.5f}, 2, 0x00FF00 },
			});
		pAnim->AddClip(L"fly_attack_intro", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\enemies\\p_fly\\frame01B.bmp",{0.5f, 0.5f}, 5, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame01C.bmp",{0.5f, 0.5f}, 5, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame01B.bmp",{0.5f, 0.5f}, 5, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame01C.bmp",{0.5f, 0.5f}, 5, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame01B.bmp",{0.5f, 0.5f}, 5, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame01C.bmp",{0.5f, 0.5f}, 5, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame01B.bmp",{0.5f, 0.5f}, 5, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame01C.bmp",{0.5f, 0.5f}, 5, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame01B.bmp",{0.5f, 0.5f}, 5, 0x00FF00 },
				{ L"resource\\enemies\\p_fly\\frame01C.bmp",{0.5f, 0.5f}, 5, 0x00FF00 },
			});
		pAnim->AddClip(L"fly_dead", clip);
	}
}

void Fly::initFSM()
{
	GamePlay::FSM* fsm = AddComponent<GamePlay::FSM>();

	{
		Fly_Idle* state = new Fly_Idle();
		state->SetOwner(this);
		fsm->RegisterState(L"idle", state);
	}
	{
		Fly_Attack* state = new Fly_Attack();
		state->SetOwner(this);
		fsm->RegisterState(L"attack", state);
	}
	{
		Fly_Dead* state = new Fly_Dead();
		state->SetOwner(this);
		fsm->RegisterState(L"dead", state);
	}

	fsm->InitState(L"idle");
}
