#pragma once
#ifndef __GAME_CORE_SCENE_H__
#define __GAME_CORE_SCENE_H__
#include <windows.h>
#include <string>
#include <memory>
#include "../GameUI/UICanvas.h"


namespace GameCore
{
	/** @brief 鼠标按键枚举 */
	typedef enum class emMouseButton
	{
		M_LBUTTON,
		M_RBUTTON,
		M_MBUTTON
	}MouseButton;

	/**
 * @brief 场景基类
 *
 * 定义场景的生命周期接口，子类继承实现具体场景逻辑
 */
	class Scene
	{
	public:
		/** @brief 构造函数 */
		Scene();
		/** @brief 析构函数 */
		virtual ~Scene(){};

		/** @brief 场景进入时调用 */
		virtual void OnEnter() {}

		/** @brief 场景退出时调用 */
		virtual void OnExit() {}

		/**
		 * @brief 每帧逻辑更新
		 *
		 * @param deltaTime 帧间隔时间（秒）
		 */
		virtual void Update(float deltaTime) {}

		virtual void LaterUpdate(float deltaTime) {}

		/** @brief 暂停世界逻辑，UI 仍按真实帧时间更新。 */
		virtual bool IsWorldPaused() const { return false; }

		/**
		 * @brief 每帧渲染
		 *
		 * @param renderer 渲染器引用
		 */
		virtual void Render(float deltaTime) {}



		/**
		 * @brief 键盘按下事件（转发至UI画布）
		 *
		 * @param key 按键代码
		 */
		virtual void OnKeyDown(WPARAM key)
		{
			if (mUICanvas != nullptr)
			{
				mUICanvas->OnKeyDown(key);
			}
		}

		/**
		 * @brief 键盘释放事件（转发至UI画布）
		 *
		 * @param key 按键代码
		 */
		virtual void OnKeyUp(WPARAM key)
		{
			if (mUICanvas != nullptr)
			{
				mUICanvas->OnKeyUp(key);
			}
		}

		/**
		 * @brief 鼠标移动事件（转发至UI画布）
		 *
		 * @param x 鼠标X坐标
		 * @param y 鼠标Y坐标
		 */
		virtual void OnMouseMove(int x, int y)
		{
			if (mUICanvas != nullptr)
			{
				mUICanvas->OnMouseMove(x, y);
			}
		}

		/**
		 * @brief 鼠标按下事件（转发至UI画布）
		 *
		 * @param mb 鼠标按键类型
		 * @param x 鼠标X坐标
		 * @param y 鼠标Y坐标
		 */
		virtual void OnMouseDown(MouseButton mb, int x, int y)
		{
			if (mb == MouseButton::M_LBUTTON && mUICanvas != nullptr)
			{
				mUICanvas->OnMouseDown(x, y);
			}
		}

		/**
		 * @brief 鼠标释放事件（转发至UI画布）
		 *
		 * @param mb 鼠标按键类型
		 * @param x 鼠标X坐标
		 * @param y 鼠标Y坐标
		 */
		virtual void OnMouseUp(MouseButton mb, int x, int y)
		{
			if (mb == MouseButton::M_LBUTTON && mUICanvas != nullptr)
			{
				mUICanvas->OnMouseUp(x, y);
			}
		}

		/**
		 * @brief 鼠标滚轮事件（转发至UI画布）
		 *
		 * @param delta 滚轮增量
		 */
		virtual void OnMouseWheel(int delta)
		{
			if (mUICanvas != nullptr)
			{
				mUICanvas->OnMouseWheel(delta);
			}
		}

		/** @brief 获取场景是否活跃 */
		bool IsActive() const { return m_isActive; }

		/** @brief 设置场景活跃状态 */
		void SetActive(bool active) { m_isActive = active; }

		/** @brief 获取场景名称 */
		const std::string& GetName() const { return mName; }

		/** @brief 设置场景名称（由 SceneManager 注册时调用） */
		void SetName(const std::string& name) { mName = name; }

		float deltaTime() const;

		/**
		 * @brief 获取UI画布
		 *
		 * @return UICanvas指针，未初始化返回nullptr
		 */
		GameUI::UICanvas* GetUICanvas() const { return mUICanvas.get(); }

		/**
		 * @brief 初始化UI画布
		 *
		 * @param screenWidth 屏幕宽度
		 * @param screenHeight 屏幕高度
		 */
		void InitializeUICanvas(int screenWidth, int screenHeight)
		{
			if (mUICanvas == nullptr)
			{
				mUICanvas = std::make_unique<GameUI::UICanvas>();
				mUICanvas->Initialize(screenWidth, screenHeight);
			}
		}

		/**
		 * @brief 渲染UI画布（Update + Render）
		 * @param dt 本帧 delta time（秒）
		 */
		virtual void RenderUICanvas(float dt)
		{
			if (mUICanvas != nullptr)
			{
				mUICanvas->Update(dt);
				mUICanvas->Render();
			}
		}

	protected:
		// Render a reference-resolution canvas in the same world space as its background.
		void RenderUICanvasInWorld(float dt, float originX, float originY, int width, int height);
		bool m_isActive;
		std::string mName;                        ///< 场景名称
		std::unique_ptr<GameUI::UICanvas> mUICanvas;  ///< UI画布
	};

}
#endif
