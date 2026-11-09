#pragma once
#ifndef __MY_PLAYER_H__
#define __MY_PLAYER_H__

#include "GamePlay/GameObject.h"
#include "Item.h"

namespace HelpTool { class INIConfig; }

class Player : public GamePlay::GameObject
{
public:
	~Player() override { if (mainPlayer == this) mainPlayer = nullptr; }
	WeaponItem* mHand = nullptr;
	ArmItem* mBody = nullptr;
	ArmItem* mAccessory = nullptr;
	SoulItem* mRedSoul = nullptr;
	SoulItem* mBulueSoul = nullptr;
	SoulItem* mYellowSoul = nullptr;

	static Player* main();
	static void SetMainPlayer(Player* player);

	// 存档序列化：装备槽 id 存取（空槽为 -1）
	void SaveEquipmentTo(HelpTool::INIConfig& ini);
	void LoadEquipmentFrom(HelpTool::INIConfig& ini);
private:
	static Player* mainPlayer;  ///< 主玩家实例
};

#define MainPlayer Player::main()

#endif // !__MY_PLAYER_H__
