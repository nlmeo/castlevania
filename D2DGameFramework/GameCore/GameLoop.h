#pragma once
#ifndef __GAME_CORE_LOOP_H__
#define __GAME_CORE_LOOP_H__

#include <windows.h>
#include "../HelpTool/SingletonTemplate.h"

namespace GameCore
{
	/**
	 * @brief 游戏主循环类
	 *
	 * 提供单例模式的游戏主循环功能，负责控制游戏帧率和帧更新。
	 */
	class GameLoop : public Singleton<GameLoop>
	{
		friend class Scene;
		PRIVATE_COPY(GameLoop)

	public:
		/** @brief 构造函数 */
		GameLoop();

		/** @brief 析构函数 */
		~GameLoop() = default;

		/** @brief 启动游戏主循环 */
		void Run();
		/** @brief 推进一帧逻辑；暂停仅冻结世界，场景/UI 仍更新。 */
		void UpdateFrame(float deltaTime);
		bool ShouldRenderWorldOverlays() const;

		/**
		 * @brief 设置帧率
		 *
		 * @param fps 帧率值，0表示不限制帧率
		 */
		void SetFrameRate(size_t fps);

		double frameDeltaTime() const { return m_TimeDelta; }

		double AFPS() const { return m_FPS_av; }

	private:
		bool mDebugRenderEnabled = false;
		size_t m_FrameRate;        ///< 帧率
		double m_FrameTime;        ///< 帧时间（毫秒）
		LONGLONG m_lastTime;      ///< 上一帧的高精度计数
		double m_TimeDelta;//帧间隔时间
		ULONGLONG m_GameStartTime;//游戏开始时间
		double m_TimeSenceGameStart;//游戏到现在经过的秒数
		ULONGLONG m_Last3Second;//过去3秒
		ULONGLONG m_FrameCount;//游戏开始到现在经过的总帧数
		double m_FPS_av;//平均帧率

		
	};

#define GameLooper GameCore::GameLoop::Instance()
}

#endif
