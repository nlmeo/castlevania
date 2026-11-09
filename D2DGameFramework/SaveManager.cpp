#include "SaveManager.h"
#include "HelpTool/INIConfig.h"
#include "GameCore/SceneManager.h"
#include "GameCore/AudioLib.h"
#include "Player.h"
#include "Soma.h"
#include "BackPack.h"
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cerrno>
#include <charconv>
#include <limits>
#include <windows.h>

std::string SaveManager::SlotPath(int slot) const
{
	char buf[64] = {};
	sprintf_s(buf, "save\\slot%d.ini", slot);
	return std::string(buf);
}

void SaveManager::SetCurrentSlot(int slot)
{
	if (IsValidSlot(slot)) mCurrentSlot = slot;
}

int SaveManager::GetCurrentSlot() const
{
	return mCurrentSlot;
}

const std::string& SaveManager::LoadedScene() const
{
	return mLoadedScene;
}

const std::string& SaveManager::LoadedBGM() const
{
	return mLoadedBGM;
}

bool SaveManager::HasSave(int slot)
{
	if (!IsValidSlot(slot)) return false;
	const DWORD attributes = GetFileAttributesA(SlotPath(slot).c_str());
	return attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY);
}

std::string SaveManager::GetSaveTime(int slot) const
{
	if (!IsValidSlot(slot)) return {};
	const auto path = SlotPath(slot);
	WIN32_FILE_ATTRIBUTE_DATA file{};
	if (!GetFileAttributesExA(path.c_str(), GetFileExInfoStandard, &file) ||
		(file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) return {};
	HelpTool::INIConfig ini;
	if (ini.ParseINI(path.c_str()) && ini.HasKey("meta", "saved_at"))
	{
		const std::string timestamp = ini.Value("meta", "saved_at");
		bool valid = timestamp.size() == 19;
		for (size_t i = 0; valid && i < timestamp.size(); ++i)
		{
			const char separator = (i == 4 || i == 7) ? '-' : i == 10 ? ' ' : (i == 13 || i == 16) ? ':' : '\0';
			valid = separator ? timestamp[i] == separator : timestamp[i] >= '0' && timestamp[i] <= '9';
		}
		if (valid) return timestamp;
	}
	FILETIME localTime{};
	SYSTEMTIME savedAt{};
	if (!FileTimeToLocalFileTime(&file.ftLastWriteTime, &localTime) ||
		!FileTimeToSystemTime(&localTime, &savedAt)) return {};
	char timestamp[32]{};
	sprintf_s(timestamp, "%04u-%02u-%02u %02u:%02u:%02u",
		savedAt.wYear, savedAt.wMonth, savedAt.wDay,
		savedAt.wHour, savedAt.wMinute, savedAt.wSecond);
	return timestamp;
}

bool SaveManager::SaveGame(int slot)
{
	if (!IsValidSlot(slot)) return false;
	// 确保 save 目录存在（已存在时返回失败可忽略）
	CreateDirectoryA("save", nullptr);

	Player* player = MainPlayer;
	if (player == nullptr)
		return false;

	HelpTool::INIConfig ini;

	// 玩家属性与位置
	if (auto* soma = dynamic_cast<Soma*>(player))
		soma->SaveTo(ini);

	// 装备槽
	player->SaveEquipmentTo(ini);

	// 背包
	BackPack.SaveTo(ini);

	// 元信息
	ini.Value("meta", "version") = 1;
	ini.Value("meta", "scene") = SceneMgr.GetCurrentSceneName();
	ini.Value("meta", "bgm") = AudioLib.GetCurrentBGM();
	SYSTEMTIME savedAt{};
	GetLocalTime(&savedAt);
	char timestamp[32]{};
	sprintf_s(timestamp, "%04u-%02u-%02u %02u:%02u:%02u",
		savedAt.wYear, savedAt.wMonth, savedAt.wDay,
		savedAt.wHour, savedAt.wMinute, savedAt.wSecond);
	ini.Value("meta", "saved_at") = timestamp;

	const std::string path = SlotPath(slot);
	const std::string temporary = path + ".tmp";
	if (!ini.Save2File(temporary.c_str())) { DeleteFileA(temporary.c_str()); return false; }
	if (!MoveFileExA(temporary.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
	{
		DeleteFileA(temporary.c_str());
		return false;
	}
	return true;
}

bool SaveManager::LoadGame(int slot)
{
	mLoadedScene.clear();
	mLoadedBGM.clear();
	if (!IsValidSlot(slot)) return false;
	HelpTool::INIConfig ini;
	if (!ini.ParseINI(SlotPath(slot).c_str()))
		return false;

	// 版本校验
	if (!ini.HasKey("meta", "version") || (std::string)ini.Value("meta", "version") != "1")
		return false;

	const std::string scene = (std::string)ini.Value("meta", "scene");
	if (scene.size() != 2 || scene[0] != 's' || scene[1] < '0' || scene[1] > '6' || !SceneMgr.GetScene(scene))
		return false;
	if (!ini.HasSection("stats") || !ini.HasSection("position") ||
		!ini.HasSection("backpack") || !ini.HasSection("equipment")) return false;

	// 所有校验必须先于玩家、背包和装备的修改。
	auto validInteger = [](const std::string& text) {
		int value = 0;
		const auto result = std::from_chars(text.data(), text.data() + text.size(), value);
		return result.ec == std::errc() && result.ptr == text.data() + text.size();
	};
	auto validNumber = [&](const char* section, const char* key, bool required, bool integer) {
		if (!ini.HasKey(section, key)) return !required;
		const std::string value = (std::string)ini.Value(section, key);
		if (integer) return validInteger(value);
		if (value.find_first_not_of("+-0123456789.eE") != std::string::npos) return false;
		char* end = nullptr;
		errno = 0;
		const double number = std::strtod(value.c_str(), &end);
		return !value.empty() && end == value.c_str() + value.size() && errno != ERANGE &&
			std::isfinite(number) && std::abs(number) <= (std::numeric_limits<float>::max)();
	};
	if (!validNumber("position", "x", true, false) || !validNumber("position", "y", true, false)) return false;
	for (const char* key : { "hp", "cur_hp", "cur_mp" })
		if (!validNumber("stats", key, true, false)) return false;
	for (const char* key : { "mp", "level" })
		if (!validNumber("stats", key, true, true)) return false;
	for (const char* key : { "exp", "next_level_exp", "gold", "atk", "def", "lck", "str", "con", "intel" })
		if (!validNumber("stats", key, false, true)) return false;
	for (const char* key : { "sp", "jp", "g", "max_jp_height" })
		if (!validNumber("stats", key, false, false)) return false;
	for (const char* key : { "weapon", "armor", "accessory", "red_soul", "blue_soul", "yellow_soul" })
		if (!validNumber("equipment", key, false, true)) return false;
	for (const char* key : { "weapons", "armors", "accessories", "red_souls", "blue_souls", "yellow_souls", "grey_souls", "consumables" })
	{
		if (!ini.HasKey("backpack", key)) continue;
		const std::string items = (std::string)ini.Value("backpack", key);
		for (size_t start = 0; start < items.size();)
		{
			const auto end = items.find(',', start);
			const std::string token = items.substr(start, end == std::string::npos ? end : end - start);
			const auto colon = token.find(':');
			if (colon == std::string::npos) return false;
			const std::string id = token.substr(0, colon), count = token.substr(colon + 1);
			if (!validInteger(id) || !validInteger(count) || std::atoi(id.c_str()) <= 0 || std::atoi(count.c_str()) <= 0) return false;
			if (end == std::string::npos) break;
			start = end + 1;
			if (start == items.size()) return false;
		}
	}
	const float hp = (float)ini.Value("stats", "hp");
	const float currentHp = (float)ini.Value("stats", "cur_hp");
	const int mp = (int)ini.Value("stats", "mp");
	const float currentMp = (float)ini.Value("stats", "cur_mp");
	if (hp <= 0 || currentHp < 0 || currentHp > hp || mp < 0 || currentMp < 0 || currentMp > mp) return false;

	Player* player = MainPlayer;
	if (player == nullptr)
		return false;

	// 玩家属性与位置
	if (auto* soma = dynamic_cast<Soma*>(player))
		soma->LoadFrom(ini);

	// 背包（内部会先清空再重建）
	BackPack.LoadFrom(ini);

	// 装备槽（依赖背包已就绪，会重建 SoulSkill）
	player->LoadEquipmentFrom(ini);

	mLoadedScene = scene;

	mLoadedBGM = ini.HasKey("meta", "bgm") ? (std::string)ini.Value("meta", "bgm") : "";

	return true;
}

bool SaveManager::DeleteSave(int slot)
{
	if (!IsValidSlot(slot)) return false;
	return DeleteFileA(SlotPath(slot).c_str()) != 0;
}

bool SaveManager::CopySave(int srcSlot, int dstSlot)
{
	if (!IsValidSlot(srcSlot) || !IsValidSlot(dstSlot) || srcSlot == dstSlot)
		return false;

	if (!HasSave(srcSlot))
		return false;

	// 目标已存在时不覆盖（bFailIfExists = TRUE）
	return CopyFileA(SlotPath(srcSlot).c_str(), SlotPath(dstSlot).c_str(), TRUE) != 0;
}
