#pragma once
#ifndef __MY_CONSOLE_H__
#define __MY_CONSOLE_H__

#include "SingletonTemplate.h"
#include <iostream>

namespace HelpTool
{
	/**
	 * @brief 控制台管理类
	 *
	 * 提供单例模式的控制台管理功能，支持显示和释放控制台窗口。
	 */
	class MyConsole : public Singleton<MyConsole>
	{
		PRIVATE_CONSTRUCTOR_AND_COPY(MyConsole)

	public:
		/** @brief 显示控制台窗口 */
		void ShowConsole();

		/** @brief 释放控制台窗口 */
		void FreeConsole();

	protected:
	private:
		FILE* stream_in = nullptr;   ///< 输入流指针
		FILE* stream_out = nullptr;  ///< 输出流指针
	};

	#define MYConsole MyConsole::Instance()
}

#endif