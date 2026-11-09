#pragma once
#ifndef __GAME_UI_MENU_STACK_H__
#define __GAME_UI_MENU_STACK_H__

#include "IMenu.h"
#include <vector>
#include <memory>

namespace GameUI
{
	/**
	 * @brief 菜单栈管理器
	 *
	 * 管理 IMenu 的入栈/出栈，仅驱动栈顶菜单的 Update/Render。
	 * 底层通过 OnEnter/OnExit 控制 UI 子节点在 Canvas 上的挂载与卸载，
	 * Canvas 的 Render/Update 循环无需感知 MenuStack 的存在。
	 *
	 * 使用方式：
	 *   MenuStack stack;
	 *   stack.SetCanvas(canvas);
	 *   stack.Push(std::make_unique<PauseMenu>());  // 打开一级菜单
	 *   stack.Push(std::make_unique<EquipMenu>());  // 进入二级子菜单
	 *   stack.Pop();                                 // 返回上一级
	 *   stack.Clear();                               // 关闭所有菜单
	 */
	class MenuStack
	{
	public:
		MenuStack() = default;
		~MenuStack();

		/** @brief 设置关联的 Canvas */
		void SetCanvas(UICanvas* canvas) { mCanvas = canvas; }

		/** @brief 菜单栈是否为空（空 = 无菜单显示 = 游戏模式） */
		bool IsEmpty() const { return mStack.empty(); }

		/**
		 * @brief 入栈新菜单
		 * @param menu 菜单实例（转移所有权）
		 */
		void Push(std::unique_ptr<IMenu> menu);

		/** @brief 出栈当前菜单，回到上一级 */
		void Pop();

		/** @brief 清空整个菜单栈，回到游戏 */
		void Clear(bool showTransition = true);

		/** @brief 驱动栈顶菜单的 Update */
		void Update(float deltaTime);

		/** @brief 仅将按键分发给栈顶菜单 */
		void OnKeyDown(WPARAM key);

	private:
		std::vector<std::unique_ptr<IMenu>> mStack;
		UICanvas* mCanvas = nullptr;
	};
}

#endif
