#include "Player.h"
#include "BackPack.h"
#include "HelpTool/INIConfig.h"

Player* Player::mainPlayer = nullptr;

Player* Player::main()
{
	return mainPlayer;
}

void Player::SetMainPlayer(Player* player)
{
	mainPlayer = player;
}

void Player::SaveEquipmentTo(HelpTool::INIConfig& ini)
{
	ini.Value("equipment", "weapon") = mHand ? mHand->id : -1;
	ini.Value("equipment", "armor") = mBody ? mBody->id : -1;
	ini.Value("equipment", "accessory") = mAccessory ? mAccessory->id : -1;
	ini.Value("equipment", "red_soul") = mRedSoul ? mRedSoul->id : -1;
	ini.Value("equipment", "blue_soul") = mBulueSoul ? mBulueSoul->id : -1;
	ini.Value("equipment", "yellow_soul") = mYellowSoul ? mYellowSoul->id : -1;
}

void Player::LoadEquipmentFrom(HelpTool::INIConfig& ini)
{
	auto getSlotId = [&](const char* key) -> int {
		return ini.HasKey("equipment", key) ? (int)ini.Value("equipment", key) : -1;
	};

	int weapon = getSlotId("weapon");
	int armor = getSlotId("armor");
	int accessory = getSlotId("accessory");
	int redSoul = getSlotId("red_soul");
	int blueSoul = getSlotId("blue_soul");
	int yellowSoul = getSlotId("yellow_soul");

	// 清空槽位（读档时 MainPlayer 通常为新对象，这里做兜底）
	mHand = nullptr;
	mBody = nullptr;
	mAccessory = nullptr;
	mRedSoul = nullptr;
	mBulueSoul = nullptr;
	mYellowSoul = nullptr;

	// 装备依赖背包已就绪。武器/防具/饰品在装备时会被从背包移出，
	// 存档的背包段里不包含它们；读档时需重新克隆一件装备，
	// 否则会错误地从背包再取一件，导致数量变少。
	if (weapon > 0)
	{
		BackPack.AddItem(weapon, 1);
		BackPack.EquipWeapon(weapon);
	}
	if (armor > 0)
	{
		BackPack.AddItem(armor, 1);
		BackPack.EquipArmor(armor);
	}
	if (accessory > 0)
	{
		BackPack.AddItem(accessory, 1);
		BackPack.EquipAccessory(accessory);
	}
	// 魂装备后仍保留在背包中，直接装备即可
	if (redSoul > 0) BackPack.EquipRedSoul(redSoul);
	if (blueSoul > 0) BackPack.EquipBlueSoul(blueSoul);
	if (yellowSoul > 0) BackPack.EquipYellowSoul(yellowSoul);
}
