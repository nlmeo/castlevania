#pragma once
#ifndef __GAME_UI_IMENU_H__
#define __GAME_UI_IMENU_H__

#include "UICanvas.h"

namespace GameUI
{
	class MenuStack;

	/**
	 * @brief 菜单接口
	 *
	 * 所有可入栈的菜单（PauseMenu、EquipMenu 等）需实现此接口。
	 * OnEnter 负责在 Canvas 上创建 UI 子节点，OnExit 负责清理。
	 */
	class IMenu
	{
	public:
		virtual ~IMenu() = default;

		/** @brief 进入菜单：在 canvas 上 AddChild 创建所有 UI 子节点 */
		virtual void OnEnter(GameUI::UICanvas* canvas, MenuStack* menuStack) = 0;

		/** @brief 退出菜单：从 canvas 上 RemoveChild 清理所有 UI 子节点 */
		virtual void OnExit(GameUI::UICanvas* canvas) = 0;

		/** @brief 每帧更新 */
		virtual void Update(float deltaTime) = 0;

		/** @brief 按键处理 */
		virtual void OnKeyDown(WPARAM key) = 0;
	};
}

#endif
