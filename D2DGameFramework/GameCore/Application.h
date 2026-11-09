#pragma once
#ifndef __GAME_CORE_APPLICATION_H__
#define __GAME_CORE_APPLICATION_H__

#include <windows.h>
#include <string>
#include "../HelpTool/SingletonTemplate.h"

namespace GameCore
{
	class Application :public Singleton<Application>
	{
		PRIVATE_COPY(Application)
	private:
		/** @brief 构造函数 */
		Application();
	public:
		/** @brief 析构函数 */
		~Application();

		bool Initialize(HINSTANCE hInstance, int nCmdShow, int width, int height, std::string  title);

		/** @brief 获取窗口句柄 */
		HWND GetHwnd() const { return m_hwnd; }

		/** @brief 获取窗口客户区宽度 */
		int GetWidth() const { return m_width; }

		/** @brief 获取窗口客户区高度 */
		int GetHeight() const { return m_height; }

		/** @brief 获取应用程序运行状态 */
		bool IsRunning() const { return m_isRunning; }

		/** @brief 设置应用程序运行状态 */
		void SetRunning(bool running) { m_isRunning = running; }

		bool IsInit() const { return m_isInit; }
	protected:
		void _OnSizeChange(int clientwidth, int clientheight);

	private:
		static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	private:
		bool m_isInit;				///是否初始化
		HWND m_hwnd;               ///< 窗口句柄
		HINSTANCE m_hInstance;     ///< 应用程序实例句柄
		int m_width;               ///< 窗口客户区宽度
		int m_height;              ///< 窗口客户区高度
		bool m_isRunning;          ///< 应用程序运行状态标记
		const wchar_t* WINDOW_CLASS_NAME;  ///< 窗口类名
	};
#define  App GameCore::Application::Instance()
}
#endif // !__GAME_CORE_APPLICATION_H__
