#include "Zombi.h"
#include "./D2DCore/TextureManager.h"
#include "./HelpTool/RandomMaker.h"
#include "GameTool.h"
#include "GameCore/GameLoop.h"
#include "Zombi_Idle.h"
#include "Zombi_Patrol.h"
#include "Zombi_Alert.h"
#include "Zombi_Dead.h"
#include "Zombi_Chase.h"
#include "Soma.h"
#include "Player.h"
#include "GameUI/FloatingNumberPool.h"

Zombi::Zombi()
{
	initAnim();
	initFSM();

	m_attributes.hp = 10;
	m_attributes.atk = 8;
	m_attributes.sp = 1.0f;
	m_attributes.cursp = 0;
	m_attributes.atk_cd = 60;
	m_attributes.cur_atk_cd = m_attributes.atk_cd;
	m_attributes.dx = 4;
	m_attributes.dy = 5;
	m_attributes.g = 1.5f;

	m_attributes.is_hit = false;
	m_attributes.is_invulnurable = false;
	m_attributes.invulnurable_time = 0.0f;
	m_attributes.is_dead = false;
	m_attributes.is_corpse = false;
	m_attributes.fall_height = 0;
	mDetectRange = { 400.0f, 200.0f };
}

void Zombi::OnDebugRender()
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

void Zombi::SavePos()
{
	mLastPos = mBodyCollider->GetCollider()->CollitionAABBRect();
}

void Zombi::Awake()
{
	// 创建独立碰撞器对象作为子对象
	mBodyCollider = GT::CreateGameObject<GamePlay::ChildObject>();
	mBodyCollider->SetOwner(this);
	mBodyCollider->layer() = GamePlay::Enemy;
	GamePlay::BoxCollider* c = mBodyCollider->AddComponent<GamePlay::BoxCollider>();
	c->SetColliderSize(50, 135);
	c->SetCollitionCallBack([&](GamePlay::Collider* c) { return onCollision(c); });

	// 1. 角色初始化时添加组件
	auto flash = AddComponent<GamePlay::HitFlash>();
	flash->SetFlashColor(0.6f, 0.05f, 0.05f);
	flash->SetFlashTiming(0.04f, 0.25f, 0.75f);
	//flash->SetFlashColor(1.0f, 0.3f, 0.3f);     // 红色闪烁
	//flash->SetFlashTiming(0.05f, 0.3f, 1.0f);   // 峰值时间、衰减时间、最大强度

}

void Zombi::OnEnable()
{
}

void Zombi::Start()
{
	if (!mBodyCollider) return;
	mBodyCollider->transform().SetPosition(transform().position());
	mBodyCollider->_ComponentUpdate(0);
}

void Zombi::Update(float deltaTime)
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

	UpdatePhysics(deltaTime);
}

void Zombi::LaterUpdate(float deltaTime)
{
	EvaluateTransitions();

	m_attributes.m_last_scale_x = (int)transform().localScale().x;
	m_attributes.last_bottom = mBodyCollider->GetComponent<GamePlay::BoxCollider>()->CollitionAABBRect().bottom;
}

void Zombi::OnDisable()
{
}

void Zombi::OnDestroy()
{
	if (mBodyCollider)
	{
		mBodyCollider->Destroy();
		mBodyCollider = nullptr;
	}
}

void Zombi::FacePlayer()
{
	float dist = PlayerDistanceX();

	if (dist > 0)
	{
		transform().SetScale({ -1.0f, 1.0f });
		m_attributes.dx = 6;
	}
	else
	{
		transform().SetScale({ 1.0f, 1.0f });
		m_attributes.dx = 4;
	}
}

void Zombi::Hit(float atk, int atkType)
{
	
	if (m_attributes.is_invulnurable || m_attributes.is_dead) return;

	Enemy::Hit(atk, atkType);
	GetComponent<GamePlay::HitFlash>()->TriggerHit();

	//int dmg = rand() % 100;
	MathType::vec2f curPos = transform().position();
	FLOATINGNUMS.Spawn(atk, curPos.x, curPos.y, 50.0f, 1.5f, true);


	m_attributes.hp -= atk;
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

void Zombi::UpdatePhysics(float deltaTime)
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

void Zombi::onCollision(GamePlay::Collider* c)
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

void Zombi::initAnim()
{
	TexMgr.LoadTextureWithoutColor(L"resource\\enemies\\p_zombi\\frame011.bmp");
	TexMgr.LoadTextureWithoutColor(L"resource\\enemies\\p_zombi\\frame012.bmp");
	TexMgr.LoadTextureWithoutColor(L"resource\\enemies\\p_zombi\\frame013.bmp");
	TexMgr.LoadTextureWithoutColor(L"resource\\enemies\\p_zombi\\frame014.bmp");
	TexMgr.LoadTextureWithoutColor(L"resource\\enemies\\p_zombi\\frame015.bmp");
	TexMgr.LoadTextureWithoutColor(L"resource\\enemies\\p_zombi\\frame016.bmp");

	auto* pAnim = AddComponent<GamePlay::Animator>();

	
	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\enemies\\p_zombi\\frame000.bmp",{0.53f, 0.47f},13, 0x00FF00 },
				{ L"resource\\enemies\\p_zombi\\frame001.bmp",{0.53f, 0.47f},4,  0x00FF00 },
				{ L"resource\\enemies\\p_zombi\\frame002.bmp",{0.53f, 0.47f},6,  0x00FF00 },
				{ L"resource\\enemies\\p_zombi\\frame003.bmp",{0.53f, 0.47f},8,  0x00FF00 },
				{ L"resource\\enemies\\p_zombi\\frame004.bmp",{0.53f, 0.47f},9,  0x00FF00 },
				{ L"resource\\enemies\\p_zombi\\frame005.bmp",{0.53f, 0.47f},8,  0x00FF00 },
				{ L"resource\\enemies\\p_zombi\\frame006.bmp",{0.53f, 0.47f},8,  0x00FF00 },
				{ L"resource\\enemies\\p_zombi\\frame007.bmp",{0.53f, 0.47f},9,  0x00FF00 },
			});
		pAnim->AddClip(L"zombi_idle", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\enemies\\p_zombi\\frame008.bmp",{0.53f, 0.47f},9,  0x00FF00 },
				{ L"resource\\enemies\\p_zombi\\frame009.bmp",{0.53f, 0.47f},9,  0x00FF00 },
				{ L"resource\\enemies\\p_zombi\\frame00A.bmp",{0.53f, 0.47f},9,  0x00FF00 },
				{ L"resource\\enemies\\p_zombi\\frame00B.bmp",{0.53f, 0.47f},11, 0x00FF00 },
				{ L"resource\\enemies\\p_zombi\\frame00C.bmp",{0.53f, 0.47f},11, 0x00FF00 },
				{ L"resource\\enemies\\p_zombi\\frame00D.bmp",{0.53f, 0.47f},9,  0x00FF00 },
				{ L"resource\\enemies\\p_zombi\\frame00E.bmp",{0.53f, 0.47f},9,  0x00FF00 },
				{ L"resource\\enemies\\p_zombi\\frame00F.bmp",{0.53f, 0.47f},13, 0x00FF00 },
			});
		pAnim->AddClip(L"zombi_walk", clip);
	}
}

void Zombi::initFSM()
{
	GamePlay::FSM* fsm = AddComponent<GamePlay::FSM>();

	{
		Zombi_Idle* state = new Zombi_Idle();
		state->SetOwner(this);
		fsm->RegisterState(L"idle", state);
	}
	{
		Zombi_Patrol* state = new Zombi_Patrol();
		state->SetOwner(this);
		fsm->RegisterState(L"patrol", state);
	}
	{
		Zombi_Alert* state = new Zombi_Alert();
		state->SetOwner(this);
		fsm->RegisterState(L"alert", state);
	}
	{
		Zombi_Chase* state = new Zombi_Chase();
		state->SetOwner(this);
		fsm->RegisterState(L"chase", state);
	}
	{
		Zombi_Dead* state = new Zombi_Dead();
		state->SetOwner(this);
		fsm->RegisterState(L"dead", state);
	}

	fsm->InitState(L"idle");
}

void Zombi::EvaluateTransitions()
{
	if (mOnGround || mOnSlop)
	{
		m_attributes.curjp = 0;
	}
}

void Zombi::CheckGroundBelow()
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

			if (m_attributes.last_bottom < ty && characterRect.bottom > ty)
				foundGround = true;
		}

		if (foundGround)
		{
			mOnGround = true;
			m_attributes.curjp = 0;
			break;
		}
	}
}
