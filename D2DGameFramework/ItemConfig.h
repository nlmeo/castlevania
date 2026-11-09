#pragma once
#include "Item.h"
#include <unordered_map>

//全局物品配置类，负责加载和提供物品数据
class CItemConfig
{
	std::unordered_map<int,struct Item*> m_item_templates; //物品ID到物品指针的映射
	CItemConfig() = default;
public:
	static CItemConfig& GetInstance(); //单例模式
	void Init(); //加载物品数据
	const struct Item* GetItemTemplate(int item_id) const; //根据ID获取物品
};
#define ITEM_CONFIG CItemConfig::GetInstance()