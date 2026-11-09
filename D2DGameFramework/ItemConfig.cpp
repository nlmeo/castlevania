#include "ItemConfig.h"
#include "S_Fly.h"
#include "S_BlackPather.h"
#include "S_Axe.h"
#include "GameTool.h"

CItemConfig& CItemConfig::GetInstance()
{
	static CItemConfig instance;
	return instance;
}

const Item* CItemConfig::GetItemTemplate(int item_id) const
{
	std::unordered_map<int, Item*>::const_iterator it = m_item_templates.find(item_id);
	if (it != m_item_templates.end())
		return it->second;
	return nullptr;
}

//TODO模板确认后修改析构

void CItemConfig::Init()
{
    // ==============================
    // 【在这里配置所有物品 & 魂】
    // ==============================

    // 消耗品
    {
        ConsumableItem* item = new ConsumableItem();
        item->id = 1;
        item->name = L"小红瓶";
        item->desc = L"回复100点HP";
        item->icon_id = L"resource\\ui\\Icons\\008.bmp";
        item->item_type = 0; // 消耗品
		item->c_type = 0; // HP药水
        item->c_val = 100;
        item->store_price = 10;
        m_item_templates[item->id] = item;
    }

    {
        ConsumableItem* item = new ConsumableItem();
        item->id = 2;
        item->name = L"小蓝瓶";
        item->desc = L"回复100点MP";
        item->icon_id = L"resource\\ui\\Icons\\005.bmp";
        item->item_type = 0; // 消耗品
		item->c_type = 1; // MP药水
        item->c_val = 100;
        item->store_price = 10;
        m_item_templates[item->id] = item;
    }

    {
        WeaponItem* item = new WeaponItem();
        item->id = 3;
        item->name = L"钝刀";
        item->desc = L"由于经常使用，已经磨损严重";
        item->icon_id = L"resource\\ui\\Icons\\097.bmp";
		item->item_type = 1;  // 武器
       
        item->p_atk = 12;
        //item->p_def = 1;
        //item->p_luck = 1;
        //item->p_str = 1;
        //item->p_con = 1;
        //item->p_int = 1;
        item->swing_ani_id = 1;
		item->swing_sound_id = 1;
        item->super_ani_id = 1;
		item->effect_ani_id = 1;
		item->i_frames = 0;

        item->store_price = 50;
        m_item_templates[item->id] = item;
    }


	//{
	//	WeaponItem* item = new WeaponItem();
	//	item->id = 66;
	//	item->name = L"死神之镰";
	//	item->desc = L"由于经常使用，已经磨损严重";
	//	item->icon_id = L"resource\\ui\\Icons\\097.bmp";
	//	item->item_type = 1;  // 武器

	//	item->p_atk = 7;
	//	//item->p_def = 1;
	//	//item->p_luck = 1;
	//	//item->p_str = 1;
	//	//item->p_con = 1;
	//	//item->p_int = 1;
	//	item->swing_ani_id = 1;
	//	item->swing_sound_id = 1;
	//	item->super_ani_id = 1;
	//	item->effect_ani_id = 1;
	//	item->i_frames = 0;

	//	item->store_price = 50;
	//	m_item_templates[item->id] = item;
	//}




    {
        WeaponItem* item = new WeaponItem();
        item->id = 4;
		item->super_mp_cost = 35;
        item->name = L"光剑·克劳索拉斯";
        item->desc = L"某位魔法使锻造的巨剑，传说里面寄宿着神秘的力量";
        item->icon_id = L"resource\\ui\\Icons\\089.bmp";
        item->item_type = 1;  // 武器

        item->p_atk = 21;

        item->swing_ani_id = 1;
        item->swing_sound_id = 1;
        item->super_ani_id = 1;
        item->effect_ani_id = 1;

        item->store_price = 100;
        m_item_templates[item->id] = item;
    }

    {
        ArmItem* item = new ArmItem();
        item->id = 5;
        item->name = L"AAAA魔法长袍";
        item->desc = L"防护能力一般，但是胜在轻巧";
        item->icon_id = L"resource\\ui\\Icons\\121.bmp";
		item->item_type = 2; // 防具
        
        item->p_def = 8;
        item->store_price = 30;
        m_item_templates[item->id] = item;
    }

	{
		ArmItem* item = new ArmItem();
		item->id = 6;
		item->name = L"布衣";
		item->desc = L"手里捧着窝窝头~，菜里没有一滴油~~~";
		item->icon_id = L"resource\\ui\\Icons\\122.bmp";
		item->item_type = 2; // 防具

		item->p_def = 1;
		item->store_price = 30;
		m_item_templates[item->id] = item;
	}

    {
        ArmItem* item = new ArmItem();
        item->id = 7;
        item->name = L"AAAA魔法指环";
        item->desc = L"那个男人成为神之前使用过的指环";
        item->icon_id = L"resource\\ui\\Icons\\170.bmp";
		item->item_type = 3; // 首饰

        item->p_atk = 1;
        item->p_def = 1;
		item->p_luck = 1;
		item->p_str = 1;
		item->p_con = 1;
        item->p_int = 1;
        
        item->store_price = 30;
        m_item_templates[item->id] = item;
    }

	{
		ArmItem* item = new ArmItem();
		item->id = 8;
		item->name = L"护身符";
		item->desc = L"随地可见的护身符";
		item->icon_id = L"resource\\ui\\Icons\\155.bmp";
		item->item_type = 3; // 首饰

		item->p_atk = 1;
		item->p_def = 1;
		item->p_luck = 1;
		item->p_str = 1;
		item->p_con = 1;
		item->p_int = 1;

		item->store_price = 30;
		m_item_templates[item->id] = item;
	}


    // ==============================
    // 苍月十字架 四色魂配置
    // ==============================

	// 4. 红魂 - 斧甲兵
    {
        SoulItem* item = new SoulItem();
        item->id = 9;
        item->name = L"斧甲兵";
        item->desc = L"向上方投出斧头";
        item->item_type = 4;

        item->s_level_scale = 9;
        item->mp_cost = 25;
        item->s_val = 10; // 未修正伤害
        m_item_templates[item->id] = item;
    }

    // 5. 蓝魂 - 蝙蝠
    {
        SoulItem* item = new SoulItem();
        item->id = 10;
        item->name = L"蝙蝠";
        item->desc = L"召唤蝙蝠使魔攻击敌人";
        item->item_type = 5;
        item->s_level_scale = 3;
        item->mp_cost = 9; // 持续消耗
        item->s_val = 10;  // 未修正基础伤害
        m_item_templates[item->id] = item;
    }

	

    // 6. 黄魂 - 石傀儡
    {
        SoulItem* item = new SoulItem();
        item->id = 11;
        item->name = L"石傀儡";
        item->desc = L"提高STR";
        item->item_type = 6;
        item->s_level_scale = 9;
        item->s_val = 2; // 攻击加成
        m_item_templates[item->id] = item;
    }

    // 7. 灰魂 - 二段跳
    {
        SoulItem* item = new SoulItem();
        item->id = 12;
        item->name = L"二段跳";
        item->desc = L"可以跳第二次";
        item->item_type = 7; // 灰魂
        item->s_level_scale = 1;
        item->s_val = 1; // 开启能力
        m_item_templates[item->id] = item;
    }

	{
		SoulItem* item = new SoulItem();
		item->id = 13;
		item->name = L"飞装甲";
		item->desc = L"减少重力，可以跳的更远";
		item->item_type = 5;
		item->s_level_scale = 3;
		item->mp_cost = 9; // 持续消耗
		item->s_val = 10;  // 未修正基础伤害
		m_item_templates[item->id] = item;
	}

	{
		SoulItem* item = new SoulItem();
		item->id = 14;
		item->name = L"黑豹";
		item->desc = L"可以进行冲刺";
		item->item_type = 5;
		item->s_level_scale = 3;
		item->mp_cost = 9; // 持续消耗
		item->s_val = 10;  // 未修正基础伤害
		m_item_templates[item->id] = item;
	}
}


