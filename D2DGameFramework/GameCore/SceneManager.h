#pragma once
#ifndef __GAME_CORE_SCENE_MANAGER_H__
#define __GAME_CORE_SCENE_MANAGER_H__

#include <windows.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <functional>

#include "../HelpTool/SingletonTemplate.h"
#include "Scene.h"

namespace GameCore
{
	/**
	 * @brief 场景管理器
	 *
	 * 单例类，负责场景的创建、管理和切换，支持场景栈操作
	 */
	class SceneManager : public Singleton<SceneManager>
	{
		PRIVATE_CONSTRUCTOR_AND_COPY(SceneManager)

	public:
		/** @brief 析构函数 */
		~SceneManager();

		/**
		 * @brief 创建场景（模板版本）
		 *
		 * @tparam T 场景类型
		 * @param name 场景名称
		 * @return 创建的场景指针，失败返回nullptr
		 */
		template<typename T>
		class Scene* CreateScene(const std::string& name)
		{
			auto it = m_scenes.find(name);
			if (it != m_scenes.end())
			{
				std::cout << "Scene name already registered" << std::endl;
				return nullptr;
			}
			Scene* scene = new T();
			scene->SetName(name);
			return m_scenes[name] = scene;
		}

		/**
		 * @brief 切换到指定场景
		 *
		 * 结束当前场景并切换到新场景
		 *
		 * @param name 场景名称
		 */
		void SwitchTo(const std::string& name, std::function<void()> onEntered = {});

		/**
		 * @brief 推入场景到栈顶
		 *
		 * 将新场景压入栈顶，保留当前场景
		 *
		 * @param name 场景名称
		 */
		void PushScene(const std::string& name);

		/** @brief 弹出栈顶场景 */
		void PopScene();

		/** @brief 更新期间只记录切换请求，在 EndUpdate 的安全边界执行。 */
		void BeginUpdate() { mDeferringTransitions = true; }
		void EndUpdate();
		bool HasPendingTransition() const { return mPendingTransition != Transition::None; }
		void UpdateTransition(float deltaTime);
		void RenderTransition();
		/** @brief 从全黑淡出，供场景和菜单切换共用。 */
		void StartFade() { mFadeAlpha = 1.0f; }

		/** @brief 获取当前活动场景 */
		class Scene* GetCurrentScene() const;

		/** @brief 获取当前活动场景名称（无场景时返回空串） */
		std::string GetCurrentSceneName() const
		{
			Scene* s = GetCurrentScene();
			return s ? s->GetName() : std::string();
		}

		/**
		 * @brief 根据名称获取场景
		 *
		 * @param name 场景名称
		 * @return 场景指针，不存在返回nullptr
		 */
		class Scene* GetScene(const std::string& name) const;

		/**
		 * @brief 更新当前场景
		 *
		 * @param deltaTime 帧间隔时间（秒）
		 */
		void Update(float deltaTime);


		void LaterUpdate(float deltaTime);
		/**
		 * @brief 渲染当前场景
		 *
		 * @param deltaTime 帧间隔时间（秒）
		 */
		void Render(float deltaTime);

		/**
		 * @brief 转发消息到当前场景
		 *
		 * @param msg 消息类型
		 * @param wParam 消息参数
		 * @param lParam 消息参数
		 */
		void ForwardMessage(UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		enum class Transition { None, Switch, Push, Pop };
		bool mDeferringTransitions = false;
		// 切换后黑色遮罩消失的时长（秒），不影响世界更新与输入。
		static constexpr float FadeDuration = 0.5f;
		float mFadeAlpha = 0.0f;
		Transition mPendingTransition = Transition::None;
		std::string mPendingScene;
		std::function<void()> mOnEntered;
		std::unordered_map<std::string, class Scene*> m_scenes;  ///< 场景注册表
		std::vector<class Scene*> m_sceneStack;                   ///< 场景栈
	};

	/** @brief 场景管理器宏定义 */
#define SceneMgr GameCore::SceneManager::Instance()
}
#endif
