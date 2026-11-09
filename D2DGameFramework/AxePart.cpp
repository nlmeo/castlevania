#include "AxePart.h"
#include "GameTool.h"
#include "./GamePlay/Collider2d.h"
#include "./GamePlay/Animator.h"
#include "./HelpTool/RandomMaker.h"
#include "EffectsPool.h"
AxePart::AxePart()
{
	mLayer = GamePlay::GameLayer::Layer_5;
	mCollider = AddComponent<GamePlay::BoxCollider>();
	mCollider->SetColliderSize(8, 8);
	mAnimator = AddComponent<GamePlay::Animator>();
	mAnimator->SetOrderInLayer(GameObjectMgr.GameObjectCount());
}

void AxePart::Init(const wchar_t* frameKey, MathType::vec2f startPos, float direction)
{
	createAnim(frameKey);
	transform().SetPosition(startPos);

	// 溅射速度：统一沿攻击方向，横向 150~300，纵向 -350~-180（向上爆散）
	mVx = HelpTool::RandomFloat(150.0f, 300.0f) * direction;
	mVy = HelpTool::RandomFloat(-350.0f, -180.0f);

}

void AxePart::Update(float deltaTime)
{

	// 重力
	mVy += mGravity * deltaTime;

	// 移动
	MathType::vec2f delta = { mVx * deltaTime, mVy * deltaTime };
	transform().translate(delta);

	if (!mCollider) return;

	// 同步碰撞器 AABB
	mCollider->UpdateCollider();

	GamePlay::Collider& self = *mCollider;
	std::vector<GamePlay::Collider*> hits;
	if (GT::CollisionDetect(self, mGroundMask, hits))
	{
		for (GamePlay::Collider* c : hits)
		{
			onCollision(c);
		}
		if (mOnGround)
		{
			GT::PlaySE(SE_AXE_PART_LAND);
			switch (HelpTool::RandomInt(0, 2))
			{
			case 0:
				Effect::Spawn(EFFECT_DUST_01, transform().position() , false ,0.6f);
				break;
			case 1:
				Effect::Spawn(EFFECT_DUST_02, transform().position(),false, 0.6f);
				break;
			default:
				break;
			}
			Destroy();
		}
	}
}

void AxePart::createAnim(const wchar_t* frameKey)
{
	GamePlay::AnimationClip clip;
	clip.AddFrames({
		{ frameKey, {0.5f, 0.5f}, 60, 0x00FF00 },
		});
	mAnimator->AddClip(L"corpse_part", clip);
	mAnimator->Play(L"corpse_part");
}


void AxePart::onCollision(GamePlay::Collider* other)
{
	if (!other) return;

	GamePlay::GameLayer layer = other->gameobject()->layer();

	// 斜坡：采用点碰撞（脚底中心点采样），参考 Soma 的斜坡处理
	if (layer == GamePlay::LeftSlop || layer == GamePlay::RightSlop)
	{
		MathType::RectF selfRect = mCollider->CollitionAABBRect();
		MathType::RectF otherRect = other->CollitionAABBRect();
		MathType::vec2f ptInRect(selfRect.CenterX(), selfRect.bottom);
		if (ptInRect.x < otherRect.left || ptInRect.x > otherRect.right) return;

		float k = otherRect.Height() / otherRect.Width();
		float w = ptInRect.x - otherRect.left;
		float t_y = (layer == GamePlay::LeftSlop)
			? otherRect.bottom - (k * w)
			: otherRect.top + (k * w);

		// 脚底点仍在斜坡表面上方，尚未落地
		if (ptInRect.y < t_y) return;

		mOnGround = true;
		MathType::vec2f newPos = transform().position();
		newPos.y = t_y - (selfRect.Height() / 2.0f);
		transform().SetPosition(newPos);
		mCollider->UpdateCollider();
	}
	// 地形碰撞 → 落地（参考 Soma 的地形碰撞处理）
	else if (layer == GamePlay::Obstacle || layer == GamePlay::Platform ||
		layer == GamePlay::EdgePlatform)
	{
		MathType::RectF selfRect = mCollider->CollitionAABBRect();
		MathType::RectF otherRect = other->CollitionAABBRect();
		MathType::RectF overlapRect;
		if (!selfRect.Overlap(otherRect, &overlapRect)) return;

		float overlapW = overlapRect.Width();
		float overlapH = overlapRect.Height();
		MathType::vec2f curPos = transform().position();
		MathType::vec2f newPos = curPos;

		mOnGround = true;
		if (overlapW > overlapH)
		{
			// 垂直方向重叠为主
			if (selfRect.CenterY() < otherRect.CenterY())
			{
				// 掉落物在地形上方：落地
				newPos.y = curPos.y - overlapH;
			}
			else
			{
				// 掉落物在地形下方：撞顶（正常不会发生）
				newPos.y = curPos.y + overlapH;
			}
		}
		else
		{
			// 水平方向重叠为主：侧面推出
			if (selfRect.CenterX() > otherRect.CenterX())
				newPos.x = curPos.x + overlapW;
			else
				newPos.x = curPos.x - overlapW;
		}


		transform().SetPosition(newPos);
		mCollider->UpdateCollider();

	}
}