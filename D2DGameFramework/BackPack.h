#pragma once
#include "Item.h"
#include <vector>

class Player;
namespace HelpTool { class INIConfig; }

class CBackPackManager
{
	std::vector<WeaponItem*> m_weapons;					// 武器
	std::vector<ArmItem*> m_armors;						// 防具
	std::vector<ArmItem*> m_accessories;				// 饰品
	std::vector<SoulItem*> m_red_souls;					// 红魂
	std::vector<SoulItem*> m_blue_souls;				// 蓝魂
	std::vector<SoulItem*> m_yellow_souls;				// 黄魂
	std::vector<SoulItem*> m_grey_souls;				// 灰魂
	std::vector<ConsumableItem*> m_consumables;			// 消耗品

public:
	static CBackPackManager& GetInstance();

	// ========== ID 统一接口 ==========
	bool AddItem(int itemId, int num = 1);
	bool SubItem(int itemId, int num = 1);

	// ========== 测试数据（开发阶段用） ==========
	void InitTestData();

	// ========== 存档序列化 ==========
	void Clear();									// 清空背包所有物品
	void SaveTo(HelpTool::INIConfig& ini);			// 将背包写入 INI
	void LoadFrom(HelpTool::INIConfig& ini);		// 从 INI 读取背包（先清空再重建）

	// ========== 查询 ==========
	int  GetItemCount(int itemId) const;				// 某物品在背包中的总数量
	bool HasItem(int itemId) const;						// 是否持有某物品

	// ========== 按类型获取列表（供 UI 遍历） ==========
	const std::vector<WeaponItem*>&		GetWeapons()     const { return m_weapons; }
	const std::vector<ArmItem*>&		GetArmors()      const { return m_armors; }
	const std::vector<ArmItem*>&		GetAccessories() const { return m_accessories; }
	const std::vector<SoulItem*>&		GetRedSouls()    const { return m_red_souls; }
	const std::vector<SoulItem*>&		GetBlueSouls()   const { return m_blue_souls; }
	const std::vector<SoulItem*>&		GetYellowSouls() const { return m_yellow_souls; }
	const std::vector<SoulItem*>&		GetGreySouls()   const { return m_grey_souls; }
	const std::vector<ConsumableItem*>&	GetConsumables() const { return m_consumables; }

	// ========== 装备管理（操作 Player::main()） ==========
	bool EquipWeapon(int itemId);			// true=成功 false=背包中无此物
	bool EquipArmor(int itemId);
	bool EquipAccessory(int itemId);
	bool EquipRedSoul(int itemId);
	bool EquipBlueSoul(int itemId);
	bool EquipYellowSoul(int itemId);

	// 卸下当前装备（放回背包 / 清空槽位）
	void UnequipWeapon();
	void UnequipArmor();
	void UnequipAccessory();
	void UnequipRedSoul();
	void UnequipBlueSoul();
	void UnequipYellowSoul();

	// 获取当前装备（nullptr = 空槽）
	WeaponItem* GetEquippedWeapon()    const;
	ArmItem*    GetEquippedArmor()     const;
	ArmItem*    GetEquippedAccessory() const;
	SoulItem*   GetEquippedRedSoul()   const;
	SoulItem*   GetEquippedBlueSoul()  const;
	SoulItem*   GetEquippedYellowSoul() const;

private:
	// 从 ItemConfig 模板克隆指定类型的物品实例
	Item* _CloneTemplate(const Item* pTemplate) const;
};

#define BackPack CBackPackManager::GetInstance()
