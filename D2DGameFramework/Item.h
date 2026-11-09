#pragma once
#include <string>
#include "SoulSkill.h"
// 基础物品模板数据
struct Item
{
	int id;						 // 物品唯一ID
	std::wstring icon_id;		 // 图标ID(加载路径当ID)
	std::wstring name;			 // 名称
	std::wstring desc;			 // 描述
	int item_type;				 // 物品类型 (0 消耗品 1 武器 2 防具 3 饰品 4 红魂 5 蓝魂 6 黄魂 7 灰魂)
	int item_count;				 // 物品数量
	int max_stack = 9;			 // 最大堆叠数
	bool can_stack = true;       // 是否可堆叠

	virtual ~Item() = default;
};


struct ConsumableItem : public Item
{
	int store_price = 0;         // 商店购买价格
	int c_type;					 // 消耗品类型（0：HP药水 1：MP药水 2：回复状态药水） 
	int c_val;					 // 如果是hp mp 则代表回复的数值，如果是状态药水则代表状态ID
};




// 武器和防具的扩展
struct WeaponItem : public Item
{
	int super_mp_cost = 20;              // 超级攻击体力消耗
	int store_price = 0;					// 商店购买价格
	int p_atk = 0;							// 攻击力加成
	int p_def = 0;							// 防御力加成
	int p_luck = 0;							// 幸运加成
	int p_str = 0;							// 力量加成
	int p_con = 0;							// 体质加成
	int p_int = 0;							// 智力加成
	std::wstring swing_ani_id = L"";		// 挥舞动画ID
	std::wstring swing_sound_id = L"";		// 挥舞音效ID
	std::wstring super_ani_id = L"";		// 强化攻击动画ID
	std::wstring effect_ani_id = L"";		// 特效ID
	int i_frames = 0;						// 无敌帧数(攻击后怪物的无敌帧数)
};

struct ArmItem : public Item
{
	int store_price = 0;    // 商店购买价格
	int p_atk = 0;          // 攻击力加成
	int p_def = 0;          // 防御力加成
	int p_luck = 0;         // 幸运加成
	int p_str = 0;		    // 力量加成
	int p_con = 0;		    // 体质加成
	int p_int = 0;			// 智力加成
};

struct SoulItem : public Item
{
	int s_level_scale;      // 魂等级上限
	// 特殊的数值，红魂,蓝魂,代表魂基础数值，具体的实现在角色行为中实现，
	// 黄魂代表属性加成数值或者特殊效果数值，灰魂代表特殊能力bool开关
	int s_val = 0;
	int mp_cost = 0;			// 使用该魂的MP消耗（红魂为一次性消耗，蓝魂为持续消耗）
	SoulSkill* p_skill = nullptr;
		
};


