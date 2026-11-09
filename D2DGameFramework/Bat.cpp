#include "Bat.h"
#include "./D2DCore/TextureManager.h"
#include "GameTool.h"
#include "Bat_Idle.h"
#include "Bat_Walk.h"
#include "Bat_Dead.h"
#include "Soma.h"
#include "GameUI/FloatingNumberPool.h"

Bat::Bat()
{
	initAnim();
	initFSM();

	m_attributes.hp = 1;
	m_attributes.atk = 3;
	m_attributes.sp = 3.6f;
	m_attributes.cursp = 0;
	m_attributes.atk_cd = 60;
	m_attributes.cur_atk_cd = m_attributes.atk_cd;
	m_attributes.dx = 4;
	m_attributes.dy = 5;

	m_attributes.is_hit = false;
	m_attributes.is_invulnurable = false;
	m_attributes.invulnurable_time = 0.0f;
	m_attributes.is_dead = false;
	m_attributes.is_corpse = false;
}

void Bat::OnDebugRender()
{
	if (!mBodyCollider) return;

	float posx = transform().position().x;
	float posy = transform().position().y;

	// 索敌矩形
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
}

void Bat::Awake()
{
	mBodyCollider = GT::CreateGameObject<GamePlay::ChildObject>();
	mBodyCollider->SetOwner(this);
	mBodyCollider->layer() = GamePlay::Enemy;
	GamePlay::BoxCollider* c = mBodyCollider->AddComponent<GamePlay::BoxCollider>();
	c->SetColliderSize(50, 150);
	c->SetCollitionCallBack([&](GamePlay::Collider* c) { return onCollision(c); });
}

void Bat::Start()
{
	if (!mBodyCollider) return;
	mBodyCollider->transform().SetPosition(transform().position());
	mBodyCollider->_ComponentUpdate(0);
}

void Bat::Update(float deltaTime)
{
	if (m_attributes.is_corpse)
		return;

	if (m_attributes.is_dead)
	{
		GetComponent<GamePlay::FSM>()->SetNextState(L"dead");
		_ComponentUpdate(deltaTime);
		return;
	}

	if (m_attributes.is_invulnurable)
	{
		m_attributes.cur_invulnurable_time -= deltaTime;
		if (m_attributes.cur_invulnurable_time <= 0)
		{
			m_attributes.is_invulnurable = false;
		}
	}

	mBodyCollider->transform().SetPosition(transform().position());
	mBodyCollider->_ComponentUpdate(0);
}

void Bat::OnDestroy()
{
	if (mBodyCollider)
	{
		mBodyCollider->Destroy();
		mBodyCollider = nullptr;
	}
}

void Bat::Hit(float atk, int atkType)
{
	if (m_attributes.is_invulnurable || m_attributes.is_dead) return;


	Enemy::Hit(atk, atkType);
	//int dmg = rand() % 100;
	MathType::vec2f curPos = transform().position();
	FLOATINGNUMS.Spawn(atk, curPos.x, curPos.y, 50.0f, 1.5f, true);

	m_attributes.hp -= atk;
	if (m_attributes.hp <= 0)
	{
		m_attributes.is_dead = true;
		m_attributes.cursp = 0;
		if(mBodyCollider != nullptr && mBodyCollider->Active())
			mBodyCollider->SetActive(false);
		return;
	}

	m_attributes.is_invulnurable = true;
	m_attributes.cur_invulnurable_time = m_attributes.invulnurable_time;
}

Soma* Bat::FindPlayerInRange(float rangeX, float rangeY)
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

void Bat::onCollision(GamePlay::Collider* c)
{
	
}

void Bat::initAnim()
{
	auto* pAnim = AddComponent<GamePlay::Animator>();
	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\enemies\\p_armc02\\frame00A.bmp",{0.5f, 0.5f}, 8, 0x00FF00 },
				{ L"resource\\enemies\\p_armc02\\frame00B.bmp",{0.5f, 0.5f},10, 0x00FF00 },
			});
		pAnim->AddClip(L"bat_idle", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\enemies\\p_armc02\\frame000.bmp",{0.5f, 0.5f}, 4, 0x00FF00 },
				{ L"resource\\enemies\\p_armc02\\frame001.bmp",{0.5f, 0.5f}, 1, 0x00FF00 },
				{ L"resource\\enemies\\p_armc02\\frame002.bmp",{0.5f, 0.5f}, 1, 0x00FF00 },
				{ L"resource\\enemies\\p_armc02\\frame009.bmp",{0.5f, 0.5f}, 1, 0x00FF00 },
				{ L"resource\\enemies\\p_armc02\\frame003.bmp",{0.5f, 0.5f}, 1, 0x00FF00 },
				{ L"resource\\enemies\\p_armc02\\frame004.bmp",{0.5f, 0.5f}, 1, 0x00FF00 },
				{ L"resource\\enemies\\p_armc02\\frame005.bmp",{0.5f, 0.5f}, 4, 0x00FF00 },
				{ L"resource\\enemies\\p_armc02\\frame006.bmp",{0.5f, 0.5f}, 2, 0x00FF00 },
				{ L"resource\\enemies\\p_armc02\\frame007.bmp",{0.5f, 0.5f}, 2, 0x00FF00 },
				{ L"resource\\enemies\\p_armc02\\frame008.bmp",{0.5f, 0.5f}, 2, 0x00FF00 },
				{ L"resource\\enemies\\p_armc02\\frame009.bmp",{0.5f, 0.5f}, 1, 0x00FF00 },
				{ L"resource\\enemies\\p_armc02\\frame002.bmp",{0.5f, 0.5f}, 2, 0x00FF00 },
				{ L"resource\\enemies\\p_armc02\\frame001.bmp",{0.5f, 0.5f}, 2, 0x00FF00 },
			});
		pAnim->AddClip(L"bat_fly", clip);
	}
}

void Bat::initFSM()
{
	GamePlay::FSM* fsm = AddComponent<GamePlay::FSM>();

	{
		Bat_Idle* state = new Bat_Idle();
		state->SetOwner(this);
		fsm->RegisterState(L"idle", state);
	}
	{
		Bat_Walk* state = new Bat_Walk();
		state->SetOwner(this);
		fsm->RegisterState(L"walk", state);
	}
	{
		Bat_Dead* state = new Bat_Dead();
		state->SetOwner(this);
		fsm->RegisterState(L"dead", state);
	}

	fsm->InitState(L"idle");
}
