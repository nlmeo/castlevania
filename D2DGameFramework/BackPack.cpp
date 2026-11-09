#include "BackPack.h"
#include "ItemConfig.h"
#include "Player.h"
#include <algorithm>
#include <string>
#include <cstdlib>
#include "HelpTool/INIConfig.h"
#include "GameTool.h"
#include "SoulSkill.h"
#include "S_Fly.h"
#include "S_BlackPather.h"
#include "Soma.h"
#include "S_Bat.h"
#include "S_Axe.h"
CBackPackManager& CBackPackManager::GetInstance()
{
	static CBackPackManager instance;
	return instance;
}

// ==================== 测试数据 ====================

void CBackPackManager::InitTestData()
{
	// 注意：ItemConfig::Init() 应由游戏初始化链路提前调用，此处仅填充背包数据

	// ---------- 武器 ----------
	AddItem(3, 4); // 短剑 x30
	AddItem(4, 4);  // 传说中的魔剑 x1

	// ---------- 防具 ----------
	AddItem(5, 1);  // 传说中的魔铠甲 x5
	AddItem(6, 1);
	// ---------- 饰品 ----------
	AddItem(7, 1);  // 大魔法师的戒指 x3
	AddItem(8, 1);
	// ---------- 红魂 ----------
	AddItem(9, 9);  // 雷枪 x3

	// ---------- 蓝魂 ----------
	AddItem(10, 6); // 
	AddItem(13, 1); // 
	AddItem(14, 1); // 
	// ---------- 黄魂 ----------
	AddItem(11, 3);  // 石像鬼 x4

	// ---------- 灰魂 ----------
	AddItem(10, 1); // 猫妖 x1

	// ---------- 消耗品 ----------
	AddItem(1, 99); // 小药瓶(HP) x99
	AddItem(2, 50); // 小药瓶(MP) x50

	// ---------- 初始装备 ----------
	EquipWeapon(4);    // 装备短剑
	EquipArmor(5);     // 装备铠甲
	EquipAccessory(7); // 装备戒指
	//EquipRedSoul(7);   // 装备雷枪
	EquipBlueSoul(10);  // 装备蝙蝠
	EquipYellowSoul(11);   // 装备石像鬼
}

// ==================== 存档序列化 ====================

void CBackPackManager::Clear()
{
	for (auto* p : m_weapons) delete p;
	for (auto* p : m_armors) delete p;
	for (auto* p : m_accessories) delete p;
	for (auto* p : m_red_souls) delete p;
	for (auto* p : m_blue_souls) delete p;
	for (auto* p : m_yellow_souls) delete p;
	for (auto* p : m_grey_souls) delete p;
	for (auto* p : m_consumables) delete p;

	m_weapons.clear();
	m_armors.clear();
	m_accessories.clear();
	m_red_souls.clear();
	m_blue_souls.clear();
	m_yellow_souls.clear();
	m_grey_souls.clear();
	m_consumables.clear();
}

void CBackPackManager::SaveTo(HelpTool::INIConfig& ini)
{
	auto serializeVec = [](const auto& vec) -> std::string {
		std::string s;
		for (const auto* item : vec)
		{
			if (!s.empty()) s += ",";
			s += std::to_string(item->id) + ":" + std::to_string(item->item_count);
		}
		return s;
	};

	ini.Value("backpack", "weapons") = serializeVec(m_weapons);
	ini.Value("backpack", "armors") = serializeVec(m_armors);
	ini.Value("backpack", "accessories") = serializeVec(m_accessories);
	ini.Value("backpack", "red_souls") = serializeVec(m_red_souls);
	ini.Value("backpack", "blue_souls") = serializeVec(m_blue_souls);
	ini.Value("backpack", "yellow_souls") = serializeVec(m_yellow_souls);
	ini.Value("backpack", "grey_souls") = serializeVec(m_grey_souls);
	ini.Value("backpack", "consumables") = serializeVec(m_consumables);
}

void CBackPackManager::LoadFrom(HelpTool::INIConfig& ini)
{
	Clear();

	auto loadVec = [&](const char* key) {
		if (!ini.HasKey("backpack", key)) return;
		std::string s = (std::string)ini.Value("backpack", key);

		size_t pos = 0;
		while (pos < s.size())
		{
			size_t comma = s.find(',', pos);
			std::string token = (comma == std::string::npos) ? s.substr(pos) : s.substr(pos, comma - pos);

			size_t colon = token.find(':');
			if (colon != std::string::npos)
			{
				int id = atoi(token.substr(0, colon).c_str());
				int count = atoi(token.substr(colon + 1).c_str());
				if (id > 0 && count > 0)
					AddItem(id, count);
			}

			if (comma == std::string::npos) break;
			pos = comma + 1;
		}
	};

	loadVec("weapons");
	loadVec("armors");
	loadVec("accessories");
	loadVec("red_souls");
	loadVec("blue_souls");
	loadVec("yellow_souls");
	loadVec("grey_souls");
	loadVec("consumables");
}

// ==================== 内部辅助 ====================

Item* CBackPackManager::_CloneTemplate(const Item* pTemplate) const
{
	if (!pTemplate) return nullptr;

	switch (pTemplate->item_type)
	{
	case 0: // Consumable
	{
		auto* clone = new ConsumableItem(*static_cast<const ConsumableItem*>(pTemplate));
		clone->item_count = 0;
		return clone;
	}
	case 1: // Weapon
	{
		auto* clone = new WeaponItem(*static_cast<const WeaponItem*>(pTemplate));
		clone->item_count = 1;
		return clone;
	}
	case 2: // Armor
	case 3: // Accessory
	{
		auto* clone = new ArmItem(*static_cast<const ArmItem*>(pTemplate));
		clone->item_count = 1;
		return clone;
	}
	case 4: // RedSoul
	case 5: // BlueSoul
	case 6: // YellowSoul
	case 7: // GreySoul
	{
		auto* clone = new SoulItem(*static_cast<const SoulItem*>(pTemplate));
		clone->item_count = 1;
		return clone;
	}
	default:
		return nullptr;
	}
}

// ==================== 查询 ====================

int CBackPackManager::GetItemCount(int itemId) const
{
	const Item* pTemplate = ITEM_CONFIG.GetItemTemplate(itemId);
	if (!pTemplate) return 0;

	auto countInVec = [itemId](const auto& vec) -> int {
		int total = 0;
		for (const auto* item : vec)
		{
			if (item->id == itemId)
				total += item->item_count;
		}
		return total;
	};

	switch (pTemplate->item_type)
	{
	case 0: return countInVec(m_consumables);
	case 1: return countInVec(m_weapons);
	case 2: return countInVec(m_armors);
	case 3: return countInVec(m_accessories);
	case 4: return countInVec(m_red_souls);
	case 5: return countInVec(m_blue_souls);
	case 6: return countInVec(m_yellow_souls);
	case 7: return countInVec(m_grey_souls);
	default: return 0;
	}
}

bool CBackPackManager::HasItem(int itemId) const
{
	return GetItemCount(itemId) > 0;
}

// ==================== 添加物品 ====================

bool CBackPackManager::AddItem(int itemId, int num)
{
	if (num <= 0) return false;

	const Item* pTemplate = ITEM_CONFIG.GetItemTemplate(itemId);
	if (!pTemplate) return false;

	switch (pTemplate->item_type)
	{
	// ---------- 消耗品（可堆叠） ----------
	case 0:
	{
		if (pTemplate->can_stack)
		{
			// 先尝试堆叠到已有物品
			for (auto* item : m_consumables)
			{
				if (item->id != itemId) continue;
				int space = item->max_stack - item->item_count;
				if (space <= 0) continue;

				int add = (std::min)(num, space);
				item->item_count += add;
				num -= add;
				if (num <= 0) return true;
			}
		}
		// 剩余部分新建条目
		while (num > 0)
		{
			auto* clone = static_cast<ConsumableItem*>(_CloneTemplate(pTemplate));
			if (!clone) return false;
			int add = (std::min)(num, clone->max_stack);
			clone->item_count = add;
			num -= add;
			m_consumables.push_back(clone);
		}
		return true;
	}

	// ---------- 武器 / 防具 / 饰品（不可堆叠，逐个添加） ----------
	case 1:
		for (int i = 0; i < num; i++)
		{
			auto* clone = static_cast<WeaponItem*>(_CloneTemplate(pTemplate));
			if (!clone) return false;
			m_weapons.push_back(clone);
		}
		return true;

	case 2:
		for (int i = 0; i < num; i++)
		{
			auto* clone = static_cast<ArmItem*>(_CloneTemplate(pTemplate));
			if (!clone) return false;
			m_armors.push_back(clone);
		}
		return true;

	case 3:
		for (int i = 0; i < num; i++)
		{
			auto* clone = static_cast<ArmItem*>(_CloneTemplate(pTemplate));
			if (!clone) return false;
			m_accessories.push_back(clone);
		}
		return true;

	case 4:
		//for (int i = 0; i < num; i++)
		//{
		//	auto* clone = static_cast<SoulItem*>(_CloneTemplate(pTemplate));
		//	if (!clone) return false;
		//	m_red_souls.push_back(clone);
		//}
		//return true;

		if (pTemplate->can_stack)
		{
			// 先尝试堆叠到已有物品
			for (auto* item : m_red_souls)
			{
				if (item->id != itemId) continue;
				int space = item->max_stack - item->item_count;
				if (space <= 0) continue;

				int add = (std::min)(num, space);
				item->item_count += add;
				num -= add;
				if (num <= 0) return true;
			}
		}
		// 剩余部分新建条目
		while (num > 0)
		{
			auto* clone = static_cast<SoulItem*>(_CloneTemplate(pTemplate));
			if (!clone) return false;
			int add = (std::min)(num, clone->max_stack);
			clone->item_count = add;
			num -= add;
			m_red_souls.push_back(clone);
		}
		return true;
	case 5:
		//for (int i = 0; i < num; i++)
		//{
		//	auto* clone = static_cast<SoulItem*>(_CloneTemplate(pTemplate));
		//	if (!clone) return false;
		//	m_blue_souls.push_back(clone);
		//}
		//return true;

		if (pTemplate->can_stack)
		{
			// 先尝试堆叠到已有物品
			for (auto* item : m_blue_souls)
			{
				if (item->id != itemId) continue;
				int space = item->max_stack - item->item_count;
				if (space <= 0) continue;

				int add = (std::min)(num, space);
				item->item_count += add;
				num -= add;
				if (num <= 0) return true;
			}
		}
		// 剩余部分新建条目
		while (num > 0)
		{
			auto* clone = static_cast<SoulItem*>(_CloneTemplate(pTemplate));
			if (!clone) return false;
			int add = (std::min)(num, clone->max_stack);
			clone->item_count = add;
			num -= add;
			m_blue_souls.push_back(clone);
		}
		return true;

	case 6:
		//for (int i = 0; i < num; i++)
		//{
		//	auto* clone = static_cast<SoulItem*>(_CloneTemplate(pTemplate));
		//	if (!clone) return false;
		//	m_yellow_souls.push_back(clone);
		//}
		//return true;
		if (pTemplate->can_stack)
		{
			// 先尝试堆叠到已有物品
			for (auto* item : m_yellow_souls)
			{
				if (item->id != itemId) continue;
				int space = item->max_stack - item->item_count;
				if (space <= 0) continue;

				int add = (std::min)(num, space);
				item->item_count += add;
				num -= add;
				if (num <= 0) return true;
			}
		}
		// 剩余部分新建条目
		while (num > 0)
		{
			auto* clone = static_cast<SoulItem*>(_CloneTemplate(pTemplate));
			if (!clone) return false;
			int add = (std::min)(num, clone->max_stack);
			clone->item_count = add;
			num -= add;
			m_yellow_souls.push_back(clone);
		}
		return true;
	case 7:
		//for (int i = 0; i < num; i++)
		//{
		//	auto* clone = static_cast<SoulItem*>(_CloneTemplate(pTemplate));
		//	if (!clone) return false;
		//	m_grey_souls.push_back(clone);
		//}
		//return true;

		if (pTemplate->can_stack)
		{
			// 先尝试堆叠到已有物品
			for (auto* item : m_grey_souls)
			{
				if (item->id != itemId) continue;
				int space = item->max_stack - item->item_count;
				if (space <= 0) continue;

				int add = (std::min)(num, space);
				item->item_count += add;
				num -= add;
				if (num <= 0) return true;
			}
		}
		// 剩余部分新建条目
		while (num > 0)
		{
			auto* clone = static_cast<SoulItem*>(_CloneTemplate(pTemplate));
			if (!clone) return false;
			int add = (std::min)(num, clone->max_stack);
			clone->item_count = add;
			num -= add;
			m_grey_souls.push_back(clone);
		}
		return true;

	default:
		return false;
	}
}

// ==================== 移除物品 ====================

bool CBackPackManager::SubItem(int itemId, int num)
{
	if (num <= 0) return false;

	const Item* pTemplate = ITEM_CONFIG.GetItemTemplate(itemId);
	if (!pTemplate) return false;

	// 辅助：从指定向量中移除 num 个 itemId
	auto removeFromVec = [itemId, &num](auto& vec) -> bool {
		for (auto it = vec.begin(); it != vec.end() && num > 0; )
		{
			if ((*it)->id != itemId)
			{
				++it;
				continue;
			}

			if ((*it)->item_count <= num)
			{
				num -= (*it)->item_count;
				delete *it;
				it = vec.erase(it);
			}
			else
			{
				(*it)->item_count -= num;
				num = 0;
				break;
			}
		}
		return true;
	};

	switch (pTemplate->item_type)
	{
	case 0: return removeFromVec(m_consumables) && num == 0;
	case 1: return removeFromVec(m_weapons)     && num == 0;
	case 2: return removeFromVec(m_armors)      && num == 0;
	case 3: return removeFromVec(m_accessories) && num == 0;
	case 4: return removeFromVec(m_red_souls)   && num == 0;
	case 5: return removeFromVec(m_blue_souls)  && num == 0;
	case 6: return removeFromVec(m_yellow_souls)&& num == 0;
	case 7: return removeFromVec(m_grey_souls)  && num == 0;
	default: return false;
	}
}

// ==================== 装备管理 ====================

bool CBackPackManager::EquipWeapon(int itemId)
{
	Player* player = Player::main();
	if (!player) return false;

	// 从背包取出
	WeaponItem* target = nullptr;
	for (auto it = m_weapons.begin(); it != m_weapons.end(); ++it)
	{
		if ((*it)->id == itemId)
		{
			target = *it;
			m_weapons.erase(it);
			break;
		}
	}
	if (!target) return false;

	// 卸下当前装备（放回背包）
	if (player->mHand)
		m_weapons.push_back(player->mHand);

	// 装备新武器
	player->mHand = target;
	return true;
}

bool CBackPackManager::EquipArmor(int itemId)
{
	Player* player = Player::main();
	if (!player) return false;

	ArmItem* target = nullptr;
	for (auto it = m_armors.begin(); it != m_armors.end(); ++it)
	{
		if ((*it)->id == itemId)
		{
			target = *it;
			m_armors.erase(it);
			break;
		}
	}
	if (!target) return false;

	if (player->mBody)
		m_armors.push_back(player->mBody);

	player->mBody = target;
	return true;
}

bool CBackPackManager::EquipAccessory(int itemId)
{
	Player* player = Player::main();
	if (!player) return false;

	ArmItem* target = nullptr;
	for (auto it = m_accessories.begin(); it != m_accessories.end(); ++it)
	{
		if ((*it)->id == itemId)
		{
			target = *it;
			m_accessories.erase(it);
			break;
		}
	}
	if (!target) return false;

	if (player->mAccessory)
		m_accessories.push_back(player->mAccessory);

	player->mAccessory = target;


	auto* curWeapon = BackPack.GetEquippedWeapon();
	auto* curAcc = BackPack.GetEquippedAccessory();
	auto* curBody = BackPack.GetEquippedArmor();
	if (curWeapon && curWeapon->id == 4 && curBody && curBody->id == 5 && curAcc && curAcc->id == 7)
	{
		GT::StopAllMusic();
		GT::PlayBGM(BGM_VAMPIRE_KILLER);
	}


	return true;
}

bool CBackPackManager::EquipRedSoul(int itemId)
{
	Player* player = Player::main();
	if (!player) return false;

	SoulItem* target = nullptr;
	for (auto it = m_red_souls.begin(); it != m_red_souls.end(); ++it)
	{
		if ((*it)->id == itemId)
		{
			target = *it;
			//m_red_souls.erase(it);
			break;
		}
	}
	if (!target) return false;

	//if (player->mRedSoul)
	//	m_red_souls.push_back(player->mRedSoul);

	if (player->mRedSoul && player->mRedSoul->p_skill)
	{
		SAFE_DESTROY(player->mRedSoul->p_skill);
		player->mRedSoul->p_skill = nullptr;
	}

	// 退出当前魂 FSM 状态（如果正在使用中）
	//auto fsm = player->GetComponent<GamePlay::FSM>();
	//if (fsm)
	//{
	//	auto* curState = fsm->GetCurrentState();
	//	if (curState == fsm->GetState(L"black_panther") || curState == fsm->GetState(L"blue_soul"))
	//	{
	//		Soma* soma = dynamic_cast<Soma*>(player);
	//		fsm->SetNextState(soma && soma->OnGround() ? L"idle" : L"fall");
	//	}
	//}

	// 创建新 SouSkill（运行时框架已就绪）
	if (itemId == 9)
		target->p_skill = GT::CreateGameObject<S_Axe>();
	

	if (target->p_skill != nullptr)
		target->p_skill->Init();



	player->mRedSoul = target;
	return true;
}

bool CBackPackManager::EquipBlueSoul(int itemId)
{
	Player* player = Player::main();
	if (!player) return false;

	SoulItem* target = nullptr;
	for (auto it = m_blue_souls.begin(); it != m_blue_souls.end(); ++it)
	{
		if ((*it)->id == itemId)
		{
			target = *it;
			//m_blue_souls.erase(it);
			break;
		}
	}
	if (!target) return false;

	//if (player->mBulueSoul)
	//	m_blue_souls.push_back(player->mBulueSoul);

	// 销毁旧蓝魂的 SouSkill，停止特效和属性修改
	if (player->mBulueSoul && player->mBulueSoul->p_skill)
	{
		SAFE_DESTROY(player->mBulueSoul->p_skill);
		player->mBulueSoul->p_skill = nullptr;
	}

	// 退出当前蓝魂 FSM 状态（如果正在使用中）
	auto fsm = player->GetComponent<GamePlay::FSM>();
	if (fsm)
	{
		auto* curState = fsm->GetCurrentState();
		if (curState == fsm->GetState(L"black_panther") || curState == fsm->GetState(L"blue_soul"))
		{
			Soma* soma = dynamic_cast<Soma*>(player);
			fsm->SetNextState(soma && soma->OnGround() ? L"idle" : L"fall");
		}
	}

	// 创建新 SouSkill（运行时框架已就绪）
	if (itemId == 13)
		target->p_skill = GT::CreateGameObject<S_Fly>();
	else if (itemId == 14)
		target->p_skill = GT::CreateGameObject<S_BlackPather>();
	else if(itemId == 10)
		target->p_skill = GT::CreateGameObject<S_Bat>();

	if (target->p_skill != nullptr)
		target->p_skill->Init();

	player->mBulueSoul = target;
	
	return true;
}

bool CBackPackManager::EquipYellowSoul(int itemId)
{
	Player* player = Player::main();
	if (!player) return false;

	SoulItem* target = nullptr;
	for (auto it = m_yellow_souls.begin(); it != m_yellow_souls.end(); ++it)
	{
		if ((*it)->id == itemId)
		{
			target = *it;
			//m_yellow_souls.erase(it);
			break;
		}
	}
	if (!target) return false;

	//if (player->mYellowSoul)
	//	m_yellow_souls.push_back(player->mYellowSoul);

	player->mYellowSoul = target;
	return true;
}

// ==================== 获取当前装备 ====================

WeaponItem* CBackPackManager::GetEquippedWeapon() const
{
	Player* player = Player::main();
	return player ? player->mHand : nullptr;
}

ArmItem* CBackPackManager::GetEquippedArmor() const
{
	Player* player = Player::main();
	return player ? player->mBody : nullptr;
}

ArmItem* CBackPackManager::GetEquippedAccessory() const
{
	Player* player = Player::main();
	return player ? player->mAccessory : nullptr;
}

SoulItem* CBackPackManager::GetEquippedRedSoul() const
{
	Player* player = Player::main();
	return player ? player->mRedSoul : nullptr;
}

SoulItem* CBackPackManager::GetEquippedBlueSoul() const
{
	Player* player = Player::main();
	return player ? player->mBulueSoul : nullptr;
}

SoulItem* CBackPackManager::GetEquippedYellowSoul() const
{
	Player* player = Player::main();
	return player ? player->mYellowSoul : nullptr;
}

// ==================== 卸下装备 ====================

void CBackPackManager::UnequipWeapon()
{
	Player* player = Player::main();
	if (!player || !player->mHand) return;
	m_weapons.push_back(player->mHand);
	player->mHand = nullptr;
}

void CBackPackManager::UnequipArmor()
{
	Player* player = Player::main();
	if (!player || !player->mBody) return;
	m_armors.push_back(player->mBody);
	player->mBody = nullptr;
}

void CBackPackManager::UnequipAccessory()
{
	Player* player = Player::main();
	if (!player || !player->mAccessory) return;
	m_accessories.push_back(player->mAccessory);
	player->mAccessory = nullptr;
}

void CBackPackManager::UnequipRedSoul()
{
	Player* player = Player::main();
	//if (player) player->mRedSoul = nullptr;
	if (!player) return;

	// 停止特效并销毁 SouSkill
	if (player->mRedSoul && player->mRedSoul->p_skill)
	{
		SAFE_DESTROY(player->mRedSoul->p_skill);
		player->mRedSoul->p_skill = nullptr;
	}

	player->mRedSoul = nullptr;

	//// 退出蓝魂 FSM 状态
	//auto fsm = player->GetComponent<GamePlay::FSM>();
	//if (fsm)
	//{
	//	auto* curState = fsm->GetCurrentState();
	//	if (curState == fsm->GetState(L"black_panther") || curState == fsm->GetState(L"blue_soul"))
	//	{
	//		auto* soma = dynamic_cast<Soma*>(player);
	//		fsm->SetNextState(soma && soma->OnGround() ? L"idle" : L"fall");
	//	}
	//}
}

void CBackPackManager::UnequipBlueSoul()
{
	Player* player = Player::main();
	if (!player) return;

	// 停止特效并销毁 SouSkill
	if (player->mBulueSoul && player->mBulueSoul->p_skill)
	{
		SAFE_DESTROY(player->mBulueSoul->p_skill);
		player->mBulueSoul->p_skill = nullptr;
	}

	player->mBulueSoul = nullptr;

	// 退出蓝魂 FSM 状态
	auto fsm = player->GetComponent<GamePlay::FSM>();
	if (fsm)
	{
		auto* curState = fsm->GetCurrentState();
		if (curState == fsm->GetState(L"black_panther") || curState == fsm->GetState(L"blue_soul"))
		{
			auto* soma = dynamic_cast<Soma*>(player);
			fsm->SetNextState(soma && soma->OnGround() ? L"idle" : L"fall");
		}
	}
}

void CBackPackManager::UnequipYellowSoul()
{
	Player* player = Player::main();
	if (player) player->mYellowSoul = nullptr;
}
