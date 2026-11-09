#pragma once
#include "GamePlay/GameObject.h"
#include "TypeDefine/Vector2.h"

// 地图掉落道具实体：
// 传入物品ID与世界坐标即可生成对应的掉落物（显示物品图标）。
// 生成后受重力下落并与地形碰撞落地（碰撞逻辑参考 Soma 实现），
// 玩家碰触（碰撞）后无需按键自动拾取并加入背包。
//
// 持久化（persistent）：
//  - persistent = false（默认，怪物掉落品）：切换场景时会被销毁
//  - persistent = true：切换场景时不销毁，但只在其所属场景可见（其他场景自动隐藏）
class Drop : public GamePlay::GameObject
{
public:
	Drop();
	virtual ~Drop() override;

	// 根据物品ID与世界坐标生成掉落物，persistent=true时跨场景保留，配置无效返回nullptr
	static Drop* Spawn(int itemId, int count, const MathType::vec2f& pos, bool persistent = false);
	// 根据物品ID与世界坐标生成掉落物（数量默认1）
	static Drop* Spawn(int itemId, const MathType::vec2f& pos, bool persistent = false);

	// 场景切换回调：销毁非持久化掉落物，并刷新持久化掉落物的可见状态（供场景 OnEnter 调用）
	static void OnSceneChanged();

	void Start() override;
	void Update(float deltaTime) override;
	void OnDebugRender() override;

private:
	// 初始化掉落物：加载物品图标、配置碰撞体（由 Spawn 调用）
	void Init(int itemId, int count);
	// 物理更新：重力下落 + 地形碰撞（参考 Soma 实现）
	void UpdatePhysics(float deltaTime);
	// 碰撞回调：玩家碰撞拾取 / 地形碰撞落地
	void onCollision(GamePlay::Collider* other);

	int mItemId;                       // 物品ID
	int mItemCount;                    // 掉落数量
	bool mPicked;                      // 是否已被拾取（防止重复触发）
	bool mOnGround;                    // 是否已落地
	bool mPersistent;                  // 是否持久化（跨场景保留，仅所属场景可见）
	float mDy;                         // 垂直下落速度（重力累积，向下为正）
	std::string mSceneName;            // 所属场景（持久化掉落物仅在此场景可见）
	GamePlay::BoxCollider* mCollider;  // 碰撞体（物理 + 拾取）

	static std::vector<Drop*> sDrops;  // 所有存活掉落物注册表
};
