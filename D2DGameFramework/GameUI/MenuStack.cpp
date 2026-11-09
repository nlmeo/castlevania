#include "MenuStack.h"
#include "../GameCore/SceneManager.h"

namespace GameUI
{
	MenuStack::~MenuStack()
	{
		Clear(false);
	}

	void MenuStack::Push(std::unique_ptr<IMenu> menu)
	{
		if (!mCanvas || !menu)
			return;

		// 隐藏当前栈顶菜单
		if (!mStack.empty())
			mStack.back()->OnExit(mCanvas);

		// 入栈并显示新菜单
		mStack.push_back(std::move(menu));
		mStack.back()->OnEnter(mCanvas, this);
		SceneMgr.StartFade();
	}

	void MenuStack::Pop()
	{
		if (mStack.empty() || !mCanvas)
			return;

		// 退出并销毁栈顶菜单
		mStack.back()->OnExit(mCanvas);
		mStack.pop_back();

		// 恢复上一级菜单
		if (!mStack.empty())
			mStack.back()->OnEnter(mCanvas, this);
		SceneMgr.StartFade();
	}

	void MenuStack::Clear(bool showTransition)
	{
		const bool hadMenus = !mStack.empty();
		while (!mStack.empty())
		{
			if (mCanvas)
				mStack.back()->OnExit(mCanvas);
			mStack.pop_back();
		}
		if (showTransition && hadMenus) SceneMgr.StartFade();
	}

	void MenuStack::Update(float deltaTime)
	{
		if (!mStack.empty())
			mStack.back()->Update(deltaTime);
	}

	void MenuStack::OnKeyDown(WPARAM key)
	{
		if (!mStack.empty())
			mStack.back()->OnKeyDown(key);
	}
}
