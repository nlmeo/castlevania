#include "Drop.h"
#include "BackPack.h"
#include "ItemConfig.h"
#include "Player.h"
#include "GameTool.h"
#include "D2DCore/TextureManager.h"
#include "TypeDefine/Geometric.h"
#include "GameCore/SceneManager.h"
#include "GamePlay/Debug.h"
#include <algorithm>

namespace
{
	constexpr float DROP_GRAVITY = 0.6f;   // 重力加速度（像素/帧²）
	constexpr float DROP_MAX_FALL = 8.0f;   // 最大下落速度（像素/帧）
}

std::vector<Drop*> Drop::sDrops;

Drop::Drop()
	: mItemId(0)
	, mItemCount(1)
	, mPicked(false)
	, mOnGround(false)
	, mPersistent(false)
	, mDy(0.0f)
	, mCollider(nullptr)
{
	// 掉落物层级（碰撞矩阵中与 Player 碰撞，用于拾取）
	mLayer = GamePlay::Drop;
	mName = "DropItem";

	// 图标渲染组件
	AddComponent<GamePlay::SpriteRender>();

	// 碰撞体：同时承担物理落地与拾取
	mCollider = AddComponent<GamePlay::BoxCollider>();
	mCollider->SetCollitionCallBack([this](GamePlay::Collider* other) { onCollision(other); });

	// 注册到存活列表
	sDrops.push_back(this);
}

Drop::~Drop()
{
	auto it = std::find(sDrops.begin(), sDrops.end(), this);
	if (it != sDrops.end()) sDrops.erase(it);
}

Drop* Drop::Spawn(int itemId, int count, const MathType::vec2f& pos, bool persistent)
{
	// 校验物品配置是否存在
	const Item* tpl = ITEM_CONFIG.GetItemTemplate(itemId);
	if (!tpl) return nullptr;

	Drop* drop = GT::CreateGameObject<Drop>();
	drop->transform().SetPosition(pos);
	drop->mPersistent = persistent;
	drop->mSceneName = SceneMgr.GetCurrentSceneName();
	drop->Init(itemId, count);
	return drop;
}

Drop* Drop::Spawn(int itemId, const MathType::vec2f& pos, bool persistent)
{
	return Spawn(itemId, 1, pos, persistent);
}

void Drop::OnSceneChanged()
{
	// 场景切换后：销毁非持久化掉落物，持久化掉落物仅在其所属场景显示
	std::string curScene = SceneMgr.GetCurrentSceneName();
	for (Drop* d : sDrops)
	{
		if (!d) continue;
		if (!d->mPersistent)
		{
			d->Destroy();
		}
		else
		{
			d->SetActive(d->mSceneName == curScene);
		}
	}
}

void Drop::Init(int itemId, int count)
{
	mItemId = itemId;
	mItemCount = count > 0 ? count : 1;

	const Item* tpl = ITEM_CONFIG.GetItemTemplate(mItemId);
	if (!tpl) return;

	// 加载物品图标作为掉落物贴图
	GamePlay::SpriteRender* sr = GetComponent<GamePlay::SpriteRender>();
	if (sr && !tpl->icon_id.empty())
	{
		D2DCore::Texture* tex = TexMgr.LoadTextureWithoutColor(tpl->icon_id.c_str());
		if (tex)
		{
			sr->SetTexture(tex);
			sr->SetAnchor(0.5f, 0.5f);
			sr->SetOrderInLayer(1);
			// 碰撞体大小跟随图标尺寸
			mCollider->SetColliderSize(tex->GetWidth(), tex->GetHeight());
		}
	}
	// 兜底：图标加载失败时使用默认碰撞尺寸
	if (mCollider->Width() <= 0.0f)
	{
		mCollider->SetColliderSize(40.0f, 40.0f);
	}
}

void Drop::Start()
{
	// 新对象在合并阶段才调用 Start（早于本帧碰撞检测），
	// 手动同步碰撞体AABB与渲染矩阵到当前位置
	if (mCollider)
	{
		mCollider->UpdateCollider();
	}
	GamePlay::SpriteRender* sr = GetComponent<GamePlay::SpriteRender>();
	if (sr)
	{
		sr->CaclRenderMatrix();
	}
}

void Drop::Update(float deltaTime)
{
	if (mPicked) return;

	UpdatePhysics(deltaTime);
}

void Drop::UpdatePhysics(float deltaTime)
{
	if (mOnGround) return;

	// 重力累积（向下为正）
	mDy += DROP_GRAVITY;
	if (mDy > DROP_MAX_FALL) mDy = DROP_MAX_FALL;

	// 分步下落，逐步检测碰撞，防止高速穿透（参考 Soma 的逐步位移）
	float moveY = mDy;
	int steps = (int)moveY;
	if (steps < 1) steps = 1;
	float stepY = moveY / (float)steps;

	unsigned int mask = ToBit(GamePlay::Obstacle) | ToBit(GamePlay::Platform) |
		ToBit(GamePlay::EdgePlatform) | ToBit(GamePlay::LeftSlop) | ToBit(GamePlay::RightSlop);
	std::vector<GamePlay::Collider*> hits;

	for (int i = 0; i < steps; ++i)
	{
		transform().translate({ 0.0f, stepY });
		mCollider->UpdateCollider();

		if (GT::CollisionDetect(*mCollider, mask, hits))
		{
			for (GamePlay::Collider* c : hits)
			{
				onCollision(c);
			}
			if (mOnGround) return;
		}
	}
}

void Drop::onCollision(GamePlay::Collider* other)
{
	if (mPicked || !other) return;

	GamePlay::GameLayer layer = other->gameobject()->layer();

	// 玩家碰撞 → 无需按键自动拾取
	if (layer == GamePlay::Player)
	{
		mPicked = true;
		BackPack.AddItem(mItemId, mItemCount);
		Debug.Log2Screen("拾取物品 id=%d 数量 x%d 已入背包", 3.0f, ColorType::COLORF::Yellow(), mItemId, mItemCount);
		Destroy();
		return;
	}

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
		if (ptInRect.y < t_y - 5.0f) return;

		mOnGround = true;
		mDy = 0.0f;
		MathType::vec2f newPos = transform().position();
		newPos.y = t_y - (selfRect.Height() / 2.0f);
		transform().SetPosition(newPos);
		mCollider->UpdateCollider();
		return;
	}

	// 地形碰撞 → 落地（参考 Soma 的地形碰撞处理）
	if (layer == GamePlay::Obstacle || layer == GamePlay::Platform ||
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

		if (overlapW > overlapH)
		{
			// 垂直方向重叠为主
			if (selfRect.CenterY() < otherRect.CenterY())
			{
				// 掉落物在地形上方：落地
				newPos.y = curPos.y - overlapH;
				mOnGround = true;
				mDy = 0.0f;
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

		if (newPos.x != curPos.x || newPos.y != curPos.y)
		{
			transform().SetPosition(newPos);
			mCollider->UpdateCollider();
		}
	}
}

void Drop::OnDebugRender()
{
	if (!mCollider) return;

	// 显示掉落物碰撞区域（用于调试拾取与物理碰撞）
	float posx = transform().position().x;
	float posy = transform().position().y;
	float halfW = mCollider->Width() / 2;
	float halfH = mCollider->Height() / 2;
	ColorType::COLORF color = ColorType::COLORF::Yellow();

	Debug.DebugDrawLine(posx - halfW, posy - halfH, posx + halfW, posy - halfH, color);
	Debug.DebugDrawLine(posx - halfW, posy - halfH, posx - halfW, posy + halfH, color);
	Debug.DebugDrawLine(posx + halfW, posy - halfH, posx + halfW, posy + halfH, color);
	Debug.DebugDrawLine(posx - halfW, posy + halfH, posx + halfW, posy + halfH, color);
}
