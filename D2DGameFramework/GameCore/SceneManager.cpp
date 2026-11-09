#include "SceneManager.h"
#include <iostream>
#include "Application.h"
#include "../Player.h"
#include "../S_Bat.h"
#include "../Soma.h"
#include "../GameUI/FloatingNumberPool.h"
#include "../D2DCore/D2DRender.h"
#include <algorithm>

namespace
{
	void ClearSceneTransientState()
	{
		GameObjectMgr.DestroySceneTransientObjects();
		FLOATINGNUMS.Clear();
		FLOATINGNUMS_HIT.Clear();
	}
	void SyncRoomCompanions()
	{
		auto* player = MainPlayer;
		if (!player || player->IsDestroyed()) return;
		if (auto* soma = dynamic_cast<Soma*>(player)) soma->OnRoomEntered();
		if (!player->mBulueSoul) return;
		auto* bat = dynamic_cast<S_Bat*>(player->mBulueSoul->p_skill);
		if (bat && !bat->IsDestroyed()) bat->ResetForRoom();
	}
}

namespace GameCore
{
	SceneManager::~SceneManager()
	{
		for (auto& pair:m_scenes)
		{
			//if (pair.second->IsActive())
			//{
			//	pair.second->OnExit();
			//}
			delete pair.second;
		}
		m_scenes.clear();
		m_sceneStack.clear();

	}

	void SceneManager::SwitchTo(const std::string& name, std::function<void()> onEntered)
	{
		if (mDeferringTransitions)
		{
			if (!HasPendingTransition() && GetScene(name))
			{
				mPendingTransition = Transition::Switch;
				mPendingScene = name;
				mOnEntered = std::move(onEntered);
			}
			return;
		}
		auto it = m_scenes.find(name);
		if (it != m_scenes.end())
		{
			Scene* curScene = GetCurrentScene();
			const bool showTransition = curScene != nullptr;
			if (curScene != nullptr)
			{
				curScene->OnExit();
				curScene->SetActive(false);
			}

			m_sceneStack.clear();
			ClearSceneTransientState();

			Scene* pNext = it->second;
			m_sceneStack.push_back(pNext);
			pNext->SetActive(true);
			pNext->OnEnter();
			if (onEntered) onEntered();
			SyncRoomCompanions();
			// 已完成切换，再让新场景上的黑色遮罩逐渐消失。
			if (showTransition) StartFade();
			else mFadeAlpha = 0.0f;
		}
	}

	void SceneManager::PushScene(const std::string& name)
	{
		if (mDeferringTransitions)
		{
			if (!HasPendingTransition() && GetScene(name))
			{
				mPendingTransition = Transition::Push;
				mPendingScene = name;
			}
			return;
		}
		auto it = m_scenes.find(name);
		if (it!= m_scenes.end())
		{
			Scene* curScene = GetCurrentScene();
			if (curScene != nullptr)
			{
				curScene->OnExit();
				curScene->SetActive(false);
			}

			Scene* pNext = it->second;
			ClearSceneTransientState();
			m_sceneStack.push_back(pNext);
			pNext->SetActive(true);
			pNext->OnEnter();
			SyncRoomCompanions();
		}
	}


	void SceneManager::PopScene()
	{
		if (mDeferringTransitions)
		{
			if (!HasPendingTransition()) mPendingTransition = Transition::Pop;
			return;
		}
		Scene* curScene = GetCurrentScene();
		if (curScene!=nullptr)
		{
			curScene->OnExit();//在弹出场景之间先将场景退出,做一些清理工作
			curScene->SetActive(false);//将场景设置为非激活
		}

		if (!m_sceneStack.empty())
		{
			ClearSceneTransientState();
			m_sceneStack.pop_back();//当栈顶场景(当前场景)弹出
		}

		Scene* pRevScene = GetCurrentScene();
		if (pRevScene != nullptr)
		{
			pRevScene->SetActive(true);//设置场景激活
			pRevScene->OnEnter();//调用场景初始化功能
			SyncRoomCompanions();
		}
	}

	void SceneManager::EndUpdate()
	{
		mDeferringTransitions = false;
		const auto transition = mPendingTransition;
		const std::string name = mPendingScene;
		auto onEntered = std::move(mOnEntered);
		mPendingTransition = Transition::None;
		mPendingScene.clear();
		switch (transition)
		{
		case Transition::Switch: SwitchTo(name, std::move(onEntered)); break;
		case Transition::Push: PushScene(name); break;
		case Transition::Pop: PopScene(); break;
		default: break;
		}
	}

	void SceneManager::UpdateTransition(float deltaTime)
	{
		mFadeAlpha = (std::max)(0.0f, mFadeAlpha - deltaTime / FadeDuration);
	}

	void SceneManager::RenderTransition()
	{
		if (mFadeAlpha <= 0.0f) return;
		auto* target = D2DRd.GetD2DRenderTarget();
		if (!target) return;
		D2D1_MATRIX_3X2_F previous;
		target->GetTransform(&previous);
		target->SetTransform(D2D1::Matrix3x2F::Identity());
		const auto size = target->GetSize();
		D2DRd.DrawRect(0, 0, size.width, size.height, ColorType::COLORF{0, 0, 0, mFadeAlpha});
		target->SetTransform(previous);
	}

	Scene* SceneManager::GetCurrentScene() const
	{
		if (m_sceneStack.empty())
		{
			return nullptr;
		}
		return m_sceneStack.back();
	}

	Scene* SceneManager::GetScene(const std::string& name) const
	{
		auto it = m_scenes.find(name);
		if (it != m_scenes.end())
		{
			return it->second;
		}
		return nullptr;
	}

	void SceneManager::ForwardMessage(UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (HasPendingTransition()) return;
		Scene* curScene = GetCurrentScene();
		if (!curScene || !curScene->IsActive())
		{
			return;
		}

		switch (msg)
		{
		case  WM_KEYDOWN: { curScene->OnKeyDown(wParam); }break;//按键按下事件
		case  WM_KEYUP: { curScene->OnKeyUp(wParam); }break;
		case  WM_MOUSEMOVE: { curScene->OnMouseMove(LOWORD(lParam),HIWORD(lParam)); }break;
		case  WM_LBUTTONDOWN: { curScene->OnMouseDown(MouseButton::M_LBUTTON, LOWORD(lParam), HIWORD(lParam)); }break;
		case  WM_LBUTTONUP: { curScene->OnMouseUp(MouseButton::M_LBUTTON, LOWORD(lParam), HIWORD(lParam)); }break;
		case  WM_MOUSEWHEEL: { curScene->OnMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam)); }break;
		case WM_DESTROY:
		{
			// 窗口销毁，设置运行状态为false并退出消息循环
			App.SetRunning(false);
			PostQuitMessage(0);
			return;
		}
		}

	}
	void SceneManager::Update(float deltaTime)
	{
		Scene* curScene = GetCurrentScene();
		if (!curScene || !curScene->IsActive())
		{
			return;
		}
		curScene->Update(deltaTime);
		//for (auto& pScene : m_sceneStack)
		//{
		//	if (pScene->IsActive())
		//	{
		//		pScene->Update(deltaTime);
		//	}
		//}
	}

	void SceneManager::LaterUpdate(float deltaTime)
	{
		Scene* curScene = GetCurrentScene();
		if (!curScene || !curScene->IsActive())
		{
			return;
		}
		curScene->LaterUpdate(deltaTime);
	}

	void SceneManager::Render(float deltaTime)
	{
		//Scene* curScene = GetCurrentScene();
		//if (!curScene || !curScene->IsActive())
		//{
		//	return;
		//}
		//curScene->Render(deltaTime);
		for (auto& pScene:m_sceneStack)
		{
			if (pScene->IsActive())
			{
				pScene->Render(deltaTime);
			}
		}	
	}
}
