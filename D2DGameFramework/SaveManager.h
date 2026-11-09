#pragma once
#ifndef __GAME_SAVE_MANAGER_H__
#define __GAME_SAVE_MANAGER_H__

#include <string>
#include "HelpTool/SingletonTemplate.h"

/**
 * @brief 存档管理器
 *
 * 单例类，负责三个存档槽的读写与状态维护。
 * 存档格式为 INI 文件（save/slotN.ini），序列化细节由
 * Soma / Player / CBackPackManager 各自实现。
 */
class SaveManager : public Singleton<SaveManager>
{
	PRIVATE_CONSTRUCTOR_AND_COPY(SaveManager)

public:
	/** @brief 存档槽数量（对应 SelectDataScene 的 3 个选项） */
	static const int MAX_SLOT = 3;

	/** @brief 记录当前选中的槽位 */
	void SetCurrentSlot(int slot);

	/** @brief 获取当前选中的槽位 */
	int GetCurrentSlot() const;

	/** @brief 保存当前进度到 slotN.ini */
	bool SaveGame(int slot);

	/** @brief 读取 slotN.ini 并应用到 MainPlayer */
	bool LoadGame(int slot);

	/** @brief 是否存在有效存档 */
	bool HasSave(int slot);
	/** @brief 只读取选档预览时间；旧存档回退到文件最后修改时间。 */
	std::string GetSaveTime(int slot) const;

	/** @brief 删除槽位存档 */
	bool DeleteSave(int slot);

	/** @brief 复制槽位存档（src -> dst），目标已存在时失败 */
	bool CopySave(int srcSlot, int dstSlot);

	/** @brief 读档后返回的目标场景名 */
	const std::string& LoadedScene() const;

	/** @brief 读档后返回的背景音乐路径（空串表示无） */
	const std::string& LoadedBGM() const;

	/** @brief 选档界面的操作模式 */
	enum class Mode { Select = 0, Copy = 1, Delete = 2 };

	/** @brief 设置选档界面的操作模式（TopScene 进入选档界面时使用） */
	void SetMode(Mode m) { mMode = m; }

	/** @brief 获取选档界面的操作模式 */
	Mode GetMode() const { return mMode; }

private:
	static bool IsValidSlot(int slot) { return slot >= 0 && slot < MAX_SLOT; }
	/** @brief 根据槽位生成存档文件相对路径 */
	std::string SlotPath(int slot) const;

	int mCurrentSlot = 0;      ///< 当前选中的槽位
	std::string mLoadedScene;  ///< 读档后要进入的场景名
	std::string mLoadedBGM;    ///< 读档后要恢复的背景音乐路径
	Mode mMode = Mode::Select; ///< 选档界面的操作模式
};

/** @brief 存档管理器宏定义 */
#define SaveMgr SaveManager::Instance()

#endif // !__GAME_SAVE_MANAGER_H__
