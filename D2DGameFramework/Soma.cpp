#include "Soma.h"
#include "HelpTool/INIConfig.h"
#include "./D2DCore/TextureManager.h"
#include "./HelpTool/RandomMaker.h"
#include "GameTool.h"
#include "GamePlay/Debug.h"
#include "GameCore/GameLoop.h"
#include "AttackEffect.h"
#include "Soma_Idle.h"
#include "Soma_Walk.h"
#include "Soma_Jump.h"
#include "Soma_DoubleJump.h"
#include "Soma_Fall.h"
#include "Soma_Crouch.h"
#include "Soma_Dash_Back.h"
#include "Soma_Slide_Flat.h"
#include "Soma_Attack_Stand.h"
#include "Soma_Attack_Crouch.h"
#include "Soma_Attack_Air.h"
#include "Soma_Great_Sword_Attack_Air.h"
#include "Soma_Great_Sword_Attack_Crouch.h"
#include "Soma_Great_Sword_Attack_Stand.h"
#include "Soma_Great_Sword_Supper_Attack.h"
#include "Soma_Sword_Supper_Attack.h"
#include "Soma_Punch_Stand.h"
#include "Soma_Punch_Crouch.h"
#include "Soma_Punch_Air.h"
#include "Soma_AxeBullet_Ground.h"
#include "Soma_AxeBullet_Air.h"
#include "Soma_Hit_Stand.h"
#include "Soma_Hit_Air.h"
#include "Soma_Land.h"
#include "Soma_BlackPanther.h"
#include "Soma_Strike_Air.h"
#include "Soma_Dead.h"
#include "Enemy.h"
#include "AttackCollider.h"
#include "S_Fly.h"
#include "S_BlackPather.h"
#include "EffectsPool.h"
#include <algorithm>
#include <cmath>
#include "./GameUI/FloatingNumberPool.h"

Soma::Soma()
{
	mLayer = GamePlay::GameLayer::Layer_5;
	auto a = AddComponent<GamePlay::AfterImageTrail>();
	a->SetLifeTime(0.16f);
	a->SetSampleInterval(2);
	a->SetCapState(true);
	mBodyCollider = GT::CreateGameObject<GamePlay::ChildObject>();
	mBodyCollider->SetOwner(this);
	mBodyCollider->layer() = GamePlay::Player;
	GamePlay::BoxCollider* c = mBodyCollider->AddComponent<GamePlay::BoxCollider>();
	c->SetColliderSize(40, 105);
	c->SetCollitionCallBack([&](GamePlay::Collider* c) { return onCollision(c); });

	m_attributes.atk = 5;
	m_attributes.hp = 120;
	m_attributes.cur_hp = m_attributes.hp;
	m_attributes.mp = 120;
	m_attributes.cur_mp = m_attributes.mp;

	m_attributes.sp = 4;
	m_attributes.cursp = 0;
	m_attributes.jp = 9.0f; //9
	m_attributes.curjp = 0;
	m_attributes.dx = 5;
	m_attributes.dy = 5;
	m_attributes.max_jp_height = 4 * 16 * 3;
	m_attributes.cur_jp_height = 0;
	m_attributes.air_jump_remain = 1;
	m_attributes.g = 3.0f;
	m_attributes.fall_height = 0;

	m_attributes.is_hit = false;
	m_attributes.is_invulnurable = false;
	m_attributes.invulnurable_time = 90;
				
	m_attributes.is_dead = false;
	m_attributes.is_corpse = false;

	mDashCdTimer.SetWaitTime(1.2f);
	mDashCdTimer.SetOneShot(true);
	mDashCdTimer.SetOnTimeout([this]() { mCanDash = true; });

	mDeathTimer.SetWaitTime(1.5f);
	mDeathTimer.SetOneShot(true);
	mDeathTimer.SetOnTimeout([this]() { GT::SwitchTo("game_over"); });
}

bool Soma::TryConsumeStamina(float amount)
{
	if (IsDead() || !std::isfinite(amount) || amount < 0 || m_attributes.cur_mp < amount) return false;
	if (amount == 0) return true;
	m_attributes.cur_mp = (std::max)(0.0f, m_attributes.cur_mp - amount);
	return true;
}

bool Soma::TryConsumeSuperAttack()
{
	return mHand && TryConsumeStamina(static_cast<float>(mHand->super_mp_cost));
}

bool Soma::TryConsumeRedSoul()
{
	return mRedSoul && TryConsumeStamina(static_cast<float>(mRedSoul->mp_cost));
}

void Soma::UpdateStamina(float deltaTime)
{
	if (!std::isfinite(deltaTime) || deltaTime <= 0 || IsDead()) return;
	const float maximum = static_cast<float>((std::max)(0, m_attributes.mp));
	m_attributes.cur_mp = std::clamp(m_attributes.cur_mp + deltaTime * (std::max)(0, m_attributes.mp_recover_amount), 0.0f, maximum);
}

void Soma::StartDashCooldown()
{
	mCanDash = false;
	mDashCdTimer.Restart();
}

void Soma::OnDebugRender()
{
	GamePlay::BoxCollider* bc = mBodyCollider->GetComponent<GamePlay::BoxCollider>();
	
	if (bc != nullptr)
	{
		float posx = transform().position().x;
		float posy = transform().position().y;
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

		if (mSlideAtkCollider && mSlideAtkCollider->IsActive())
		{
			float posx = mSlideAtkCollider->gameobject()->transform().position().x;
			float posy = mSlideAtkCollider->gameobject()->transform().position().y;
			float w = mSlideAtkCollider->Width();
			float h = mSlideAtkCollider->Height();
			float halfW = w / 2;
			float halfH = h / 2;
			Debug.DebugDrawLine(posx - halfW, posy - halfH, posx + halfW, posy - halfH, ColorType::COLORF::Red());
			Debug.DebugDrawLine(posx - halfW, posy - halfH, posx - halfW, posy + halfH, ColorType::COLORF::Red());
			Debug.DebugDrawLine(posx + halfW, posy - halfH, posx + halfW, posy + halfH, ColorType::COLORF::Red());
			Debug.DebugDrawLine(posx - halfW, posy + halfH, posx + halfW, posy + halfH, ColorType::COLORF::Red());
		}
		

	}

	// 使魔等 SoulSkill 对象自身为 inactive，_OnDebugRender 会跳过它们，
	// 这里手动转发调试渲染调用
	if (mBulueSoul && mBulueSoul->p_skill)
		mBulueSoul->p_skill->OnDebugRender();
	if (mRedSoul && mRedSoul->p_skill)
		mRedSoul->p_skill->OnDebugRender();
	
}

void Soma::Awake()
{

	mSlideAtkCollider = AttackCollider::Create();
	mSlideAtkCollider->SetColliderSize(120, 40);
	mSlideAtkCollider->SetTimeInHit(0, 1);
	mSlideAtkCollider->SetAttack(m_attributes.atk, 2);
	mSlideAtkCollider->SetTarget(*this);
	std::wstring aniKey = EFFECT_RED_HIT;
	mSlideAtkCollider->SetEffectKey(aniKey);



	initAnim();
	initFSM();

}

void Soma::OnEnable()
{

}

void Soma::Start()
{
	mAttackEffect = GT::CreateGameObject<AttackEffect>();
	mAttackEffect->SetOwner(this);

	mBodyCollider->transform().SetPosition(transform().position());
	mBodyCollider->_ComponentUpdate(0);
	SavePos();
	m_attributes.last_bottom = mLastPos.bottom;
	m_attributes.m_last_scale_x = transform().localScale().x;
}

void Soma::Update(float deltaTime)
{
	mDashCdTimer.OnUpdate(deltaTime);

	if (m_attributes.is_dead)
		mDeathTimer.OnUpdate(deltaTime);

	if (m_attributes.is_invulnurable)
	{
		m_attributes.cur_invulnurable_time -= 1;
		if (m_attributes.cur_invulnurable_time <= 0)
		{
			m_attributes.is_invulnurable = false;
			if (!IsDead() && !mBodyCollider->Active())
			{
				mBodyCollider->SetActive(true);
			}
		}
	}

	UpdatePhysics(deltaTime);

	if (mSlideAtkCollider && mSlideAtkCollider->IsActive())
	{
		GamePlay::FSM* pfsm = GetComponent<GamePlay::FSM>();
		mSlideAtkCollider->SetColliderSize(120, 40);
		float xOffset = 50.0f;
		float yOffset = -30.0f;
		if (pfsm->GetCurrentState() == pfsm->GetState(L"slide_flat"))
			yOffset = 50.0f;
		else if (pfsm->GetCurrentState() == pfsm->GetState(L"punch_attack_stand"))
			yOffset = -30.0f;
		else if (pfsm->GetCurrentState() == pfsm->GetState(L"punch_attack_crouch"))
			yOffset = 10.0f;
		else if (pfsm->GetCurrentState() == pfsm->GetState(L"punch_attack_air"))
			yOffset = -30.0f;
		else if (pfsm->GetCurrentState() == pfsm->GetState(L"black_panther"))
		{
			yOffset = 10.0f;
			mSlideAtkCollider->SetColliderSize(120, 120);
		}
		else if (pfsm->GetCurrentState() == pfsm->GetState(L"strike_air"))
		{
			xOffset = 0.0f;
			yOffset = 60.0f;
		}

		mSlideAtkCollider->SetOffset({ xOffset, yOffset });
		mSlideAtkCollider->Update(deltaTime);
	}

	if(m_attributes.m_last_scale_x * (int)transform().localScale().x < 0)
	{
		m_attributes.m_is_turn = true;
	}

}

void Soma::OnDisable()
{
	
}

void Soma::OnDestroy()
{
	if (mBodyCollider) mBodyCollider->Destroy();
	if (mAttackEffect)
	{
		mAttackEffect->Destroy();
	}
	if (mSlideAtkCollider)
	{
		mSlideAtkCollider->gameobject()->Destroy();
	}
}

void Soma::EnableSlideAtkCollider(bool enable)
{
	if (mSlideAtkCollider)
	{
		if (enable)
		{
			mSlideAtkCollider->SetAttack(m_attributes.atk,2);
			mSlideAtkCollider->Activate();
		}
		else
			mSlideAtkCollider->Deactivate();
	}
}

bool Soma::ConsumeSlideAttackHit()
{
	return mSlideAtkCollider ? mSlideAtkCollider->ConsumeHit() : false;
}


void Soma::UpdatePhysics(float deltaTime)
{
	if (m_attributes.is_corpse)
		return;

	m_wasOnGround = mOnGround;
	m_wasOnCollideTop = mOnCollideTop;
	mOnObstacle = false;
	mOnPlatform = false;
	mOnEdgePlatform = false;
	mOnSlop = false;
	mOnGround = false;
	mOnCollideTop = false;
	m_currentPlatformCollider = nullptr;
	bool wasInAir = !m_wasOnGround;
	
	m_attributes.cur_g = m_attributes.g * 1.93f;//m_attributes.g * 1.93f;


	//jp大于0时重力正常累加,jp < 0时重力插值累加
	//if (m_attributes.curjp >= 0)
	//{
	//	m_attributes.curjp -= m_attributes.cur_g;
	//
	//}
	//else
	//{
		float new_jp = m_attributes.curjp - m_attributes.cur_g;
		float t = m_attributes.curjp >= 0 ? deltaTime * 40 : deltaTime * 30;
		m_attributes.curjp = new_jp * t + m_attributes.curjp * (1 - t);
	//}
	
	
	float totalDx, totalDy;
	totalDx = (m_attributes.dx == 6 ? 1.0f : m_attributes.dx == 4 ? -1.0f : 0.0f) * m_attributes.cursp * deltaTime * 68;
	totalDy = -m_attributes.curjp * deltaTime * 8.0f;

	
	MathType::vec2f normalizedDir = { totalDx,totalDy };
	float totalMag = normalizedDir.magnitude();
	int detectCount = (int)totalMag;
	normalizedDir.Normalize();

	// detectCount == 0 但总位移 > 0 时仍执行一次碰撞检测，防止极小速度下的穿模盲区
	if (detectCount == 0 && totalMag > 0.0f) { detectCount = 1; }

	GamePlay::Collider& self = *mBodyCollider->GetComponent<GamePlay::BoxCollider>();
	std::vector<GamePlay::Collider*> v;
	
	unsigned int mask = ToBit(GamePlay::Obstacle) | ToBit(GamePlay::LeftSlop) |
		ToBit(GamePlay::RightSlop) | ToBit(GamePlay::Platform) | ToBit(GamePlay::EdgePlatform);
	for (int i = 0; i < detectCount; ++i)
	{
		transform().translate(normalizedDir);
		mBodyCollider->transform().SetPosition(transform().position());
		mBodyCollider->_ComponentUpdate(deltaTime);
		if (GT::CollisionDetect(self, mask, v))
		{
			for (GamePlay::Collider* collider : v)
			{
				onCollision(collider);
			}
		}
		totalDx -= normalizedDir.x;
		totalDy -= normalizedDir.y;
	}

	transform().translate({totalDx,totalDy});
	mBodyCollider->transform().SetPosition(transform().position());
	mBodyCollider->_ComponentUpdate(deltaTime);

	if (GT::CollisionDetect(self, mask, v))
	{
		for (GamePlay::Collider* collider : v)
		{
			onCollision(collider);
		}
		
	}

	CheckGroundBelow();

}

void Soma::EvaluateTransitions()
{
	m_justLanded = !m_wasOnGround && mOnGround;
	const float deltaY = mBodyCollider->GetCollider()->CollitionAABBRect().CenterY() - mLastPos.CenterY();
	if (!m_wasOnGround || !mOnGround)
	{
		if (deltaY < -0.001f)
		{
			m_attributes.fall_height = 0;
			m_attributes.cur_jp_height -= deltaY;
		}
		else m_attributes.fall_height += (std::max)(0.0f, deltaY);
	}
	if (m_justLanded) m_attributes.landing_fall_height = m_attributes.fall_height;
	if (mOnGround)
	{
		m_attributes.curjp = 0;
		m_attributes.air_jump_remain = 1;
		m_attributes.fall_height = 0;
		m_attributes.cur_jp_height = 0;
	}
	if (mOnCollideTop) m_attributes.curjp = 0;
	SavePos();
	m_attributes.last_bottom = mLastPos.bottom;
	m_attributes.m_last_scale_x = transform().localScale().x;
	auto* fsm = GetComponent<GamePlay::FSM>();
	if (!fsm || !fsm->GetCurrentState()) return;
	auto* current = fsm->GetCurrentState();
	auto is = [&](const wchar_t* key) { return current == fsm->GetState(key); };
	if (IsDead() || m_attributes.is_corpse)
	{
		if (!is(L"dead")) fsm->SetNextState(L"dead");
		return;
	}
	if (m_attributes.is_hit && m_attributes.is_hit_state_change && !is(L"hit_stand") && !is(L"hit_air"))
	{
		fsm->SetNextState(mOnGround ? L"hit_stand" : L"hit_air");
		return;
	}
	if (!mOnGround && is(L"hit_stand")) { fsm->SetNextState(L"hit_air"); return; }
	if (mOnGround)
	{
		for (const auto* key : {L"jump", L"double_jump", L"fall", L"attack_air", L"punch_attack_air",
			L"using_axe_bullet_in_air", L"hit_air", L"strike_air"})
			if (is(key)) { fsm->SetNextState(L"land"); return; }
		if (fsm->GetNextState() == fsm->GetState(L"fall")) fsm->SetNextState(L"idle");
	}
	else
	{
		for (const auto* key : {L"idle", L"walk", L"crouch", L"dash_back", L"slide_flat", L"land", L"attack_stand",
			L"attack_crouch", L"great_sword_attack_stand", L"great_sword_attack_crouch", L"sword_supper_attack",
			L"great_sword_supper_attack", L"punch_attack_stand", L"punch_attack_crouch", L"using_axe_bullet_on_ground"})
			if (is(key)) { fsm->SetNextState(L"fall"); return; }
		if (fsm->GetNextState() == fsm->GetState(L"land")) fsm->SetNextState(L"fall");
	}
}

void Soma::OnRoomEntered()
{
	m_attributes.m_last_scale_x = transform().localScale().x;
	mBodyCollider->transform().SetPosition(transform().position());
	mBodyCollider->_ComponentUpdate(0);
	SavePos();
	m_attributes.last_bottom = mLastPos.bottom;
	m_attributes.fall_height = m_attributes.landing_fall_height = m_attributes.cur_jp_height = 0;
	m_justLanded = false;
	m_ignoredCollider = m_currentPlatformCollider = nullptr;
	mOnPlatform = mOnObstacle = mOnEdgePlatform = mOnSlop = false;
}

void Soma::CheckGroundBelow()
{
	// curjp > 0 表示仍在上升，不需要检测地面；curjp == 0 时可能 detectCount = 0，需要兜底
	if (m_attributes.curjp > 0 || mOnGround == true) return;
	static const float groundSenseRange = 4.0f;
	MathType::RectF characterRect = mBodyCollider->GetComponent<GamePlay::BoxCollider>()->CollitionAABBRect();
	characterRect.top += groundSenseRange;
	characterRect.bottom += groundSenseRange;
	MathType::RectF overlapRect;

	bool foundGround = false;
	GamePlay::Collider& self = *mBodyCollider->GetComponent<GamePlay::BoxCollider>();
	unsigned int mask = ToBit(GamePlay::Obstacle) | ToBit(GamePlay::LeftSlop) |
		ToBit(GamePlay::RightSlop) | ToBit(GamePlay::Platform) | ToBit(GamePlay::EdgePlatform);
	unsigned int colliderMask;
	GamePlay::GameLayer colliderLayer;
	std::vector<GamePlay::Collider*> colliders = GameObjectMgr.ColliderObjects();
	for (auto* collider : colliders)
	{

		if (collider == &self || !collider->gameobject()->Active() || collider == m_ignoredCollider) continue;
		colliderLayer = collider->gameobject()->layer();
		colliderMask = ToBit(colliderLayer);
		if((colliderMask & mask) == 0) continue;

		auto& rc = collider->CollitionAABBRect();
		if (!rc.Overlap(characterRect, &overlapRect)) continue;

		if (colliderLayer == GamePlay::Obstacle)
		{
			// 用上一帧底边位置判断是否从上方落在障碍物上，避免侧面误判
			if (overlapRect.Width() > overlapRect.Height())
			{
				if (m_attributes.last_bottom <= rc.top && characterRect.bottom > rc.top)
				{
					foundGround = true;
				}
			}
		}
		else if (colliderLayer == GamePlay::Platform)
		{
			if (m_attributes.last_bottom <= rc.top && characterRect.bottom >= rc.top)
			{ 
				foundGround = true; 
				m_currentPlatformCollider = collider;
			}
		}
		else if (colliderLayer == GamePlay::EdgePlatform)
		{
			float footX = characterRect.CenterX();
			if (footX < rc.left || footX > rc.right) { continue; }
			if (m_attributes.last_bottom <= rc.top && characterRect.bottom >= rc.top)
			{
				foundGround = true;
			}

		}
		else if (colliderLayer == GamePlay::LeftSlop || colliderLayer == GamePlay::RightSlop)
		{
			float footX = characterRect.CenterX();
			if (footX < rc.left || footX > rc.right) { continue; }

			float k = rc.Height() / rc.Width();
			float w = footX - rc.left;
			float dy = k * w;
			float ty = (colliderLayer == GamePlay::LeftSlop) ? rc.bottom - dy : rc.top + dy;

			if (m_attributes.last_bottom < ty && characterRect.bottom > ty) { foundGround = true; }
		}

		if (foundGround) 
		{ 
			mOnGround = true;
			break; 
		}
	
	}

}

bool Soma::IsTurnDir()
{
	float curScaleX = transform().localScale().x;
	float lastScaleX = m_attributes.m_last_scale_x;
	return (lastScaleX > 0 && curScaleX < 0) || (lastScaleX < 0 && curScaleX > 0);
}

void Soma::LaterUpdate(float deltaTime)
{
	if (!IsDead())
	{
		if (mBulueSoul && mBulueSoul->p_skill) mBulueSoul->p_skill->Update(deltaTime);
		if (mRedSoul && mRedSoul->p_skill) mRedSoul->p_skill->Update(deltaTime);
	}
	UpdateStamina(deltaTime);
	EvaluateTransitions();
}
void Soma::SavePos()
{
	mLastPos = mBodyCollider->GetCollider()->CollitionAABBRect();
}

void Soma::SaveTo(HelpTool::INIConfig& ini)
{
	Attributes& a = m_attributes;

	// 成长
	ini.Value("stats", "level") = a.level;
	ini.Value("stats", "exp") = a.exp;
	ini.Value("stats", "next_level_exp") = a.next_level_exp;
	ini.Value("stats", "gold") = a.gold;

	// 生命法力
	ini.Value("stats", "hp") = a.hp;
	ini.Value("stats", "cur_hp") = a.cur_hp;
	ini.Value("stats", "mp") = a.mp;
	ini.Value("stats", "cur_mp") = a.cur_mp;

	// 主属性
	ini.Value("stats", "atk") = a.atk;
	ini.Value("stats", "def") = a.def;
	ini.Value("stats", "lck") = a.lck;
	ini.Value("stats", "str") = a.str;
	ini.Value("stats", "con") = a.con;
	ini.Value("stats", "intel") = a.intel;

	// 物理参数
	ini.Value("stats", "sp") = a.sp;
	ini.Value("stats", "jp") = a.jp;
	ini.Value("stats", "g") = a.g;
	ini.Value("stats", "max_jp_height") = a.max_jp_height;

	// 位置
	ini.Value("position", "x") = transform().position().x;
	ini.Value("position", "y") = transform().position().y;
}

void Soma::LoadFrom(HelpTool::INIConfig& ini)
{
	Attributes& a = m_attributes;

	// 成长
	if (ini.HasKey("stats", "level")) a.level = (int)ini.Value("stats", "level");
	if (ini.HasKey("stats", "exp")) a.exp = (int)ini.Value("stats", "exp");
	if (ini.HasKey("stats", "next_level_exp")) a.next_level_exp = (int)ini.Value("stats", "next_level_exp");
	if (ini.HasKey("stats", "gold")) a.gold = (int)ini.Value("stats", "gold");

	// 生命法力
	if (ini.HasKey("stats", "hp")) a.hp = (float)ini.Value("stats", "hp");
	if (ini.HasKey("stats", "cur_hp")) a.cur_hp = (float)ini.Value("stats", "cur_hp");
	if (ini.HasKey("stats", "mp")) a.mp = (int)ini.Value("stats", "mp");
	if (ini.HasKey("stats", "cur_mp")) a.cur_mp = (float)ini.Value("stats", "cur_mp");

	// 主属性
	if (ini.HasKey("stats", "atk")) a.atk = (int)ini.Value("stats", "atk");
	if (ini.HasKey("stats", "def")) a.def = (int)ini.Value("stats", "def");
	if (ini.HasKey("stats", "lck")) a.lck = (int)ini.Value("stats", "lck");
	if (ini.HasKey("stats", "str")) a.str = (int)ini.Value("stats", "str");
	if (ini.HasKey("stats", "con")) a.con = (int)ini.Value("stats", "con");
	if (ini.HasKey("stats", "intel")) a.intel = (int)ini.Value("stats", "intel");

	// 物理参数
	if (ini.HasKey("stats", "sp")) a.sp = (float)ini.Value("stats", "sp");
	if (ini.HasKey("stats", "jp")) a.jp = (float)ini.Value("stats", "jp");
	if (ini.HasKey("stats", "g")) a.g = (float)ini.Value("stats", "g");
	if (ini.HasKey("stats", "max_jp_height")) a.max_jp_height = (float)ini.Value("stats", "max_jp_height");

	// 位置
	if (ini.HasKey("position", "x") && ini.HasKey("position", "y"))
	{
		float x = (float)ini.Value("position", "x");
		float y = (float)ini.Value("position", "y");
		transform().SetPosition({ x, y });
	}
}

void Soma::Hit(int atk)
{
	if (IsInvulnerable()) return;

	m_attributes.cur_hp = (std::max)(0.0f, m_attributes.cur_hp - atk);
	m_attributes.is_hit = true;
	m_attributes.is_invulnurable = true;
	m_attributes.cur_invulnurable_time = m_attributes.invulnurable_time;

	//int dmg = rand() % 100;
	MathType::vec2f curPos = transform().position();
	curPos.x += rand() % 50 - 25;
	curPos.y += rand() % 25 - 50;
	FLOATINGNUMS_HIT.Spawn(atk, curPos.x, curPos.y, 50.0f, 1.5f, true);

	if (m_attributes.cur_hp <= 0)
	{
		m_attributes.is_dead = true;
		m_attributes.cursp = 0;
		if (!mDeathTriggered)
		{
			mDeathTriggered = true;
			mDeathTimer.Restart();
		}
	}
	if (mBodyCollider->Active())
	{
		mBodyCollider->SetActive(false);
	}
	
}

void Soma::initAnim()
{
	auto* pAnim = AddComponent<GamePlay::Animator>();
	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame001.bmp",{0.46f,0.50f},4, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame002.bmp",{0.46f,0.50f},4, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame003.bmp",{0.46f,0.50f},4, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame004.bmp",{0.46f,0.50f},4, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame005.bmp",{0.46f,0.50f},4, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame006.bmp",{0.46f,0.50f},4, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame007.bmp",{0.46f,0.50f},10,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame006.bmp",{0.46f,0.50f},5, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame005.bmp",{0.46f,0.50f},5, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame004.bmp",{0.46f,0.50f},5, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame003.bmp",{0.46f,0.50f},6, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame002.bmp",{0.46f,0.50f},7, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame001.bmp",{0.46f,0.50f},7, 0x00FF00 },
			});
		pAnim->AddClip(L"idle", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame016.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame017.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame008.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame009.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame00A.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame00B.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame00C.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame00D.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame00E.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame00F.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame010.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame011.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame012.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame013.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame014.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame015.bmp",{0.46f,0.50f},3,0x00FF00 },
			});
		pAnim->AddClip(L"walk_loop", clip);

	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame007.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame018.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame019.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame189.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame18A.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame18B.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame18C.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame18D.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame18E.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame18F.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame190.bmp",{0.46f,0.50f},3,0x00FF00 },
			});
		pAnim->AddClip(L"walk_start", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame01C.bmp",{0.46f,0.50f},6,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame01D.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame01E.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame01F.bmp",{0.46f,0.50f},7,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame020.bmp",{0.46f,0.50f},4,0x00FF00 },

			});
		pAnim->AddClip(L"walk_turn", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame021.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame022.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame023.bmp",{0.46f,0.50f},8,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame024.bmp",{0.46f,0.50f},7,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame025.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame026.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame001.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame002.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame003.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame004.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame005.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame006.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame007.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame006.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame005.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame004.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame003.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame002.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame001.bmp",{0.46f,0.50f},5,0x00FF00 },
			});
		pAnim->AddClip(L"walk_stop", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame021.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame022.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame023.bmp",{0.46f,0.50f},8,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame024.bmp",{0.46f,0.50f},7,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame025.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame026.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame001.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame002.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame003.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame004.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame005.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame006.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame007.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame006.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame005.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame004.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame003.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame002.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame001.bmp",{0.46f,0.50f},5,0x00FF00 },
			});
		pAnim->AddClip(L"walk_stop", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame039.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame03A.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame03B.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame03C.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame03D.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame03E.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame03F.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame040.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame041.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame042.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame043.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame044.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame045.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame045.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame046.bmp",{0.46f,0.50f},6,0x00FF00 },

			});
		pAnim->AddClip(L"crouch_start", clip);
	}


	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame047.bmp",{0.46f,0.50f},31,0x00FF00 },

			});
		pAnim->AddClip(L"crouch_loop", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame048.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame049.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame04A.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame018.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame003.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame004.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame005.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame006.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame007.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame006.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame005.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame004.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame003.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame002.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame001.bmp",{0.46f,0.50f},5,0x00FF00 },

			});
		pAnim->AddClip(L"crouch_stop", clip);
	}



	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame04B.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame04C.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame04B.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame04C.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame04B.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame04D.bmp",{0.46f,0.50f},17,0x00FF00 },
			});
		pAnim->AddClip(L"jump_straight", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame028.bmp",{0.46f,0.50f},8,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame029.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame02A.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame029.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame02A.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame029.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame02A.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame029.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame02A.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame029.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame02A.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame029.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame02A.bmp",{0.46f,0.50f},4,0x00FF00 },
			});
		pAnim->AddClip(L"jump_diagonal", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame02B.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame184.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame02C.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame185.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame02D.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame02E.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame02F.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame030.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame02F.bmp",{0.46f,0.50f},8,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame030.bmp",{0.46f,0.50f},7,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame02F.bmp",{0.46f,0.50f},7,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame030.bmp",{0.46f,0.50f},7,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame02F.bmp",{0.46f,0.50f},7,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame030.bmp",{0.46f,0.50f},7,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame02F.bmp",{0.46f,0.50f},7,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame030.bmp",{0.46f,0.50f},7,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame02F.bmp",{0.46f,0.50f},6,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame030.bmp",{0.46f,0.50f},6,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame02F.bmp",{0.46f,0.50f},6,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame030.bmp",{0.46f,0.50f},6,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame02F.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame030.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame02F.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame030.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame02F.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame030.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame02F.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame030.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame02F.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame030.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame02F.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame030.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame02F.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame030.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame02F.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame030.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame02F.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame030.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame02F.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame030.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame02F.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame030.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame02F.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame030.bmp",{0.46f,0.50f},1,0x00FF00 },
			});
		pAnim->AddClip(L"fall", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame033.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame034.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame035.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame036.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame037.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame038.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame001.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame002.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame003.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame004.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame005.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame006.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame007.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame006.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame005.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame004.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame003.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame002.bmp",{0.46f,0.50f},6,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame001.bmp",{0.46f,0.50f},5,0x00FF00 },
			});
		pAnim->AddClip(L"land_small_height", clip);
	}

	

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame02E.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame186.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame031.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame032.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame033.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame034.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame035.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame036.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame037.bmp",{0.46f,0.50f},6,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame038.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame001.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame002.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame003.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame004.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame005.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame006.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame007.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame006.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame005.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame004.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame003.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame002.bmp",{0.46f,0.50f},6,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame001.bmp",{0.46f,0.50f},6,0x00FF00 },
			});
		pAnim->AddClip(L"land_medium_height", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame03B.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame03C.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame03D.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame03E.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame03F.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame040.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame042.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame043.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame044.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame045.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame045.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame046.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame047.bmp",{0.46f,0.50f},4,0x00FF00 },
			});
		pAnim->AddClip(L"land_large_height", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame0C2.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C3.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C4.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C5.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C6.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C7.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C8.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C9.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0CA.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame03F.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame042.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame044.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame046.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame047.bmp",{0.46f,0.50f},3,0x00FF00 },
			});
		pAnim->AddClip(L"land_diagonal", clip);
	}




	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame02E.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame186.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame031.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame00C.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame00D.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame00E.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame00F.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame010.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame011.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame012.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame013.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame014.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame015.bmp",{0.46f,0.50f},3,0x00FF00 },
			});
		pAnim->AddClip(L"land_while_move", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame06B.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame06C.bmp",{0.46f,0.50f},6,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame06B.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame024.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame025.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame026.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame001.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame002.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame003.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame004.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame005.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame006.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame007.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame006.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame005.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame004.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame003.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame002.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame001.bmp",{0.46f,0.50f},2,0x00FF00 },
			});
		pAnim->AddClip(L"dash_back", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame06D.bmp",{0.46f,0.50f},3,0x00FF00  },
				{ L"resource\\player\\p_soma\\frame06E.bmp",{0.46f,0.50f},3,0x00FF00  },
				{ L"resource\\player\\p_soma\\frame06F.bmp",{0.46f,0.50f},3,0x00FF00  },
				{ L"resource\\player\\p_soma\\frame070.bmp",{0.46f,0.50f},3,0x00FF00  },
				{ L"resource\\player\\p_soma\\frame071.bmp",{0.46f,0.50f},10,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame072.bmp",{0.46f,0.50f},10,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame073.bmp",{0.46f,0.50f},10,0x00FF00 },
			});
		pAnim->AddClip(L"slide_on_flat_ground", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame10C.bmp",{0.46f,0.50f},2,0x00FF00  },
				{ L"resource\\player\\p_soma\\frame112.bmp",{0.46f,0.50f},2,0x00FF00  },
				{ L"resource\\player\\p_soma\\frame10E.bmp",{0.46f,0.50f},5,0x00FF00  },
				{ L"resource\\player\\p_soma\\frame10F.bmp",{0.46f,0.50f},3,0x00FF00  },
				{ L"resource\\player\\p_soma\\frame112.bmp",{0.46f,0.50f},2,0x00FF00  },
				{ L"resource\\player\\p_soma\\frame110.bmp",{0.46f,0.50f},6,0x00FF00  },
				{ L"resource\\player\\p_soma\\frame111.bmp",{0.46f,0.50f},15,0x00FF00 },
			});
		pAnim->AddClip(L"sword_attack_stand", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame10C.bmp",{0.46f,0.50f},11,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame10D.bmp",{0.46f,0.50f},9, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame057.bmp",{0.46f,0.50f},4, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame007.bmp",{0.46f,0.50f},2, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame006.bmp",{0.46f,0.50f},2, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame005.bmp",{0.46f,0.50f},2, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame004.bmp",{0.46f,0.50f},2, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame003.bmp",{0.46f,0.50f},2, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame002.bmp",{0.46f,0.50f},2, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame001.bmp",{0.46f,0.50f},2, 0x00FF00 },
			});
		pAnim->AddClip(L"sword_attack_stand_end", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame117.bmp",{0.46f,0.50f},2, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame119.bmp",{0.46f,0.50f},2, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame118.bmp",{0.46f,0.50f},5, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame118.bmp",{0.46f,0.50f},3, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame119.bmp",{0.46f,0.50f},2, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame117.bmp",{0.46f,0.50f},6, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame11B.bmp",{0.46f,0.50f},15,0x00FF00 },
			});
		pAnim->AddClip(L"sword_attack_crouch", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame11A.bmp",{0.46f,0.50f},20,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame05F.bmp",{0.46f,0.50f},9,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame060.bmp",{0.46f,0.50f},8,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame047.bmp",{0.46f,0.50f},8,0x00FF00 },
			});
		pAnim->AddClip(L"sword_attack_crouch_end", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame115.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame113.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame116.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame114.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame113.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame115.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame115.bmp",{0.46f,0.50f},19,0x00FF00 },
			});
		pAnim->AddClip(L"sword_attack_air", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\weapon\\sword\\frame000.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\weapon\\sword\\frame001.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\weapon\\sword\\frame002.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\weapon\\sword\\frame003.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\weapon\\sword\\frame000.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\weapon\\sword\\frame004.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\weapon\\sword\\frame005.bmp",{0.46f,0.50f},3,0x00FF00 },
			});
		pAnim->AddClip(L"sword_effects", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame0A2.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0A1.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0A2.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0A1.bmp",{0.46f,0.50f},8,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame024.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame025.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame026.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame001.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame002.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame003.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame004.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame005.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame006.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame007.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame006.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame005.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame004.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame003.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame002.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame001.bmp",{0.46f,0.50f},2,0x00FF00 },
			});
		pAnim->AddClip(L"hit_in_the_groud_front", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame0B8.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0B9.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0BA.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0BB.bmp",{0.46f,0.50f},16,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0BA.bmp",{0.46f,0.50f},3,0x00FF00 },
			});
		pAnim->AddClip(L"hit_from_behind", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame0B6.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0B7.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0B6.bmp",{0.46f,0.50f},13,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame05F.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame060.bmp",{0.46f,0.50f},4,0x00FF00 },
			});
		pAnim->AddClip(L"hit_while_crouch", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame0BC.bmp",{0.46f,0.50f},7,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0CB.bmp",{0.46f,0.50f},7,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0BC.bmp",{0.46f,0.50f},7,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0CB.bmp",{0.46f,0.50f},7,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0BC.bmp",{0.46f,0.50f},8,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0CB.bmp",{0.46f,0.50f},8,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0BC.bmp",{0.46f,0.50f},9,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0CB.bmp",{0.46f,0.50f},9,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0BC.bmp",{0.46f,0.50f},10,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0CB.bmp",{0.46f,0.50f},10,0x00FF00 },
			});
		pAnim->AddClip(L"hit_in_the_air_front", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame0B8.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0B9.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0BC.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0BD.bmp",{0.46f,0.50f},6,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0BE.bmp",{0.46f,0.50f},6,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0BF.bmp",{0.46f,0.50f},6,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C0.bmp",{0.46f,0.50f},8,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C1.bmp",{0.46f,0.50f},8,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C0.bmp",{0.46f,0.50f},6,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C1.bmp",{0.46f,0.50f},6,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C0.bmp",{0.46f,0.50f},8,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C1.bmp",{0.46f,0.50f},8,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C0.bmp",{0.46f,0.50f},10,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C1.bmp",{0.46f,0.50f},10,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C0.bmp",{0.46f,0.50f},12,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C1.bmp",{0.46f,0.50f},12,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C0.bmp",{0.46f,0.50f},14,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C1.bmp",{0.46f,0.50f},14,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C0.bmp",{0.46f,0.50f},16,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C1.bmp",{0.46f,0.50f},16,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C0.bmp",{0.46f,0.50f},16,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C1.bmp",{0.46f,0.50f},16,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C0.bmp",{0.46f,0.50f},16,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C1.bmp",{0.46f,0.50f},16,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C0.bmp",{0.46f,0.50f},16,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C1.bmp",{0.46f,0.50f},16,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C0.bmp",{0.46f,0.50f},19,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C1.bmp",{0.46f,0.50f},19,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C0.bmp",{0.46f,0.50f},19,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C1.bmp",{0.46f,0.50f},19,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C0.bmp",{0.46f,0.50f},19,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C1.bmp",{0.46f,0.50f},19,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C0.bmp",{0.46f,0.50f},19,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0C1.bmp",{0.46f,0.50f},19,0x00FF00 },
			});
		pAnim->AddClip(L"dead", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame07D.bmp",{0.46f,0.50f},7,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0D8.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame07E.bmp",{0.46f,0.50f},7,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame07F.bmp",{0.46f,0.50f},6,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame051.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame052.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame053.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame054.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame055.bmp",{0.46f,0.50f},2,0x00FF00 },
			});
		pAnim->AddClip(L"using_skill_on_ground", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame062.bmp",{0.46f,0.50f},7,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame063.bmp",{0.46f,0.50f},16,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame064.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame065.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame066.bmp",{0.46f,0.50f},5,0x00FF00 },
			});
		pAnim->AddClip(L"using_skill_in_air", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame080.bmp",{0.46f,0.50f},8,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame081.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame082.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame083.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame084.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame085.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame086.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame087.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame088.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame089.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame08A.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame08B.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame08C.bmp",{0.46f,0.50f},5,0x00FF00 },
			});
		pAnim->AddClip(L"great_sword_attack", clip);
	}

	//巨剑没有类似end的收招，而是将收招动作也加入攻击帧
	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame080.bmp",{0.46f,0.50f},9,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame081.bmp",{0.46f,0.50f},6,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame082.bmp",{0.46f,0.50f},6,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame083.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame084.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame085.bmp",{0.46f,0.50f},6,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame086.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame087.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame088.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame089.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame08A.bmp",{0.46f,0.50f},6,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame08B.bmp",{0.46f,0.50f},6,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame08C.bmp",{0.46f,0.50f},6,0x00FF00 },
			});
		pAnim->AddClip(L"great_sword_attack_stand", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame095.bmp",{0.46f,0.50f},9,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame096.bmp",{0.46f,0.50f},6,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame097.bmp",{0.46f,0.50f},6,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame098.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame099.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame09A.bmp",{0.46f,0.50f},6,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame09B.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame09C.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame09D.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame09E.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame09F.bmp",{0.46f,0.50f},6,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame08A.bmp",{0.46f,0.50f},6,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0A0.bmp",{0.46f,0.50f},6,0x00FF00 },
			});
		pAnim->AddClip(L"great_sword_attack_crouch", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame08D.bmp",{0.46f,0.50f},9,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame08E.bmp",{0.46f,0.50f},6,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame08F.bmp",{0.46f,0.50f},6,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame090.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame091.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame091.bmp",{0.46f,0.50f},6,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame093.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame092.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame093.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame092.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame093.bmp",{0.46f,0.50f},6,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame094.bmp",{0.46f,0.50f},6,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame094.bmp",{0.46f,0.50f},6,0x00FF00 },
			});
		pAnim->AddClip(L"great_sword_attack_air", clip);
	}


	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame0FF.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame100.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame101.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0FF.bmp",{0.46f,0.50f},2,0x00FF00 },
			});
		pAnim->AddClip(L"punch_attack_stand", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame0FF.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame102.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame103.bmp",{0.46f,0.50f},17,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame104.bmp",{0.46f,0.50f},6,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame001.bmp",{0.46f,0.50f},2, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame002.bmp",{0.46f,0.50f},2, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame003.bmp",{0.46f,0.50f},2, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame004.bmp",{0.46f,0.50f},2, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame005.bmp",{0.46f,0.50f},2, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame006.bmp",{0.46f,0.50f},2, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame007.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame006.bmp",{0.46f,0.50f},3, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame005.bmp",{0.46f,0.50f},3, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame004.bmp",{0.46f,0.50f},4, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame003.bmp",{0.46f,0.50f},4, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame002.bmp",{0.46f,0.50f},5, 0x00FF00 },
				{ L"resource\\player\\p_soma\\frame001.bmp",{0.46f,0.50f},5, 0x00FF00 },
			});
		pAnim->AddClip(L"punch_attack_stand_end", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame105.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame106.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame107.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame105.bmp",{0.46f,0.50f},2,0x00FF00 },
			});
		pAnim->AddClip(L"punch_attack_crouch", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame105.bmp",{0.46f,0.50f},7,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame108.bmp",{0.46f,0.50f},28,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame05F.bmp",{0.46f,0.50f},9,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame060.bmp",{0.46f,0.50f},8,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame047.bmp",{0.46f,0.50f},8,0x00FF00 },
			});
		pAnim->AddClip(L"punch_attack_crouch_end", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame109.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame10A.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame10B.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame109.bmp",{0.46f,0.50f},2,0x00FF00 },

			});
		pAnim->AddClip(L"punch_attack_air", clip);
	}
	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame067.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame068.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame069.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame06A.bmp",{0.46f,0.50f},2,0x00FF00 },

			});
		clip.m_isLoop = true;
		pAnim->AddClip(L"use_black_panther", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame0A4.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0A5.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0A6.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0A7.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0A8.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0A9.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0AA.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0AB.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0AC.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0AD.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0AE.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0AF.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0B0.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0B1.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0B2.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0B3.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0A6.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0A5.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0B4.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0B5.bmp",{0.46f,0.50f},1,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0A4.bmp",{0.46f,0.50f},1,0x00FF00 },
			
			});
		clip.m_isLoop = false;
		pAnim->AddClip(L"double_jump", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame0DC.bmp",{0.46f,0.50f},60,0x00FF00 },
			});
		clip.m_isLoop = true;
		pAnim->AddClip(L"air_strike_diagonal", clip);
	}

	{
		GamePlay::AnimationClip clip;
		clip.AddFrames({
				{ L"resource\\player\\p_soma\\frame0DB.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DD.bmp",{0.46f,0.50f},5,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DB.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DD.bmp",{0.46f,0.50f},4,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DB.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DD.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DB.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DD.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DB.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DD.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DB.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DD.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DB.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DD.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DB.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DD.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DB.bmp",{0.46f,0.50f},3,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DD.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DB.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DD.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DB.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DD.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DB.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DD.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DB.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DD.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DB.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DD.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DB.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DD.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DB.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DD.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DB.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DD.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DB.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DD.bmp",{0.46f,0.50f},2,0x00FF00 },
				{ L"resource\\player\\p_soma\\frame0DB.bmp",{0.46f,0.50f},2,0x00FF00 },
			});
		clip.m_isLoop = true;
		pAnim->AddClip(L"air_strike_straight", clip);
	}



	pAnim->Play(L"idle");
}
void Soma::initFSM()
{
	GamePlay::FSM* fsm = AddComponent<GamePlay::FSM>();

	{
		Soma_Idle* state = new Soma_Idle();
		state->SetOwner(this);
		fsm->RegisterState(L"idle", state);
	}

	{
		Soma_Walk* state = new Soma_Walk();
		state->SetOwner(this);
		fsm->RegisterState(L"walk", state);
	}

	{
		Soma_Jump* state = new Soma_Jump();
		state->SetOwner(this);
		fsm->RegisterState(L"jump", state);
	}

	{
		Soma_DoubleJump* state = new Soma_DoubleJump();
		state->SetOwner(this);
		fsm->RegisterState(L"double_jump", state);
	}

	{
		Soma_Fall* state = new Soma_Fall();
		state->SetOwner(this);
		fsm->RegisterState(L"fall", state);
	}

	{
		Soma_Crouch* state = new Soma_Crouch();
		state->SetOwner(this);
		fsm->RegisterState(L"crouch", state);
	}

	{
		Soma_Dash_Back* state = new Soma_Dash_Back();
		state->SetOwner(this);
		fsm->RegisterState(L"dash_back", state);
	}

	{
		Soma_Slide_Flat* state = new Soma_Slide_Flat();
		state->SetOwner(this);
		fsm->RegisterState(L"slide_flat", state);
	}

	{
		Soma_Attack_Stand* state = new Soma_Attack_Stand();
		state->SetOwner(this);
		fsm->RegisterState(L"attack_stand", state);
	}

	{
		Soma_Attack_Crouch* state = new Soma_Attack_Crouch();
		state->SetOwner(this);
		fsm->RegisterState(L"attack_crouch", state);
	}

	{
		Soma_Attack_Air* state = new Soma_Attack_Air();
		state->SetOwner(this);
		fsm->RegisterState(L"attack_air", state);
	}

	{
		Soma_AxeBullet_Ground* state = new Soma_AxeBullet_Ground();
		state->SetOwner(this);
		fsm->RegisterState(L"using_axe_bullet_on_ground", state);
	}

	{
		Soma_AxeBullet_Air* state = new Soma_AxeBullet_Air();
		state->SetOwner(this);
		fsm->RegisterState(L"using_axe_bullet_in_air", state);
	}

	{
		Soma_Hit_Stand* state = new Soma_Hit_Stand();
		state->SetOwner(this);
		fsm->RegisterState(L"hit_stand", state);
	}

	{
		Soma_Hit_Air* state = new Soma_Hit_Air();
		state->SetOwner(this);
		fsm->RegisterState(L"hit_air", state);
	}

	{
		Soma_Land* state = new Soma_Land();
		state->SetOwner(this);
		fsm->RegisterState(L"land", state);
	}

	{
		Soma_Great_Sword_Attack_Air* state = new Soma_Great_Sword_Attack_Air();
		state->SetOwner(this);
		fsm->RegisterState(L"great_sword_attack_air", state);
	}

	{
		Soma_Great_Sword_Attack_Crouch* state = new Soma_Great_Sword_Attack_Crouch();
		state->SetOwner(this);
		fsm->RegisterState(L"great_sword_attack_crouch", state);
	}

	{
		Soma_Great_Sword_Attack_Stand* state = new Soma_Great_Sword_Attack_Stand();
		state->SetOwner(this);
		fsm->RegisterState(L"great_sword_attack_stand", state);
	}

	{
		Soma_Great_Sword_Supper_Attack* state = new Soma_Great_Sword_Supper_Attack();
		state->SetOwner(this);
		fsm->RegisterState(L"great_sword_supper_attack", state);
	}

	{
		Soma_Sword_Supper_Attack* state = new Soma_Sword_Supper_Attack();
		state->SetOwner(this);
		fsm->RegisterState(L"sword_supper_attack", state);
	}

	{
		Soma_Punch_Stand* state = new Soma_Punch_Stand();
		state->SetOwner(this);
		fsm->RegisterState(L"punch_attack_stand", state);
	}

	{
		Soma_Punch_Crouch* state = new Soma_Punch_Crouch();
		state->SetOwner(this);
		fsm->RegisterState(L"punch_attack_crouch", state);
	}

	{
		Soma_Punch_Air* state = new Soma_Punch_Air();
		state->SetOwner(this);
		fsm->RegisterState(L"punch_attack_air", state);
	}

	{
		Soma_BlackPanther* state = new Soma_BlackPanther();
		state->SetOwner(this);
		fsm->RegisterState(L"black_panther", state);
	}

	{
		Soma_Strike_Air* state = new Soma_Strike_Air();
		state->SetOwner(this);
		fsm->RegisterState(L"strike_air", state);
	}


	fsm->RegisterState(L"dead", new Soma_Dead());
	fsm->InitState(L"idle");
}

void Soma::onCollision(GamePlay::Collider* c)
{
	MathType::RectF selfRect = mBodyCollider->GetComponent<GamePlay::BoxCollider>()->CollitionAABBRect();
	MathType::RectF otherRect = c->CollitionAABBRect();
	MathType::vec2f curPos = transform().position();
	MathType::vec2f newPos = curPos;
	MathType::RectF overlapRect;
	if (!selfRect.Overlap(otherRect, &overlapRect)) { return; }

	float overlapW = overlapRect.Width();
	float overlapH = overlapRect.Height();

	if (c->gameobject()->layer() == GamePlay::Enemy || c->gameobject()->layer() == GamePlay::EnemyAtk)
	{
		if (IsInvulnerable()) return;

		Hit(1);
		float curScaleX = transform().localScale().x;
		m_attributes.is_hit_back = (curScaleX > 0 && selfRect.CenterX() > otherRect.CenterX()) ||
			(curScaleX < 0 && selfRect.CenterX() < otherRect.CenterX());

		return;
	}
	else if (c->gameobject()->layer() == GamePlay::Obstacle)
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
				mOnObstacle = true;
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
		if (c == m_ignoredCollider) { return; }
		if (m_attributes.curjp > 0) { return; }

		if (m_attributes.last_bottom <= otherRect.top && selfRect.bottom > otherRect.top)
		{
			newPos.y = curPos.y - overlapH;
			mOnGround = true;
			mOnPlatform = true;
			m_currentPlatformCollider = c;
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
			mOnEdgePlatform = true;
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

		mOnGround = true;
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

		mOnGround = true;
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
