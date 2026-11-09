#include "AttackEffect.h"
#include "Soma.h"
#include "GameTool.h"
#include "./GamePlay/Collider2d.h"
#include "./GamePlay/ChildObject.h"
#include "Enemy.h"
#include "BackPack.h"
#include "Player.h"
#include "EffectsPool.h"
AttackEffect::AttackEffect()
{
	mLayer = GamePlay::GameLayer::Layer_5;
	mAnimator = AddComponent<GamePlay::Animator>();
	mAnimator->SetOpcity(0.7f);

	mSp = GT::CreateGameObject<GamePlay::GameObject>()->AddComponent<GamePlay::SpriteRender>();
	mSp->SetTexture(TexMgr.LoadTextureWithoutColor(L"resource\\weapon\\greatsword\\frame000.png", 0xffffff));
	mSp->SetAnchor(0.343f, 0.8929f);
	mSp->gameobject()->layer() = GamePlay::GameLayer::Layer_5;
	mSp->SetOrderInLayer(9999);

	mAttackCollider = AttackCollider::Create();

	initAnim();
	SetActive(false);
}

// ---- 动画初始化 ----

void AttackEffect::initAnim()
{
	GamePlay::AnimationClip clip;
	clip.AddFrames({
		{ L"resource\\weapon\\sword\\frame000.bmp", {0.46f, 0.50f}, 3, 0x00FF00 },
		{ L"resource\\weapon\\sword\\frame001.bmp", {0.46f, 0.50f}, 4, 0x00FF00 },
		{ L"resource\\weapon\\sword\\frame002.bmp", {0.46f, 0.50f}, 4, 0x00FF00 },
		{ L"resource\\weapon\\sword\\frame003.bmp", {0.46f, 0.50f}, 3, 0x00FF00 },
		{ L"resource\\weapon\\sword\\frame000.bmp", {0.46f, 0.50f}, 5, 0x00FF00 },
		{ L"resource\\weapon\\sword\\frame004.bmp", {0.46f, 0.50f}, 3, 0x00FF00 },
		{ L"resource\\weapon\\sword\\frame005.bmp", {0.46f, 0.50f}, 4, 0x00FF00 },
		});
	mAnimator->AddClip(L"sword_effects", clip);

	clip.m_frames.clear();
	clip.AddFrames({
		{ L"resource\\weapon\\greatsword\\frame001.png", {0.46f, 0.50f}, 2, 0xFFFFFF },
		{ L"resource\\weapon\\greatsword\\frame002.png", {0.46f, 0.50f}, 6, 0xFFFFFF },
		{ L"resource\\weapon\\greatsword\\frame003.png", {0.46f, 0.50f}, 2, 0xFFFFFF },
		{ L"resource\\weapon\\greatsword\\frame004.png", {0.46f, 0.50f}, 3, 0xFFFFFF },
		{ L"resource\\weapon\\greatsword\\frame005.png", {0.46f, 0.50f}, 3, 0xFFFFFF },
		{ L"resource\\weapon\\greatsword\\frame006.png", {0.46f, 0.50f}, 2, 0xFFFFFF },
		{ L"resource\\weapon\\greatsword\\frame007.png", {0.46f, 0.50f}, 4, 0xFFFFFF },
		{ L"resource\\weapon\\greatsword\\frame008.png", {0.46f, 0.50f}, 3, 0xFFFFFF },
		{ L"resource\\weapon\\greatsword\\frame009.png", {0.46f, 0.50f}, 2, 0xFFFFFF },
		{ L"resource\\weapon\\greatsword\\frame00A.png", {0.46f, 0.50f}, 8, 0xFFFFFF },
		{ L"resource\\weapon\\greatsword\\frame00B.png", {0.46f, 0.50f}, 6, 0xFFFFFF },
		{ L"resource\\weapon\\greatsword\\frame00C.png", {0.46f, 0.50f}, 6, 0xFFFFFF },
		{ L"resource\\weapon\\greatsword\\frame00D.png", {0.46f, 0.50f}, 2, 0xFFFFFF },
		{ L"resource\\weapon\\greatsword\\frame00E.png", {0.46f, 0.50f}, 2, 0xFFFFFF },
		{ L"resource\\weapon\\greatsword\\frame00F.png", {0.46f, 0.50f}, 4, 0xFFFFFF },
		{ L"resource\\weapon\\greatsword\\frame010.png", {0.46f, 0.50f}, 2, 0xFFFFFF },
		{ L"resource\\weapon\\greatsword\\frame011.png", {0.46f, 0.50f}, 2, 0xFFFFFF },
		});
	mAnimator->AddClip(L"great_sword_effects", clip);
}

// ---- 公共 API ----

void AttackEffect::SetOneHit(int triggerFrame)
{
	mHitMode = AttackCollider::HitMode::OneHit;
	mOneHitFrame = triggerFrame;
	mAttackCollider->SetOneHit();
}

void AttackEffect::SetTimeInHit(int startFrame, int endFrame, float hitInterval, int maxHitsPerEnemy)
{
	mHitMode = AttackCollider::HitMode::TimeInHit;
	mHitStartFrame = startFrame;
	mHitEndFrame = endFrame;
	mHitInterval = hitInterval;
	mMaxHitsPerEnemy = maxHitsPerEnemy;
	mAttackCollider->SetTimeInHit(hitInterval, maxHitsPerEnemy);
}

// ---- 激活 / 停用 ----

void AttackEffect::Activate(bool flipX,bool isSuperAttack)
{
	SetActive(true);
	mColliderInWindow = false;

	float sx = mOwner->transform().localScale().x;
	bool f = sx < 0;
	float x = f ? mOwner->transform().position().x - 70 : mOwner->transform().position().x + 70;
	float y = mOwner->transform().position().y - 30;
	transform().SetPosition({ x, y });
	transform().SetScale({ f ? -1.0f : 1.0f, 1.0f });

	auto* curWeapon = BackPack.GetEquippedWeapon();
	mAttackCollider->SetTarget(*mOwner);
	mAttackCollider->SetAttack(1, 1);
	std::wstring ef = L"";
	mAttackCollider->SetEffectKey(ef);

	if (curWeapon && curWeapon->id == 4)
	{
		mSp->SetOpcity(0);
		mAnimator->Play(L"great_sword_effects");
		mAnimator->SetOpcity(1);
		mAttackCollider->SetColliderSize(300, 200);
		SetTimeInHit(0, 44, 0.1f, 1);

		auto* curAcc = BackPack.GetEquippedAccessory();
		auto* curBody = BackPack.GetEquippedArmor();
		if (isSuperAttack)
		{
			mIsSuperAttack = true;
			float sx = mOwner->transform().localScale().x;
			bool f = sx < 0;
			mCurrentRotation = f ? 180.0f : -180.0f;
			mSp->transform()->SetRotation(mCurrentRotation);

			mAttackCollider->SetColliderSize(500, 200);
			SetTimeInHit(0, 44, 0.1f, 5);

			std::wstring ef = EFFECT_CLAIMH_SOLAIS;
			mAttackCollider->SetEffectKey(ef);
			
			
			mSp->SetOpcity(0.0f);
		}
	}
	else if (curWeapon && curWeapon->id == 3)
	{
		mSp->SetOpcity(0);
		mAnimator->Play(L"sword_effects");
		mAttackCollider->SetColliderSize(200, 50);
		SetOneHit(2);
	}
}

void AttackEffect::Deactivate()
{
	SetActive(false);
	if (mSp)
		mSp->SetOpcity(0);
	if (mIsSuperAttack)
		mIsSuperAttack = false;
	mAttackCollider->Deactivate();
	mColliderInWindow = false;
}

// ---- 调试渲染 ----

void AttackEffect::OnDebugRender()
{
	if (mAttackCollider && mAttackCollider->IsActive())
	{
		auto pos = mAttackCollider->gameobject()->transform().position();
		float w = mAttackCollider->Width();
		float h = mAttackCollider->Height();
		float halfW = w / 2;
		float halfH = h / 2;
		Debug.DebugDrawLine(pos.x - halfW, pos.y - halfH, pos.x + halfW, pos.y - halfH, ColorType::COLORF::Red());
		Debug.DebugDrawLine(pos.x - halfW, pos.y - halfH, pos.x - halfW, pos.y + halfH, ColorType::COLORF::Red());
		Debug.DebugDrawLine(pos.x + halfW, pos.y - halfH, pos.x + halfW, pos.y + halfH, ColorType::COLORF::Red());
		Debug.DebugDrawLine(pos.x - halfW, pos.y + halfH, pos.x + halfW, pos.y + halfH, ColorType::COLORF::Red());
	}
}

// ---- 主更新 ----

void AttackEffect::Update(float deltaTime)
{
	if (!mOwner) return;

	auto* curWeapon = BackPack.GetEquippedWeapon();
	auto* curAcc = BackPack.GetEquippedAccessory();
	auto* curBody = BackPack.GetEquippedArmor();
	if (!curWeapon) return;

	float offsetX, offsetY;
	GamePlay::FSM* fsm = mOwner->GetComponent<GamePlay::FSM>();

	mAttackCollider->SetAttack(curWeapon->p_atk);

	if (curWeapon->id == 4) // 大剑
	{
		mAnimator->SetOpcity(1.0f);
		offsetX = 33;
		offsetY = (fsm->GetCurrentState() == fsm->GetState(L"great_sword_attack_crouch")) ? -49 : -73;
		
		

		if (mIsSuperAttack)
		{
			float curAtk = curWeapon->p_atk / 5.0f +  (rand() % 6); 
			mAttackCollider->SetAttack(curAtk,3);
			mAnimator->SetOpcity(0.0f);
			float curOp = mSp->Opacity();
			curOp = (curOp + deltaTime * 2 > 1) ? 1 : curOp + deltaTime * 3;
			mSp->SetOpcity(curOp);

			float offx = 10;
			float offy = (fsm->GetCurrentState() == fsm->GetState(L"great_sword_attack_crouch")) ? -49 + 40 - 25 : -73 + 40 - 25;
			float sx2 = mOwner->transform().localScale().x;
			bool f2 = sx2 < 0;
			sx2 = f2 ? -1.0f : 1.0f;
			mCurrentRotation += sx2 * deltaTime * 580;

			if (!f2 && mCurrentRotation > 100.0f)
				mCurrentRotation = 100.0f;
			else if (f2 && mCurrentRotation < -100)
				mCurrentRotation = -100.0f;

			float x2 = f2 ? mOwner->transform().position().x - offx : mOwner->transform().position().x + offx;
			float y2 = mOwner->transform().position().y + offy;
			mSp->gameobject()->transform().SetRotation(mCurrentRotation);
			mSp->gameobject()->transform().SetPosition({ x2, y2 });
			mSp->gameobject()->transform().SetScale({ f2 ? -1.0f : 1.0f, 1.0f });
			mSp->gameobject()->_ComponentUpdate(0);
		}
	}
	else if (curWeapon->id == 3) // 普通剑
	{
		mAnimator->SetOpcity(0.7f);
		offsetX = 70;
		offsetY = (fsm->GetCurrentState() == fsm->GetState(L"attack_crouch")) ? 10 : -30;
	}
	else
	{
		return;
	}

	float sx = mOwner->transform().localScale().x;
	bool flip = sx < 0;
	float x = flip ? mOwner->transform().position().x - offsetX : mOwner->transform().position().x + offsetX;
	float y = mOwner->transform().position().y + offsetY;
	transform().SetPosition({ x, y });
	transform().SetScale({ flip ? -1.0f : 1.0f, 1.0f });

	// AttackCollider 跟随目标 + 碰撞检测
	mAttackCollider->SetOffset({ offsetX, offsetY });
	mAttackCollider->Update(deltaTime);

	// 帧窗口控制碰撞体启停
	int curFrame = mAnimator->GetCurrentFrameIndex();
	bool shouldActivate = false;

	if (mHitMode == AttackCollider::HitMode::OneHit)
		shouldActivate = (curFrame >= mOneHitFrame);
	else
		shouldActivate = (curFrame >= mHitStartFrame && curFrame < mHitEndFrame);

	if (shouldActivate && !mColliderInWindow)
	{
		mAttackCollider->Activate();
		mColliderInWindow = true;
	}
	else if (shouldActivate && mColliderInWindow && mAttackCollider->IsActive() && mHitMode == AttackCollider::HitMode::OneHit)
	{
		mAttackCollider->Deactivate();
		//mColliderInWindow = false;
	}
	else if (!shouldActivate && mColliderInWindow)
	{
		mAttackCollider->Deactivate();
		mColliderInWindow = false;
	}

	if (mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
		Deactivate();
}
