#include "InputMgr.h"
#include "Application.h"

namespace GameCore
{
	InputMgr::InputMgr()
	{
		mHwnd = nullptr;
		for (int i = 0; i < 256; ++i)
		{
			mKeyStates[i] = emKeyState::KS_KEEP_UP;
		}
	}

	bool InputMgr::Initialize()
	{
		if (!App.IsInit())
		{
			MessageBox(nullptr,
				TEXT("请保证 Application::Initialize 函数优先初始化成功后再调用 InputMgr::Init 函数!"),
				TEXT("InputMgr初始化提示!"), MB_OK);
			return false;
		}
		mHwnd = App.GetHwnd();
		return true;
	}

	void InputMgr::UpdateKeyState()
	{
		for (int i = 0; i < 256; ++i)
		{
			if (GetAsyncKeyState(i) & 0x8000)
			{
				////如果按照传统的写法
				//if (emKeyState::KS_KEEP_UP == mKeyStates[i] || 
				//	emKeyState::KS_JUST_UP == mKeyStates[i])
				//{
				//	mKeyStates[i] = emKeyState::KS_JUST_DOWN;
				//}
				//else
				//{
				//	mKeyStates[i] = emKeyState::KS_KEEP_DOWN;
				//}
				mKeyStates[i] = ((int)mKeyStates[i] & 3) ? emKeyState::KS_JUST_DOWN : emKeyState::KS_KEEP_DOWN;
			}
			else
			{
				mKeyStates[i] = ((int)mKeyStates[i] & 12) ? emKeyState::KS_JUST_UP : emKeyState::KS_KEEP_UP;
			}
		}
	}

	emKeyState InputMgr::KeyState(unsigned char vk)
	{
		return mKeyStates[vk];
	}

	emKeyState InputMgr::operator[](unsigned char vk)
	{
		return mKeyStates[vk];
	}

	const POINT& InputMgr::GetMousePos()
	{
		static POINT pos;
		//得到的是屏幕空间的鼠标位置
		GetCursorPos(&pos);
		//得到了窗口客户区的鼠标位置
		ScreenToClient(mHwnd, &pos);
		return pos;
	}

	bool InputMgr::KeyDown(unsigned char vk)
	{
		return (int)mKeyStates[vk] & 12;
	}

	bool InputMgr::KeyJustDown(unsigned char vk)
	{
		return (int)mKeyStates[vk] & 8;
	}

	bool InputMgr::KeyUp(unsigned char vk)
	{
		return (int)mKeyStates[vk] & 3;
	}

	bool InputMgr::KeyJustUp(unsigned char vk)
	{
		return (int)mKeyStates[vk] & 2;
	}
}