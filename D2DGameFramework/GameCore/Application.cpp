#include "Application.h"
#include <iostream>
#include <string>
#include "../HelpTool/EnCoding.h"
#include "../D2DCore/D2DRender.h"
#include "../GamePlay/GameObject.h"
#include "SceneManager.h"
#include <imm.h>
#pragma comment(lib, "imm32.lib")



namespace GameCore
{

	Application::Application()
		: m_hwnd(nullptr)
		, m_hInstance(nullptr)
		, m_width(0)
		, m_height(0)
		, m_isRunning(false)
		, WINDOW_CLASS_NAME(L"D2DGameFrameworkWindow")
		, m_isInit(false)
	{
	}

	Application::~Application()
	{
		if (m_hwnd)
		{
			DestroyWindow(m_hwnd);
			m_hwnd = nullptr;
		}

		if (m_hInstance)
		{
			UnregisterClass(WINDOW_CLASS_NAME, m_hInstance);
		}
	}

	
	bool Application::Initialize(HINSTANCE hInstance, int nCmdShow, int width, int height, std::string title)
	{
		// 控制台输出设为 UTF-8，配合 /utf-8 编译选项解决中文乱码
		SetConsoleOutputCP(CP_UTF8);

		m_hInstance = hInstance;
		m_width = width;
		m_height = height;


		WNDCLASSEXW wc = {};
		wc.cbSize = sizeof(WNDCLASSEXW);
		wc.style = CS_HREDRAW | CS_VREDRAW;  
		wc.lpfnWndProc = WndProc;             
		wc.hInstance = hInstance;             
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);  
		wc.hbrBackground = nullptr;          
		wc.lpszClassName = WINDOW_CLASS_NAME; 

		if (!RegisterClassExW(&wc))
		{
			return false;
		}

		RECT rect = { 0, 0, width, height };
		AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

		m_hwnd = CreateWindowExW(
			0,                              
			WINDOW_CLASS_NAME,              
			HelpTool::EnCodeing::C2W(title.c_str()).c_str(),                          
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			rect.right - rect.left,
			rect.bottom - rect.top,
			nullptr,
			nullptr,
			hInstance,
			this
		);

		if (!m_hwnd)
		{
			return false;
		}

		// 游戏窗口使用按键操作，不需要文字输入；避免字母键触发输入法候选/表情组件。
		// 仅解除此窗口的输入上下文，不修改用户的系统输入法设置。
		ImmAssociateContext(m_hwnd, nullptr);

		int screex = GetSystemMetrics(SM_CXSCREEN);
		int screey = GetSystemMetrics(SM_CYSCREEN);

		MoveWindow(m_hwnd,
			(screex - (rect.right - rect.left)) / 2,
			(screey - (rect.bottom - rect.top)) / 2,
			rect.right - rect.left,
			rect.bottom - rect.top,
			TRUE);

		ShowWindow(m_hwnd, nCmdShow);
		UpdateWindow(m_hwnd);

		m_isRunning = true;
		m_isInit = true;
		return true;
	}

	void Application::_OnSizeChange(int clientwidth, int clientheight)
	{
		std::cout << "当前屏幕宽高" << clientwidth << " x " << clientheight << std::endl;
		m_width = clientwidth;
		m_height = clientheight;

		D2DRd.Resize(clientwidth, clientheight);

		GamePlay::Camera* pcam = GamePlay::Camera::main();
		if (pcam!=nullptr)
		{
			pcam->OnAppSizeChange(clientwidth,clientheight);
		}

		GameCore::Scene* pScene = SceneMgr.GetCurrentScene();
		if (pScene != nullptr)
		{
			GameUI::UICanvas* pCanvas = pScene->GetUICanvas();
			if (pCanvas != nullptr)
			{
				pCanvas->Resize(clientwidth, clientheight);
			}
		}
	}

	
	LRESULT CALLBACK Application::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
			case WM_SIZE:
			{
				App._OnSizeChange(LOWORD(lParam), HIWORD(lParam));
				return 0;
			}

			case WM_DESTROY:
			{
				App.SetRunning(false);
				PostQuitMessage(0);
				return 0;
			}
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

}
