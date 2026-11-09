#include "Debug.h"
#include "../D2DCore/D2DRender.h"
#include "../D2DCore/FontRenderer.h"
#include "../GameCore/GameLoop.h"
#include "../GameCore/Application.h"
#include "../HelpTool/EnCoding.h"
#include "../GamePlay/GameObject.h"
#include <cstdarg>
#include <iostream>

/**
 * @brief 初始化调试系统
 *
 * 创建用于绘制调试信息的D2D字体（Consolas 14pt）。
 * 必须在D2DRender和FontRenderer初始化之后调用。
 *
 * @return 字体创建成功返回true，失败返回false
 */
bool GamePlay::GameDebug::Initialize()
{
	mDebugFontId = D2DCore::FontRenderer::Instance().CreateFont(L"Consolas", 14.0f);
	return mDebugFontId >= 0;
}

/**
 * @brief 输出临时消息到屏幕
 *
 * 格式化字符串并将消息添加到消息列表中，消息将在屏幕左侧显示指定时间，
 * 超时后自动从列表中移除并不再显示。一条日志消息占一行。
 *
 * @param formatstr 格式化字符串（printf风格）
 * @param keeptime 消息保持显示时间（秒）
 * @param color 消息显示颜色
 * @param ... 可变参数列表
 */
void GamePlay::GameDebug::Log2Screen(const char* formatstr, float keeptime, ColorType::COLORF color, ...)
{
	char buffer[512];
	va_list args;
	va_start(args, color);
	vsnprintf(buffer, sizeof(buffer), formatstr, args);
	va_end(args);

	stLogMsg msg;
	msg.log = buffer;
	msg.color = color;
	msg.keeptime = keeptime;
	msg.maketime = GetTickCount64() / 1000.0f;
	mMsgs.push_back(msg);
}

/**
 * @brief 输出日志到控制台
 *
 * 格式化字符串并输出到控制台（printf），不会添加到屏幕消息列表。
 *
 * @param formatstr 格式化字符串（printf风格）
 * @param ... 可变参数列表
 */
void GamePlay::GameDebug::Log2Console(const char* formatstr, ...)
{
	char buffer[512];
	va_list args;
	va_start(args, formatstr);
	vsnprintf(buffer, sizeof(buffer), formatstr, args);
	va_end(args);
	printf("%s\n", buffer);
}

/**
 * @brief 设置是否显示游戏信息
 *
 * 控制是否在屏幕右上角显示FPS、游戏对象数量、碰撞器数量等调试信息。
 *
 * @param show true显示游戏信息，false隐藏
 * @param color 游戏信息的显示颜色，默认白色
 */
void GamePlay::GameDebug::ShowGameInfo(bool show, ColorType::COLORF color)
{
	mShowGameInfo = show;
	mInfoColor = color;
}

/**
 * @brief 更新消息列表
 *
 * 移除已超过保持时间的消息，从消息列表中删除过期消息。
 * 应在每帧调用。
 */
void GamePlay::GameDebug::_UpdateMsgList()
{
	float currentTime = GetTickCount64() / 1000.0f;
	auto it = mMsgs.begin();
	while (it != mMsgs.end())
	{
		if (currentTime - it->maketime > it->keeptime)
		{
			it = mMsgs.erase(it);
		}
		else
		{
			++it;
		}
	}
}

/**
 * @brief 绘制调试消息
 *
 * 当mShowGameInfo为真时，在屏幕右上角绘制FPS、帧间隔、游戏对象总数、
 * 活动对象数、碰撞器数、待渲染对象数、可见对象数等游戏信息（使用mInfoColor颜色）。
 * 然后在屏幕左侧绘制消息列表中所有消息（使用各自存储的颜色）。
 */
void GamePlay::GameDebug::_DrawDebugMsg()
{
	if (mDebugFontId < 0)
	{
		return;
	}

	static std::wstring wstr;
	static char infoBuffer[512] = {};
	float yOffset = 10.0f;

	//显示FPS,游戏对象数量、碰撞器数量、待渲染对象数量、可见对象数量等游戏信息
	if (mShowGameInfo)
	{
		double deltaTime = GameCore::GameLoop::Instance().frameDeltaTime();
		double afps = GameCore::GameLoop::Instance().AFPS();

		float fps = (deltaTime > 0.001f) ? (1.0f / deltaTime) : 0.0f;
		unsigned int objectCount = GameObjectMgr.GameObjectCount();
		unsigned int activeCount = GameObjectMgr.ActiveObjectCount();
		unsigned int collitionCount = GameObjectMgr.CollitionObjectCount();
		unsigned int renderCount = GameObjectMgr.RenderObjectCount();
		unsigned int visibleCount = GameObjectMgr.VisibleObjectCount();		

		int leftx = App.GetWidth() - 200;

		snprintf(infoBuffer, sizeof(infoBuffer), "游戏瞬时帧率: %.1f F/s", fps);
		wstr = HelpTool::EnCodeing::C2W(infoBuffer);
		D2DCore::FontRenderer::Instance().DrawText(
			wstr.c_str(), mDebugFontId, leftx, yOffset, mInfoColor);
		yOffset += 18.0f;

		snprintf(infoBuffer, sizeof(infoBuffer), "平均帧率(3s): %.1lf F/s", afps);
		wstr = HelpTool::EnCodeing::C2W(infoBuffer);
		D2DCore::FontRenderer::Instance().DrawText(
			wstr.c_str(), mDebugFontId, leftx, yOffset, mInfoColor);
		yOffset += 18.0f;

		snprintf(infoBuffer, sizeof(infoBuffer), "游戏对象总数: %u", objectCount);
		wstr = HelpTool::EnCodeing::C2W(infoBuffer);
		D2DCore::FontRenderer::Instance().DrawText(
			wstr.c_str(), mDebugFontId, leftx, yOffset, mInfoColor);
		yOffset += 18.0f;

		snprintf(infoBuffer, sizeof(infoBuffer), "活动对象数量: %u", activeCount);
		wstr = HelpTool::EnCodeing::C2W(infoBuffer);
		D2DCore::FontRenderer::Instance().DrawText(
			wstr.c_str(), mDebugFontId, leftx, yOffset, mInfoColor);
		yOffset += 18.0f;

		snprintf(infoBuffer, sizeof(infoBuffer), "碰撞器对象数: %u", collitionCount);
		wstr = HelpTool::EnCodeing::C2W(infoBuffer);
		D2DCore::FontRenderer::Instance().DrawText(
			wstr.c_str(), mDebugFontId, leftx, yOffset, mInfoColor);
		yOffset += 18.0f;

		snprintf(infoBuffer, sizeof(infoBuffer), "需要渲染对象: %u", renderCount);
		wstr = HelpTool::EnCodeing::C2W(infoBuffer);
		D2DCore::FontRenderer::Instance().DrawText(
			wstr.c_str(), mDebugFontId, leftx, yOffset, mInfoColor);
		yOffset += 18.0f;

		snprintf(infoBuffer, sizeof(infoBuffer), "可见对象数量: %u", visibleCount);
		wstr = HelpTool::EnCodeing::C2W(infoBuffer);
		D2DCore::FontRenderer::Instance().DrawText(
			wstr.c_str(), mDebugFontId, leftx, yOffset, mInfoColor);

		
	}
	yOffset = 500.0f;
	for (auto& msg : mMsgs)
	{
		wstr = HelpTool::EnCodeing::C2W(msg.log.c_str());
		D2DCore::FontRenderer::Instance().DrawText(wstr.c_str(), mDebugFontId, 10.0f, yOffset, msg.color);
		yOffset += 18.0f;
	}
}


void GamePlay::GameDebug::DebugDrawLine(float x1, float y1, float x2, float y2,
	const ColorType::COLORF& color, float strokeWidth)
{
	static D2D1_MATRIX_3X2_F originalTransform;
	static ID2D1HwndRenderTarget* pTarget;

	mDrawDebugItemList.push_back(
		[=]()
		{
			if (!GamePlay::Camera::main())
			{
				return;
			}
			auto vpv = GamePlay::Camera::main()->GetVPV();		
			pTarget = D2DRd.GetD2DRenderTarget();
			if (pTarget!= nullptr)
			{
				// 保存原始变换矩阵
				pTarget->GetTransform(&originalTransform);
				//设置矩阵
				pTarget->SetTransform((D2D1_MATRIX_3X2_F*)(vpv.GetMat3x2()));
				D2DRd.DrawLine(x1, y1, x2, y2, color, strokeWidth);
				pTarget->SetTransform(&originalTransform);
			}
		}
	);
}
void GamePlay::GameDebug::_DrawDebugItems(bool visible)
{
	if (!visible)
	{
		// Discard this frame's commands so hidden debug drawing cannot accumulate.
		mDrawDebugItemList.clear();
		return;
	}
	while (!mDrawDebugItemList.empty())
	{
		auto& myCallBack = mDrawDebugItemList.back();
		myCallBack();
		mDrawDebugItemList.pop_back();
	}

}

