#include "GameLoop.h"
#include "Application.h"
#include "../D2DCore/D2DRender.h"
#include "SceneManager.h"
#include "InputMgr.h"
#include "../GamePlay/SpriteRender.h"
#include "../GamePlay/GameObject.h"
#include "../GamePlay/Debug.h"
#include "../StartScene.h"
#include "../SoundConfigScene.h"
#include "../SelectDataScene.h"
#include "../TopScene.h"
#include "../S0.h"
#include "../S1.h"
#include "../S2.h"
#include "../S3.h"
#include "../S4.h"
#include "../S5.h"
#include "../S6.h"
#include "../GameOverScene.h"
#include "../GameUI/UICanvas.h"
#include "../GameUI/FloatingNumberPool.h"
#include "../BackPack.h"
#include "../ItemConfig.h"
#include <algorithm>
#include <cmath>
namespace GameCore
{
	GameLoop::GameLoop():m_FrameRate(0), m_FrameTime(0.0), m_lastTime(0),
		m_TimeDelta(0), m_GameStartTime(0), m_TimeSenceGameStart(0),
		m_Last3Second(0), m_FrameCount(0), m_FPS_av(0)
	{

	}


	void GameLoop::SetFrameRate(size_t fps)
	{
		m_FrameRate = fps;
		m_FrameTime = 0;

		if (m_FrameRate>0)
		{
			m_FrameTime = 1000.0 / static_cast<double>(m_FrameRate);
		}
	}
	
	void GameLoop::UpdateFrame(float deltaTime)
	{
		// 全局开关：不依赖场景，也不受菜单暂停影响。
		if (GameInput.KeyJustDown('P'))
		{
			mDebugRenderEnabled = !mDebugRenderEnabled;
		}
		SceneMgr.UpdateTransition(deltaTime);
		SceneMgr.BeginUpdate();
		auto* previousScene = SceneMgr.GetCurrentScene();
		const bool wasPaused = previousScene && previousScene->IsWorldPaused();
		if (!SceneMgr.HasPendingTransition()) SceneMgr.Update(deltaTime);
		auto* scene = SceneMgr.GetCurrentScene();
		// 关闭菜单的按键属于 UI，不在同一帧继续触发世界中的技能。
		const bool paused = wasPaused || (scene && scene->IsWorldPaused());
		if (!paused && !SceneMgr.HasPendingTransition()) GameObjectMgr._UpdateAllObjects(deltaTime);
		if (!SceneMgr.HasPendingTransition()) GameObjectMgr._MergeNewObjects();
		if (!paused && !SceneMgr.HasPendingTransition())
		{
			GameObjectMgr._CollitionAndTriggerEvent();
			if (!SceneMgr.HasPendingTransition()) GameObjectMgr._LaterUpdateAllObjects(deltaTime);
			if (!SceneMgr.HasPendingTransition()) SceneMgr.LaterUpdate(deltaTime);
		}
		SceneMgr.EndUpdate();
		GameObjectMgr._MergeNewObjects();
		if (!paused)
		{
			if (MainCamera)
			{
				MainCamera->ShakeCameraUpdate(deltaTime);
				MainCamera->FllowCameraUpdate(deltaTime);
				MainCamera->UpdateCamera();
			}
			FLOATINGNUMS.Update(deltaTime);
			FLOATINGNUMS_HIT.Update(deltaTime);
		}
		GameObjectMgr._PrepareRenderObjects();
	}

	bool GameLoop::ShouldRenderWorldOverlays() const
	{
		auto* scene = SceneMgr.GetCurrentScene();
		return scene && !scene->IsWorldPaused();
	}

	void GameLoop::Run()
	{
		m_TimeSenceGameStart = 0;
		//创建场景的方法改变为模板函数
		SceneMgr.CreateScene<StartScene>("start");
		SceneMgr.CreateScene<SoundConfigScene>("sound_config");
		SceneMgr.CreateScene<SelectDataScene>("select_data");
		SceneMgr.CreateScene<TopScene>("top");
		SceneMgr.CreateScene<S0>("s0");
		SceneMgr.CreateScene<S1>("s1");
		SceneMgr.CreateScene<S2>("s2");
		SceneMgr.CreateScene<S3>("s3");
		SceneMgr.CreateScene<S4>("s4");
		SceneMgr.CreateScene<S5>("s5");
		SceneMgr.CreateScene<S6>("s6");
		SceneMgr.CreateScene<GameOverScene>("game_over");

		SceneMgr.SwitchTo("start");

		CItemConfig::GetInstance().Init();
		// 背包不再于启动时填充：改由 SelectDataScene 在新建/读档时决定


		MSG msg = {};
		m_Last3Second = m_GameStartTime = GetTickCount64();
		LARGE_INTEGER frequency, counter;
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&counter);
		m_lastTime = counter.QuadPart;
		double nextFrameTime = static_cast<double>(counter.QuadPart) / frequency.QuadPart;
		ULONGLONG deltatime3second;
		while (App.IsRunning())
		{
			SceneMgr.BeginUpdate();
			// 处理Windows消息 
			while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
				{
					App.SetRunning(false);
					break;
				}

				TranslateMessage(&msg);
				DispatchMessage(&msg);//这函数执行了以后会调用wndProc回调函数处理派发的消息
				//理论上讲可以将场景的消息事件放在回调函数中,但是我这里所有场景相关的都放在
				//消息循环中处理,所以才在这里直接调用下面的函数
				//转发按键和鼠标消息给场景管理器
				SceneMgr.ForwardMessage(msg.message, msg.wParam, msg.lParam);
			}

			if (!App.IsRunning())
			{
				SceneMgr.EndUpdate();
				break;
			}

			
			// 获取当前时间并计算deltaTime
			ULONGLONG currentTime = GetTickCount64();
			m_TimeSenceGameStart = (currentTime - m_GameStartTime)/1000.0f;

			QueryPerformanceCounter(&counter);
			const double now = static_cast<double>(counter.QuadPart) / frequency.QuadPart;
			if (m_FrameRate > 0 && now < nextFrameTime)
			{
				SceneMgr.EndUpdate();
				const DWORD waitMs = static_cast<DWORD>(std::ceil((nextFrameTime - now) * 1000.0));
				MsgWaitForMultipleObjectsEx(0, nullptr, waitMs, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
				continue;
			}
			m_TimeDelta = (std::min)(0.1, static_cast<double>(counter.QuadPart - m_lastTime) / frequency.QuadPart);
			m_lastTime = counter.QuadPart;
			nextFrameTime += m_FrameTime / 1000.0;
			if (nextFrameTime <= now) nextFrameTime = now + m_FrameTime / 1000.0;
			

			{
				GameInput.UpdateKeyState();
				//更新场景和游戏对象
				UpdateFrame(static_cast<float>(m_TimeDelta));
				Debug._UpdateMsgList();
				//渲染
				D2DRd.BeginDraw();
				GameObjectMgr._RenderObjects();
				SceneMgr.Render(m_TimeDelta);
				if (mDebugRenderEnabled) GameObjectMgr._OnDebugRender();
				Debug._DrawDebugItems(mDebugRenderEnabled);
				if (mDebugRenderEnabled) Debug._DrawDebugMsg();
				// 世界浮动数字位于场景 UI 下方；暂停时隐藏，恢复后继续动画。
				if (ShouldRenderWorldOverlays())
				{
					FLOATINGNUMS.Render();
					FLOATINGNUMS_HIT.Render();
				}
				//UI画布渲染（必须在BeginDraw/EndDraw之间）
				if (auto* curScene = SceneMgr.GetCurrentScene())
					curScene->RenderUICanvas(m_TimeDelta);
				SceneMgr.RenderTransition();
				D2DRd.EndDraw();

				++m_FrameCount;//累计帧数
				deltatime3second = currentTime - m_Last3Second;
				if (deltatime3second>=3000)
				{
					m_FPS_av = m_FrameCount * 1000.0 / deltatime3second;//计算平均帧数
					m_FrameCount = 0;
					m_Last3Second = currentTime;
				}
				
			}		
		}
	}
}
