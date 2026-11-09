#pragma once
#ifndef __GAME_PLAY_FRAME_DEBUG_H__
#define __GAME_PLAY_FRAME_DEBUG_H__

#include "../HelpTool/SingletonTemplate.h"
#include "../TypeDefine/Color.h"
#include "../GameCore/GameLoop.h"
#include <list>
#include <string>
#include <vector>
#include <functional>

namespace GamePlay
{
	/**
	 * @brief 日志消息结构体
	 *
	 * 存储单条调试日志的信息，包括内容、颜色、创建时间和保留时间
	 */
	struct stLogMsg
	{
		ColorType::COLORF color;  ///< 日志颜色
		std::string log;          ///< 日志内容
		float maketime;           ///< 创建时间（秒）
		float keeptime;           ///< 保留时间（秒）
	};

	/**
	 * @brief 调试工具类
	 *
	 * 单例类，提供屏幕日志输出、控制台日志和游戏信息显示功能
	 */
	class GameDebug : public Singleton<GameDebug>
	{
		friend class GameCore::GameLoop;
		PRIVATE_CONSTRUCTOR_AND_COPY(GameDebug)

	public:
		/**
		 * @brief 初始化调试工具
		 *
		 * 初始化D2D字体用于文字输出
		 *
		 * @return 是否初始化成功
		 */
		bool Initialize();

		/**
		 * @brief 添加日志到屏幕显示
		 *
		 * 将日志消息添加到消息队列，在屏幕左侧显示一段时间后自动移除
		 *
		 * @param formatstr 格式化字符串
		 * @param keeptime 保留时间（秒）
		 * @param color 日志颜色
		 * @param ... 可变参数（格式化参数）
		 */
		void Log2Screen(const char* formatstr, float keeptime, ColorType::COLORF color, ...);

		/**
		 * @brief 输出日志到控制台
		 *
		 * 仅输出到控制台，不加入屏幕消息队列
		 *
		 * @param formatstr 格式化字符串
		 * @param ... 可变参数（格式化参数）
		 */
		void Log2Console(const char* formatstr, ...);

		/**
		 * @brief 设置是否显示游戏信息
		 *
		 * 显示FPS、游戏对象数量、激活对象数量、可视对象数量等信息到屏幕右上角
		 *
		 * @param show 是否显示
		 * @param color 信息颜色（默认白色）
		 */
		void ShowGameInfo(bool show = true, ColorType::COLORF color = ColorType::COLORF::White());

		void DebugDrawLine(float x1, float y1, float x2, float y2,
			const ColorType::COLORF& color, float strokeWidth = 1.0f);


	protected:
		/** @brief 更新消息队列，移除超时消息 */
		void _UpdateMsgList();

		/** @brief 绘制调试消息到屏幕 */
		void _DrawDebugMsg();

		//绘制调试元素
		void _DrawDebugItems(bool visible = true);

	protected:
		bool mShowGameInfo = false;   ///< 是否显示游戏信息
		ColorType::COLORF mInfoColor = ColorType::COLORF::White();  ///< 信息颜色
		int mDebugFontId = -1;        ///< 调试字体ID
		std::list<stLogMsg> mMsgs;    ///< 消息队列

		std::vector<std::function<void()>> mDrawDebugItemList;//绘制调试元素函数列表
	};

	/** @brief 调试工具宏定义 */
#define Debug GamePlay::GameDebug::Instance()
}
#endif
