#pragma once
#ifndef __GAME_CORE_INPUT_MANAGER_H__
#define __GAME_CORE_INPUT_MANAGER_H__

#include <windows.h>
#include "../HelpTool/SingletonTemplate.h"

namespace GameCore
{
	/**
	 * @brief 按键状态枚举
	 *
	 * 定义按键的不同状态，支持位运算组合判断。
	 */
	enum class emKeyState
	{
		KS_UNKNOWN = 0,    ///< 未定义的状态
		KS_KEEP_UP = 1,    ///< 持续抬起状态
		KS_JUST_UP = 2,    ///< 刚刚抬起
		KS_KEEP_DOWN = 4,  ///< 持续按下状态
		KS_JUST_DOWN = 8   ///< 刚刚按下
	};

	/**
	 * @brief 输入管理器类
	 *
	 * 提供单例模式的输入管理功能，负责处理键盘和鼠标输入。
	 */
	class InputMgr : public Singleton<InputMgr>
	{
		PRIVATE_COPY(InputMgr);

	private:
		/** @brief 私有构造函数 */
		InputMgr();

	public:
		/**
		 * @brief 初始化输入管理器
		 *
		 * @return 初始化成功返回true，失败返回false
		 */
		bool Initialize();

		/** @brief 更新所有按键状态 */
		void UpdateKeyState();

		/**
		 * @brief 获取指定按键的状态
		 *
		 * @param vk 虚拟按键码
		 * @return 按键状态
		 */
		emKeyState KeyState(unsigned char vk);

		/**
		 * @brief 获取指定按键的状态（数组索引方式）
		 *
		 * @param vk 虚拟按键码
		 * @return 按键状态
		 */
		emKeyState operator [](unsigned char vk);

		/**
		 * @brief 获取鼠标位置
		 *
		 * @return 鼠标坐标点
		 */
		const POINT& GetMousePos();

		/**
		 * @brief 检查按键是否按下
		 *
		 * @param vk 虚拟按键码
		 * @return 按下返回true，否则返回false
		 */
		bool KeyDown(unsigned char vk);

		/**
		 * @brief 检查按键是否刚刚按下
		 *
		 * @param vk 虚拟按键码
		 * @return 刚刚按下返回true，否则返回false
		 */
		bool KeyJustDown(unsigned char vk);

		/**
		 * @brief 检查按键是否抬起
		 *
		 * @param vk 虚拟按键码
		 * @return 抬起返回true，否则返回false
		 */
		bool KeyUp(unsigned char vk);

		/**
		 * @brief 检查按键是否刚刚抬起
		 *
		 * @param vk 虚拟按键码
		 * @return 刚刚抬起返回true，否则返回false
		 */
		bool KeyJustUp(unsigned char vk);

	private:
		emKeyState mKeyStates[256];  ///< 存储所有按键状态
		HWND mHwnd;                   ///< 关联的窗口句柄
	};

	#define GameInput GameCore::InputMgr::Instance()
}

#endif
