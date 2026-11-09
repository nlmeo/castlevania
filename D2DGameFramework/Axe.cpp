#include "Axe.h"
#include "./D2DCore/TextureManager.h"
#include "./HelpTool/RandomMaker.h"
#include "GameTool.h"
#include "GameCore/GameLoop.h"
#include "Axe_Idle.h"
#include "Axe_Patrol.h"
#include "Axe_Alert.h"
#include "Axe_Dead.h"
#include "Axe_Chase.h"
#include "Axe_Attack.h"
#include "Soma.h"
#include "GameUI/FloatingNumberPool.h"

Axe::Axe()
{
	initAnim();
	initFSM();

	m_attributes.hp = 40;
	m_attributes.atk = 1;
	m_attributes.sp = 0.8f;
	m_attributes.cursp = 0;
	m_attributes.atk_cd = 120;
	m_attributes.cur_atk_cd = m_attributes.atk_cd;
	m_attributes.dx = 4;
	m_attributes.dy = 5;
	m_attributes.g = 1.5f;
	m_attributes.is_hit = false;
	m_attributes.is_invulnurable = false;
	m_attributes.invulnurable_time = 0.0f;//20.0f;
	m_attributes.is_dead = false;
	m_attributes.is_corpse = false;
	m_attributes.fall_height = 0;

	mDetectRange = { 768.0f, 192.0f };
}

void Axe::OnDebugRender()
{
	if (!mBodyCollider) return;

	float posx = transform().position().x;
	float posy = transform().position().y;

	float hw = DetectWith() / 2;
	float hh = DetectHeight() / 2;
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

void Axe::SavePos()
{
	mLastPos = mBodyCollider->GetCollider()->CollitionAABBRect();
}

void Axe::Awake()
{
	mBodyCollider = GT::CreateGameObject<GamePlay::ChildObject>();
	mBodyCollider->SetOwner(this);
	mBodyCollider->layer() = GamePlay::Enemy;
	GamePlay::BoxCollider* c = mBodyCollider->AddComponent<GamePlay::BoxCollider>();
	c->SetColliderSize(50, 162);
	c->SetCollitionCallBack([&](GamePlay::Collider* c) { return onCollision(c); });

	auto flash = AddComponent<GamePlay::HitFlash>();
	flash->SetFlashColor(0.6f, 0.05f, 0.05f);
	flash->SetFlashTiming(0.04f, 0.25f, 0.75f);
	//flash->SetFlashColor(1.0f, 1.0f, 1.0f);
	//flash->SetFlashTiming(0.05f, 0.3f, 0.3f);
}

void Axe::OnEnable()
{
}

void Axe::Start()
{
	if (!mBodyCollider) return;
	mBodyCollider->transform().SetPosition(transform().position());
	mBodyCollider->_ComponentUpdate(0);
}

void Axe::Update(float deltaTime)
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

	if (m_attributes.cur_atk_cd > 0)
	{
		m_attributes.cur_atk_cd -= 1;
		if (m_attributes.cur_atk_cd < 0)
			m_attributes.cur_atk_cd = 0;
	}

	UpdatePhysics(deltaTime);

	MathType::vec2f curPos = transform().position();
	const MathType::RectF& camBound = MainCamera->CameraBound();
	if (!camBound.Contains(curPos.x, curPos.y))
	{
		Destroy();
	}
}

void Axe::LaterUpdate(float deltaTime)
{
	EvaluateTransitions();
	m_attributes.m_last_scale_x = (int)transform().localScale().x;
	m_attributes.last_bottom = mBodyCollider->GetComponent<GamePlay::BoxCollider>()->CollitionAABBRect().bottom;
}

void Axe::OnDisable()
{
}

void Axe::OnDestroy()
{
	if (mBodyCollider)
	{
		mBodyCollider->Destroy();
		mBodyCollider = nullptr;
	}
}

void Axe::FacePlayer()
{
	float dist = PlayerDistanceX();

	if (dist > 0)
	{
		transform().SetScale({ -1.0f, 1.0f });
	}
	else
	{
		transform().SetScale({ 1.0f, 1.0f });
	}
}


void Axe::Hit(float atk,int atkType)
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

	m_attributes.hp -= atk;



	//int dmg = rand() % 100;
	MathType::vec2f curPos = transform().position();
	curPos.x += rand() % 50 - 25;
	curPos.y += rand() % 25 - 50;
	FLOATINGNUMS.Spawn(atk, curPos.x, curPos.y, 50.0f, 1.5f,true);


	if (m_attributes.hp <= 0)
	{
		CaptureDeathScatterDirection();
		m_attributes.is_dead = true;
		m_attributes.cursp = 0;
		if (mBodyCollider != nullptr && mBodyCollider->Active())
			mBodyCollider->SetActive(false);
		return;
	}

	m_attributes.is_invulnurable = true;
	m_attributes.cur_invulnurable_time = m_attributes.invulnurable_time;
}

void Axe::UpdatePhysics(float deltaTime)
{
	if (m_attributes.is_corpse)
		return;

	m_wasOnGround = mOnGround;
	m_wasOnSlop = mOnSlop;
	m_wasOnCollideTop = mOnCollideTop;
	mOnGround = false;
	mOnSlop = false;
	mOnCollideTop = false;

	m_attributes.cur_g = m_attributes.g;
	m_attributes.curjp -= m_attributes.cur_g;

	float speed = (m_attributes.dx == 6) ? m_attributes.cursp * deltaTime * 50 : -m_attributes.cursp * deltaTime * 50;
	float totalDx = speed;
	float totalDy = -m_attributes.curjp * deltaTime * 10;

	MathType::vec2f normalizedDir = { totalDx, totalDy };
	int detectCount = (int)normalizedDir.magnitude();
	if (detectCount == 0) detectCount = 1;
	normalizedDir.Normalize();

	GamePlay::Collider& self = *mBodyCollider->GetComponent<GamePlay::BoxCollider>();
	std::vector<GamePlay::Collider*> v;

	for (int i = 0; i < detectCount; ++i)
	{
		transform().translate(normalizedDir);
		mBodyCollider->transform().SetPosition(transform().position());
		mBodyCollider->_ComponentUpdate(deltaTime);
		if (GT::CollisionDetect(self, mCollisionFilter, v))
		{
			for (GamePlay::Collider* collider : v)
			{
				onCollision(collider);
			}
		}
		totalDx -= normalizedDir.x;
		totalDy -= normalizedDir.y;
	}

	transform().translate({ totalDx, totalDy });
	mBodyCollider->transform().SetPosition(transform().position());
	mBodyCollider->_ComponentUpdate(deltaTime);

	if (GT::CollisionDetect(self, mCollisionFilter, v))
	{
		for (GamePlay::Collider* collider : v)
		{
			onCollision(collider);
		}
	}

	if (mOnGround || mOnSlop)
	{
		m_attributes.curjp = 0;
	}

	CheckGroundBelow();
}

void Axe::onCollision(GamePlay::Collider* c)
{
	MathType::RectF selfRect = mBodyCollider->GetComponent<GamePlay::BoxCollider>()->CollitionAABBRect();
	MathType::RectF otherRect = c->CollitionAABBRect();
	MathType::vec2f curPos = transform().position();
	MathType::vec2f newPos = curPos;
	MathType::RectF overlapRect;
	if (!selfRect.Overlap(otherRect, &overlapRect)) { return; }

	float overlapW = overlapRect.Width();
	float overlapH = overlapRect.Height();

	
	if (c->gameobject()->layer() == GamePlay::Obstacle)
	{
		if (overlapW > overlapH)
		{
			if (selfRect.CenterY() > otherRect.CenterY())
			{
				newPos.y = curPos.y + overlapH;
				mOnCollideTop = true;
			}
			else
			{
				newPos.y = curPos.y - overlapH;
				mOnGround = true;
			}
		}
		else
		{
			if (selfRect.CenterX() > otherRect.CenterX())
			{
				newPos.x = curPos.x + overlapW;
			}
			else
			{
				newPos.x = curPos.x - overlapW;
			}
		}
	}
	else if (c->gameobject()->layer() == GamePlay::Platform)
	{
		if (m_attributes.curjp > 0) { return; }

		if (m_attributes.last_bottom <= otherRect.top && selfRect.bottom > otherRect.top)
		{
			newPos.y = curPos.y - overlapH;
			mOnGround = true;
		}
	}
	else if (c->gameobject()->layer() == GamePlay::EdgePlatform)
	{
		MathType::vec2f ptInRect(selfRect.CenterX(), selfRect.bottom);
		if (ptInRect.x < otherRect.left || ptInRect.x > otherRect.right) { return; }

		if (selfRect.CenterY() > otherRect.CenterY())
		{
			newPos.y = curPos.y + overlapH;
			mOnCollideTop = true;
		}
		else
		{
			newPos.y = curPos.y - overlapH;
			mOnGround = true;
		}
	}
	else if (c->gameobject()->layer() == GamePlay::LeftSlop)
	{
		MathType::vec2f ptInRect(selfRect.CenterX(), selfRect.bottom);
		if (m_attributes.curjp > 0) { return; }
		if (ptInRect.x < otherRect.left || ptInRect.x > otherRect.right) { return; }
		float k = otherRect.Height() / otherRect.Width();
		float w = ptInRect.x - otherRect.left;
		float t_y = otherRect.bottom - (k * w);

		if (ptInRect.y < t_y - 5.0f) { return; }

		mOnSlop = true;
		newPos.y = t_y - (selfRect.Height() / 2.0f);
		if (m_attributes.dx == 4)
		{
			newPos.y += 1;
		}
		else if (m_attributes.dx == 6)
		{
			newPos.y -= 1;
		}
	}
	else if (c->gameobject()->layer() == GamePlay::RightSlop)
	{
		MathType::vec2f ptInRect(selfRect.CenterX(), selfRect.bottom);
		if (m_attributes.curjp > 0) { return; }
		if (ptInRect.x < otherRect.left || ptInRect.x > otherRect.right) { return; }
		float k = otherRect.Height() / otherRect.Width();
		float w = ptInRect.x - otherRect.left;
		float t_y = otherRect.top + (k * w);

		if (ptInRect.y < t_y - 5.0f) { return; }

		mOnSlop = true;
		newPos.y = t_y - (selfRect.Height() / 2.0f);
		if (m_attributes.dx == 4)
		{
			newPos.y -= 1;
		}
		else if (m_attributes.dx == 6)
		{
			newPos.y += 1;
		}
	}

	if ((newPos.x != curPos.x) || (newPos.y != curPos.y))
	{
		transform().SetPosition(newPos);
		mBodyCollider->transform().SetPosition(newPos);
		mBodyCollider->_ComponentUpdate(0);
		return;
	}
}

void Axe::initAnim()
{
	TexMgr.LoadTextureWithoutColor(L"resource\\enemies\\p_axe00\\frame028.bmp");
	TexMgr.LoadTextureWithoutColor(L"resource\\enemies\\p_axe00\\frame029.bmp");
	TexMgr.LoadTextureWithoutColor(L"resource\\enemies\\p_axe00\\frame030.bmp");
	TexMgr.LoadTextureWithoutColor(L"resource\\enemies\\p_axe00\\frame031.bmp");
	TexMgr.LoadTextureWithoutColor(L"resource\\enemies\\p_axe00\\frame032.bmp");
	TexMgr.LoadTextureWithoutColor(L"resource\\enemies\\p_axe00\\frame033.bmp");
	TexMgr.LoadTextureWithoutColor(L"resource\\enemies\\p_axe00\\frame034.bmp");
	TexMgr.LoadTextureWithoutColor(L"resource\\enemies\\p_axe00\\frame035.bmp");
	TexMgr.LoadTextureWithoutColor(L"resource\\enemies\\p_axe00\\frame036.bmp");
	TexMgr.LoadTextureWithoutColor(L"resource\\enemies\\p_axe00\\frame037.bmp");
	TexMgr.LoadTextureWithoutColor(L"resource\\enemies\\p_axe00\\frame038.bmp");
	TexMgr.LoadTextureWithoutColor(L"resource\\enemies\\p_axe00\\frame039.bmp");
	TexMgr.LoadTextureWithoutColor(L"resource\\enemies\\p_axe00\\frame040.bmp");
	TexMgr.LoadTextureWithoutColor(L"resource\\enemies\\p_axe00\\frame041.bmp");
	TexMgr.LoadTextureWithoutColor(L"resource\\enemies\\p_axe00\\frame042.bmp");
	TexMgr.LoadTextureWithoutColor(L"resource\\enemies\\p_axe00\\frame043.bmp");

	auto* pAnim = AddComponent<GamePlay::Animator>();
	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\enemies\\p_axe00\\frame001.bmp",{0.5f, 0.5f},12, 0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame002.bmp",{0.5f, 0.5f},21, 0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame003.bmp",{0.5f, 0.5f},32, 0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame004.bmp",{0.5f, 0.5f},18, 0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame005.bmp",{0.5f, 0.5f},6,  0x00FF00 },
			});
		pAnim->AddClip(L"axe_idle", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\enemies\\p_axe00\\frame007.bmp",{0.5f, 0.5f},15 , 0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame008.bmp",{0.5f, 0.5f},30 , 0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame009.bmp",{0.5f, 0.5f},15 , 0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame00A.bmp",{0.5f, 0.5f},15 , 0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame00B.bmp",{0.5f, 0.5f},15 , 0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame00C.bmp",{0.5f, 0.5f},15 , 0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame00D.bmp",{0.5f, 0.5f},30 , 0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame00E.bmp",{0.5f, 0.5f},15 , 0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame00F.bmp",{0.5f, 0.5f},15 , 0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame006.bmp",{0.5f, 0.5f},15 , 0x00FF00 },
			});
		pAnim->AddClip(L"axe_walk", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\enemies\\p_axe00\\frame007.bmp",{0.5f, 0.5f},30 , 0x00FF00 },
				{L"resource\\enemies\\p_axe00\\frame027.bmp",{0.5f, 0.5f},10,0x00FF00 },
				{L"resource\\enemies\\p_axe00\\frame010.bmp",{0.5f, 0.5f},10,0x00FF00 },
				{L"resource\\enemies\\p_axe00\\frame011.bmp",{0.5f, 0.5f},10,0x00FF00 },
				{L"resource\\enemies\\p_axe00\\frame012.bmp",{0.5f, 0.5f},10,0x00FF00 },
				{L"resource\\enemies\\p_axe00\\frame013.bmp",{0.5f, 0.5f},10,0x00FF00 },
				{L"resource\\enemies\\p_axe00\\frame014.bmp",{0.5f, 0.5f},10,0x00FF00 },
				{L"resource\\enemies\\p_axe00\\frame015.bmp",{0.5f, 0.5f},10,0x00FF00 },
				{L"resource\\enemies\\p_axe00\\frame016.bmp",{0.5f, 0.5f},10,0x00FF00 },
				{L"resource\\enemies\\p_axe00\\frame017.bmp",{0.5f, 0.5f},10,0x00FF00 },
				{L"resource\\enemies\\p_axe00\\frame018.bmp",{0.5f, 0.5f},10,0x00FF00 },
				{L"resource\\enemies\\p_axe00\\frame019.bmp",{0.5f, 0.5f},10,0x00FF00 },
			});
		pAnim->AddClip(L"axe_attack_2", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\enemies\\p_axe00\\frame007.bmp",{0.5f, 0.5f},30 , 0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame01A.bmp",{0.5f, 0.5f},10,  0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame01B.bmp",{0.5f, 0.5f},10,  0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame01C.bmp",{0.5f, 0.5f},10,  0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame01D.bmp",{0.5f, 0.5f},10,  0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame01E.bmp",{0.5f, 0.5f},10,  0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame01F.bmp",{0.5f, 0.5f},10,  0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame020.bmp",{0.5f, 0.5f},10,  0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame021.bmp",{0.5f, 0.5f},10,  0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame022.bmp",{0.5f, 0.5f},10, 0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame023.bmp",{0.5f, 0.5f},10,  0x00FF00 },
			});
		pAnim->AddClip(L"axe_attack_1", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\enemies\\p_axe00\\frame024.bmp",{0.5f, 0.5f},3, 0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame025.bmp",{0.5f, 0.5f},3, 0x00FF00 },
			});
		pAnim->AddClip(L"axe_die", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\enemies\\p_axe00\\frame026.bmp",{0.5f, 0.5f},100, 0x00FF00 },
			});
		pAnim->AddClip(L"axe_object", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\enemies\\p_axe00\\frame024.bmp",{0.5f, 0.5f},3, 0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame025.bmp",{0.5f, 0.5f},3, 0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame024.bmp",{0.5f, 0.5f},3, 0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame025.bmp",{0.5f, 0.5f},3, 0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame024.bmp",{0.5f, 0.5f},3, 0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame025.bmp",{0.5f, 0.5f},3, 0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame024.bmp",{0.5f, 0.5f},3, 0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame025.bmp",{0.5f, 0.5f},3, 0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame024.bmp",{0.5f, 0.5f},3, 0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame025.bmp",{0.5f, 0.5f},3, 0x00FF00 },
				{ L"resource\\enemies\\p_axe00\\frame024.bmp",{0.5f, 0.5f},1, 0x00FF00 },
			});
		pAnim->AddClip(L"axe_dead", clip);
	}
}

void Axe::initFSM()
{
	GamePlay::FSM* fsm = AddComponent<GamePlay::FSM>();

	{
		Axe_Idle* state = new Axe_Idle();
		state->SetOwner(this);
		fsm->RegisterState(L"idle", state);
	}
	{
		Axe_Patrol* state = new Axe_Patrol();
		state->SetOwner(this);
		fsm->RegisterState(L"patrol", state);
	}
	{
		Axe_Alert* state = new Axe_Alert();
		state->SetOwner(this);
		fsm->RegisterState(L"alert", state);
	}
	{
		Axe_Chase* state = new Axe_Chase();
		state->SetOwner(this);
		fsm->RegisterState(L"chase", state);
	}
	{
		Axe_Attack* state = new Axe_Attack();
		state->SetOwner(this);
		fsm->RegisterState(L"attack", state);
	}
	{
		Axe_Dead* state = new Axe_Dead();
		state->SetOwner(this);
		fsm->RegisterState(L"dead", state);
	}

	fsm->InitState(L"idle");
}

void Axe::EvaluateTransitions()
{
	if (mOnGround || mOnSlop)
	{
		m_attributes.curjp = 0;
	}
}

void Axe::CheckGroundBelow()
{
	if (m_attributes.curjp >= 0 || mOnGround || mOnSlop) return;

	static const float groundSenseRange = 4.0f;
	MathType::RectF characterRect = mBodyCollider->GetComponent<GamePlay::BoxCollider>()->CollitionAABBRect();
	characterRect.top += groundSenseRange;
	characterRect.bottom += groundSenseRange;
	MathType::RectF overlapRect;

	bool foundGround = false;
	GamePlay::Collider& self = *mBodyCollider->GetComponent<GamePlay::BoxCollider>();
	std::vector<GamePlay::Collider*> colliders = GameObjectMgr.ColliderObjects();
	for (auto* collider : colliders)
	{
		if (collider == &self || !collider->gameobject()->Active()) continue;
		GamePlay::GameLayer colliderLayer = collider->gameobject()->layer();
		unsigned int colliderMask = ToBit(colliderLayer);
		if ((colliderMask & mCollisionFilter) == 0) continue;

		auto& rc = collider->CollitionAABBRect();
		if (!rc.Overlap(characterRect, &overlapRect)) continue;

		if (colliderLayer == GamePlay::Obstacle)
		{
			if (overlapRect.Width() > overlapRect.Height())
			{
				if (characterRect.CenterY() < rc.CenterY())
					foundGround = true;
			}
		}
		else if (colliderLayer == GamePlay::Platform)
		{
			if (m_attributes.last_bottom <= rc.top && characterRect.bottom >= rc.top)
				foundGround = true;
		}
		else if (colliderLayer == GamePlay::EdgePlatform)
		{
			float footX = characterRect.CenterX();
			if (footX < rc.left || footX > rc.right) continue;
			if (m_attributes.last_bottom <= rc.top && characterRect.bottom >= rc.top)
				foundGround = true;
		}
		else if (colliderLayer == GamePlay::LeftSlop || colliderLayer == GamePlay::RightSlop)
		{
			float footX = characterRect.CenterX();
			if (footX < rc.left || footX > rc.right) continue;
			float k = rc.Height() / rc.Width();
			float w = footX - rc.left;
			float dy = k * w;
			float ty = (colliderLayer == GamePlay::LeftSlop) ? rc.bottom - dy : rc.top + dy;
			if (m_attributes.last_bottom < ty && characterRect.bottom > ty) foundGround = true;
		}

		if (foundGround)
		{
			mOnGround = true;
			m_attributes.curjp = 0;
			break;
		}
	}
}
