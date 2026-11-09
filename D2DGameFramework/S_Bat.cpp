#include "S_Bat.h"
#include "GameTool.h"
#include "Player.h"
#include "Enemy.h"
#include "./GamePlay/ChildObject.h"
#include "./GamePlay/GameObject.h"
#include "./GamePlay/CollisionMatrix.h"
#include "./GamePlay/Debug.h"
#include <unordered_set>
#include <cmath>

void S_Bat::Init()
{
	mSoulId = 12;
	mInputType = 1; //触发类型： 1-开关类型 2-持续按压
	mRunType = 3;   //运行类型: 1-不需要接管角色状态（附加效果） 2-临时角色状态（类似枪兵旋转技能）3-使魔
	mLayer = GamePlay::GameLayer::Layer_3;
	int order = 999;

	GamePlay::AnimationClip clip;
	clip.m_isLoop = true;
	clip.AddFrames({
		{ L"resource\\skills\\p_bat\\frame000.bmp",{0.5f, 0.5f}, 4, 0x00FF00 },
		{ L"resource\\skills\\p_bat\\frame001.bmp",{0.5f, 0.5f}, 1, 0x00FF00 },
		{ L"resource\\skills\\p_bat\\frame002.bmp",{0.5f, 0.5f}, 1, 0x00FF00 },
		{ L"resource\\skills\\p_bat\\frame009.bmp",{0.5f, 0.5f}, 1, 0x00FF00 },
		{ L"resource\\skills\\p_bat\\frame003.bmp",{0.5f, 0.5f}, 1, 0x00FF00 },
		{ L"resource\\skills\\p_bat\\frame004.bmp",{0.5f, 0.5f}, 1, 0x00FF00 },
		{ L"resource\\skills\\p_bat\\frame005.bmp",{0.5f, 0.5f}, 4, 0x00FF00 },
		{ L"resource\\skills\\p_bat\\frame006.bmp",{0.5f, 0.5f}, 2, 0x00FF00 },
		{ L"resource\\skills\\p_bat\\frame007.bmp",{0.5f, 0.5f}, 2, 0x00FF00 },
		{ L"resource\\skills\\p_bat\\frame008.bmp",{0.5f, 0.5f}, 2, 0x00FF00 },
		{ L"resource\\skills\\p_bat\\frame009.bmp",{0.5f, 0.5f}, 1, 0x00FF00 },
		{ L"resource\\skills\\p_bat\\frame000.bmp",{0.5f, 0.5f}, 2, 0x00FF00 },
		{ L"resource\\skills\\p_bat\\frame000.bmp",{0.5f, 0.5f}, 2, 0x00FF00 }
		});
	a1 = GT::PlayAnimation(clip,L"idle",++order);
	a1->gameobject()->layer() = GamePlay::GameLayer::Layer_3;
	a1->SetOpcity(0);
	

	
	a2 = GT::PlayAnimation(clip, L"idle", ++order);
	a2->gameobject()->layer() = GamePlay::GameLayer::Layer_3;
	a2->SetOpcity(0);

	a3 = GT::PlayAnimation(clip, L"idle", ++order);
	a3->gameobject()->layer() = GamePlay::GameLayer::Layer_3;
	a3->SetOpcity(0);

	// 创建攻击碰撞器，使用 TimeInHit 模式，每次激活对每个敌人最多造成一次伤害
	mCollider1 = AttackCollider::Create();
	mCollider1->SetTarget(*a1->gameobject());
	mCollider1->SetOffset({ 0, 0 });
	mCollider1->SetTimeInHit(0.0f, 1);
	mCollider1->SetAttack(1);
	mCollider1->SetColliderSize(60, 60);

	mCollider2 = AttackCollider::Create();
	mCollider2->SetTarget(*a2->gameobject());
	mCollider2->SetOffset({ 0, 0 });
	mCollider2->SetTimeInHit(0.0f, 1);
	mCollider2->SetAttack(1);
	mCollider2->SetColliderSize(60, 60);

	mCollider3 = AttackCollider::Create();
	mCollider3->SetTarget(*a3->gameobject());
	mCollider3->SetOffset({ 0, 0 });
	mCollider3->SetTimeInHit(0.0f, 1);
	mCollider3->SetAttack(1);
	mCollider3->SetColliderSize(60, 60);
	ResetForRoom();

	SetActive(false);

}

void S_Bat::Start()
{
}

void S_Bat::ResetForRoom()
{
	if (!MainPlayer) return;
	const bool facingLeft = MainPlayer->transform().localScale().x < 0;
	curCen = MainPlayer->transform().position() + MathType::vec2f(facingLeft ? 60.0f : -60.0f, -100.0f);
	rotationA = 0;
	atkTimer = 0;
	curAtkObj = 0;
	a1_isAtkFinish = a2_isAtkFinish = a3_isAtkFinish = true;
	a1_isReturning = a2_isReturning = a3_isReturning = false;
	GamePlay::Animator* followers[] = {a1, a2, a3};
	AttackCollider* attacks[] = {mCollider1, mCollider2, mCollider3};
	MathType::vec2f* targets[] = {&a1_targetPos, &a2_targetPos, &a3_targetPos};
	for (int i = 0; i < 3; ++i)
	{
		const float angle = D2R(120.0f * i);
		const MathType::vec2f position = curCen + MathType::vec2f(std::cos(angle) * 60.0f, std::sin(angle) * 60.0f);
		*targets[i] = position;
		if (followers[i])
		{
			followers[i]->transform()->SetPosition(position);
			followers[i]->transform()->SetScale({facingLeft ? 1.0f : -1.0f, 1.0f});
			followers[i]->CaclRenderMatrix();
		}
		if (attacks[i])
		{
			attacks[i]->Deactivate();
			attacks[i]->transform()->SetPosition(position);
			attacks[i]->UpdateCollider();
		}
	}
}

void S_Bat::Update(float deltaTime)
{
	if (MainPlayer == nullptr) 
	{
		return; 
	}
	


	float curOp = a1->Opacity();
	MathType::vec2f curPos = MainPlayer->transform().position();
	bool openedThisFrame = false;
	if (GT::KeyJustDown('K'))
	{
		if (isOpen) isOpen = false;
		else
		{
			ResetStaminaUsage();
			isOpen = true;
			openedThisFrame = true;
		}
	}
	if (isOpen && !TrySustain(deltaTime)) isOpen = false;
	if (isOpen && openedThisFrame)
	{
		ResetForRoom();
		GT::PlayVoice(SE_V_SOMA_KOYI);
	}
	if (isOpen)
	{
		//设置镜像
		bool s = MainPlayer->transform().localScale().x < 0;
		MathType::vec2f curScale(s ? 1 : -1, 1);
		a1->transform()->SetScale(curScale);
		a2->transform()->SetScale(curScale);
		a3->transform()->SetScale(curScale);
		//透明度递增 + 钳制
		float dop = curOp + deltaTime;
		curOp = (dop > 1) ? 1 : dop;

		a1->SetOpcity(curOp);
		a2->SetOpcity(curOp);
		a3->SetOpcity(curOp);
		//平滑跟随 + 到达目标点做圆周运动
		//计算悬挂点
		MathType::vec2f hangPos = curPos;
		hangPos.y -= 100.0f;
		hangPos.x += s ? 60.0f : -60.0f;
		//更新悬挂点
		float t = deltaTime * 3;
		curCen = curCen.Lerp(hangPos, t);
		
		//分别计算每只蝙蝠的位置
		float x, y;
		rotationA += s ? -deltaTime*4.0f : deltaTime * 4.0f;

		// 攻击计时器：寻找最近敌人并分配给空闲的蝙蝠
		atkTimer += deltaTime;
		if (atkTimer >= atkCD)
		{
			// 获取当前窗口可见区域
			MathType::RectF viewRect = MainCamera->AABBRect();

			// 遍历所有存活敌人，找到窗口范围内距离最近的
			Enemy* nearestEnemy = nullptr;
			float minDist = 999999.0f;
			std::unordered_set<Enemy*> seen;

			for (auto* collider : GameObjectMgr.ColliderObjects())
			{
				if (collider->gameobject()->layer() != GamePlay::GameLayer::Layer_13) continue;
				if (!collider->gameobject()->Active()) continue;

				GamePlay::ChildObject* cp = dynamic_cast<GamePlay::ChildObject*>(collider->gameobject());
				if (!cp) continue;

				Enemy* enemy = cp->GetOwner<Enemy>();
				if (!enemy || !enemy->Active()) continue;
				if (seen.count(enemy)) continue;
				seen.insert(enemy);

				MathType::vec2f enemyPos = enemy->transform().position();
				if (!viewRect.Contains(enemyPos.x, enemyPos.y)) continue;

				float dist = curPos.SqrDistance(enemyPos);
				if (dist < minDist)
				{
					minDist = dist;
					nearestEnemy = enemy;
				}
			}

			if (nearestEnemy != nullptr)
			{
				float curAtk = rand() % 3 + 2;
				MathType::vec2f enemyPos = nearestEnemy->transform().position();
				switch (curAtkObj)
				{
				case 0:
					if (a1_isAtkFinish)
					{
						a1_targetPos = enemyPos;
						a1_isAtkFinish = false;
						mCollider1->SetAttack(curAtk);
						mCollider1->Activate();
						
					}
					break;
				case 1:
					if (a2_isAtkFinish)
					{
						a2_targetPos = enemyPos;
						a2_isAtkFinish = false;
						mCollider2->SetAttack(curAtk);
						mCollider2->Activate();
					}
					break;
				case 2:
					if (a3_isAtkFinish)
					{
						a3_targetPos = enemyPos;
						a3_isAtkFinish = false;
						mCollider3->SetAttack(curAtk);
						mCollider3->Activate();
					}
					break;
				}
			}
			curAtkObj = (curAtkObj + 1) % 3;
			atkTimer = 0.0f;
		}

		// --- a1 ---
		if (!a1_isAtkFinish)
		{
			MathType::vec2f a1Pos = a1->transform()->position();
			MathType::vec2f dir = a1_targetPos - a1Pos;
			float dist = a1Pos.Distance(a1_targetPos);
			if (dist < 15.0f)
			{
				a1->transform()->SetPosition(a1_targetPos);
				mCollider1->Deactivate();
				a1_isAtkFinish = true;
			}
			else
			{
				dir.Normalize();
				a1->transform()->SetPosition(a1Pos + dir * mAtkMoveSpeed * deltaTime);
			}
		}
		else
		{
			MathType::vec2f a1Pos = a1->transform()->position();
			x = curCen.x + cos(rotationA) * 60;
			y = curCen.y + sin(rotationA) * 60;
			MathType::vec2f lerped = a1Pos.Lerp({ x,y }, deltaTime * 5.0f);
			a1->transform()->SetPosition(lerped);
		}

		// --- a2 ---
		float degree = D2R(120);
		if (!a2_isAtkFinish)
		{
			MathType::vec2f a2Pos = a2->transform()->position();
			MathType::vec2f dir = a2_targetPos - a2Pos;
			float dist = a2Pos.Distance(a2_targetPos);
			if (dist < 15.0f)
			{
				a2->transform()->SetPosition(a2_targetPos);
				mCollider2->Deactivate();
				a2_isAtkFinish = true;
			}
			else
			{
				dir.Normalize();
				a2->transform()->SetPosition(a2Pos + dir * mAtkMoveSpeed * deltaTime);
			}
		}
		else
		{
			MathType::vec2f a2Pos = a2->transform()->position();
			x = curCen.x + cos(rotationA + degree) * 60;
			y = curCen.y + sin(rotationA + degree) * 60;
			MathType::vec2f lerped = a2Pos.Lerp({x,y}, deltaTime * 5.0f);
			a2->transform()->SetPosition(lerped);
		}

		// --- a3 ---
		if (!a3_isAtkFinish)
		{
			MathType::vec2f a3Pos = a3->transform()->position();
			MathType::vec2f dir = a3_targetPos - a3Pos;
			float dist = a3Pos.Distance(a3_targetPos);
			if (dist < 15.0f)
			{
				a3->transform()->SetPosition(a3_targetPos);
				mCollider3->Deactivate();
				a3_isAtkFinish = true;
			}
			else
			{
				dir.Normalize();
				a3->transform()->SetPosition(a3Pos + dir * mAtkMoveSpeed * deltaTime);
			}
		}
		else
		{
			MathType::vec2f a3Pos = a3->transform()->position();
			x = curCen.x + cos(rotationA + degree + degree) * 60;
			y = curCen.y + sin(rotationA + degree + degree) * 60;
			MathType::vec2f lerped = a3Pos.Lerp({x,y}, deltaTime * 5.0f);
			a3->transform()->SetPosition(lerped);
		}

		// 更新攻击碰撞器
		mCollider1->Update(deltaTime);
		mCollider2->Update(deltaTime);
		mCollider3->Update(deltaTime);

	}
	else
	{
		//透明度递减
		float dop = curOp - deltaTime;
		curOp = (dop < 0) ? 0 : dop;
		a1->SetOpcity(curOp);
		a2->SetOpcity(curOp);
		a3->SetOpcity(curOp);
		//关闭碰撞
		mCollider1->Deactivate();
		mCollider2->Deactivate();
		mCollider3->Deactivate();
		a1_isAtkFinish = a2_isAtkFinish = a3_isAtkFinish = true;
		atkTimer = 0;
	}


	
}

void S_Bat::Awake()
{

}

void S_Bat::OnDestroy()
{
	if (a1) { a1->gameobject()->Destroy(); a1 = nullptr; }
	if (a2) { a2->gameobject()->Destroy(); a2 = nullptr; }
	if (a3) { a3->gameobject()->Destroy(); a3 = nullptr; }
	if (mCollider1) { mCollider1->gameobject()->Destroy(); mCollider1 = nullptr; }
	if (mCollider2) { mCollider2->gameobject()->Destroy(); mCollider2 = nullptr; }
	if (mCollider3) { mCollider3->gameobject()->Destroy(); mCollider3 = nullptr; }
}

void S_Bat::OnDebugRender()
{
	// 装备仍保留时，关闭召唤或体力耗尽也不显示使魔调试内容。
	if (!isOpen) return;

	auto drawBox = [](GamePlay::GameObject* obj, float w, float h, const ColorType::COLORF& color)
	{
		if (!obj) return;
		auto pos = obj->transform().position();
		float hw = w * 0.5f;
		float hh = h * 0.5f;
		Debug.DebugDrawLine(pos.x - hw, pos.y - hh, pos.x + hw, pos.y - hh, color);
		Debug.DebugDrawLine(pos.x + hw, pos.y - hh, pos.x + hw, pos.y + hh, color);
		Debug.DebugDrawLine(pos.x + hw, pos.y + hh, pos.x - hw, pos.y + hh, color);
		Debug.DebugDrawLine(pos.x - hw, pos.y + hh, pos.x - hw, pos.y - hh, color);
	};

	if (mCollider1)
		drawBox(mCollider1->gameobject(), mCollider1->Width(), mCollider1->Height(),
			mCollider1->IsActive() ? ColorType::COLORF::Red() : ColorType::COLORF::Yellow());
	if (mCollider2)
		drawBox(mCollider2->gameobject(), mCollider2->Width(), mCollider2->Height(),
			mCollider2->IsActive() ? ColorType::COLORF::Red() : ColorType::COLORF::Yellow());
	if (mCollider3)
		drawBox(mCollider3->gameobject(), mCollider3->Width(), mCollider3->Height(),
			mCollider3->IsActive() ? ColorType::COLORF::Red() : ColorType::COLORF::Yellow());

	if (isOpen)
	{
		auto drawCross = [](GamePlay::GameObject* obj, const ColorType::COLORF& color)
		{
			if (!obj) return;
			auto pos = obj->transform().position();
			const float r = 12.0f;
			Debug.DebugDrawLine(pos.x - r, pos.y, pos.x + r, pos.y, color);
			Debug.DebugDrawLine(pos.x, pos.y - r, pos.x, pos.y + r, color);
		};
		drawCross(a1->gameobject(), ColorType::COLORF::Green());
		drawCross(a2->gameobject(), ColorType::COLORF::Green());
		drawCross(a3->gameobject(), ColorType::COLORF::Green());
	}
}
