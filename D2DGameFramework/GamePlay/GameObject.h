#pragma once
#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#include "../HelpTool/SingletonTemplate.h"
#include "../HelpTool/RandomMaker.h"
#include "GameLayer.h"
#include <string>
#include <vector>
#include <queue>
#include "CollisionMatrix.h"
#include "Camera2d.h"
#include "Transform2d.h"
#include "SpriteRender.h"
#include "VideoPlayer.h"
#include "Collider2d.h"
#include "AudioSource.h"
#include "Animator.h"
#include "FiniteStateMachine.h"
#include "AfterImageTrail.h"
#include "HitFlash.h"
#include "../AttackCollider.h"
namespace GamePlay
{
	
	/**
	 * @brief 游戏对象类
	 *
	 * 游戏场景中的基本实体，包含组件系统和生命周期管理
	 */
	class GameObject
	{
		friend class GameObjectManager;
	public:
		/** @brief 构造函数 */
		GameObject()
		{
			mpTransform = AddComponent<Transform>();
			mLayer = GameLayer::Layer_0;
			mName = std::string("GameObject_") + HelpTool::RandomNum(64);
			mTag = "Default";
			mIsAvtive = true;
			mIsDestroy = false;
		}

		/** @brief 析构函数 */
		virtual ~GameObject()
		{
			if (!mComponents.empty())
			{
				for (auto& c : mComponents)
				{
					delete c;
				}
				mComponents.clear();
			}
			if (mpTransform != nullptr)
			{
				delete mpTransform;
				mpTransform = nullptr;
			}
		}

		virtual void OnDebugRender() {};

		/**
		 * @brief 添加组件（模板版本）
		 *
		 * @tparam T 组件类型
		 * @return 添加的组件指针，如果已存在则返回现有组件
		 */
		template<typename T>
		T* AddComponent()
		{
			emComponentType t = ComponentClassType::Instance().GetComponentClassType<T>();
			if (t == emComponentType::ComType_Transform)
			{
				if (mpTransform != nullptr)
				{
					return dynamic_cast<T*>(mpTransform);
				}
				mpTransform = new Transform(this);
				return dynamic_cast<T*>(mpTransform);
			}
			else
			{
				T* ptr = GetComponent<T>();
				if (ptr != nullptr)
				{
					return ptr;
				}
				ptr = new T(this);
				mComponents.push_back(ptr);
				return ptr;
			}
			return nullptr;
		}

		/**
		 * @brief 获取组件（模板版本）
		 *
		 * @tparam T 组件类型
		 * @return 组件指针，不存在返回nullptr
		 */
		template<typename T>
		T* GetComponent()
		{
			emComponentType t = ComponentClassType::Instance().GetComponentClassType<T>();

			if (t == COMTYPE::ComType_Transform)
			{
				return dynamic_cast<T*>(mpTransform);
			}

			for (auto& itr : mComponents)
			{
				if (itr->Type() == t)
				{
					return dynamic_cast<T*>(itr);
				}
			}
			return nullptr;
		}

		/**
		 * @brief 获取碰撞器组件
		 *
		 * @return 碰撞器组件指针，不存在返回nullptr
		 */
		Collider* GetCollider()
		{
			Collider* pcld = nullptr;
			for (auto& itr : mComponents)
			{
				pcld = dynamic_cast<Collider*>(itr);
				if (pcld != nullptr)
				{
					break;
				}
			}
			return pcld;
		}

		/**
		 * @brief 移除组件（模板版本）
		 *
		 * @tparam T 组件类型（Transform不可移除）
		 */
		template<typename T>
		void RemoveComponent()
		{
			emComponentType t = ComponentClassType::Instance().GetComponentClassType<T>();
			if (t == COMTYPE::ComType_Transform) { return; }
			for (auto it = mComponents.begin(); it != mComponents.end(); it++)
			{
				if ((*it)->Type() == t)
				{
					delete* it;
					mComponents.erase(it);
					return;
				}
			}
		}

		/** @brief 对象创建时调用（仅一次） */
		virtual void Awake() {}

		/** @brief 对象启用时调用 */
		virtual void OnEnable() {}

		/** @brief 对象首次更新前调用 */
		virtual void Start() {}

		/**
		 * @brief 每帧更新
		 *
		 * @param deltaTime 帧间隔时间（秒）
		 */
		virtual void Update(float deltaTime) {};

		/** @brief 碰撞检测后调用（用于碰撞驱动的状态切换） */
		virtual void LaterUpdate(float deltaTime) {};

		/** @brief 对象禁用时调用 */
		virtual void OnDisable() {}

		/** @brief 对象销毁前调用 */
		virtual void OnDestroy() {}

		/** @brief 获取Transform组件 */
		Transform& transform() { return *mpTransform; }

		/** @brief 获取对象名称 */
		std::string& name() { return mName; }

		/** @brief 获取对象标签 */
		std::string& tag() { return mTag; }

		/** @brief 获取对象层级 */
		GameLayer& layer() { return mLayer; }

		/** @brief 获取对象类型名称 */
		std::string& typeName() { return mTypeName; }

		/** @brief 判断对象是否激活 */
		bool Active() const { return mIsAvtive; }

		/**
		 * @brief 设置对象激活状态
		 *
		 * @param active 是否激活
		 */
		void SetActive(bool active)
		{
			if (mIsAvtive != active)
			{
				mIsAvtive = active;
				if (mIsAvtive)
				{
					OnEnable();
				}
				else
				{
					OnDisable();
				}
			}
		}

		/** @brief 销毁对象 */
		bool IsDestroyed() const { return mIsDestroy; }
		void SetDestroyOnSceneExit(bool enabled = true) { mDestroyOnSceneExit = enabled; }

		void Destroy() {
			if (mIsDestroy) return;
			mIsDestroy = true;
			OnDestroy();
		}

		/**
		 * @brief 创建游戏对象（模板版本）
		 *
		 * @tparam T 对象类型
		 * @param _name 对象名称（可选）
		 * @return 创建的对象指针
		 */
		template<typename T>
		static T* CreateGameObject(std::string _name = "");

		/**
		 * @brief 根据名称查找单个游戏对象
		 *
		 * @param _name 对象名称
		 * @param isActive 是否只查找激活对象（默认true）
		 * @param absolute 是否精确匹配（默认true）
		 * @return 找到的对象指针，未找到返回nullptr
		 */
		static GameObject* FindGameObjectByName(std::string _name, bool isActive = true, bool absolute = true);

		/**
		 * @brief 根据名称查找多个游戏对象
		 *
		 * @param _name 对象名称
		 * @param isActive 是否只查找激活对象（默认true）
		 * @param absolute 是否精确匹配（默认true）
		 * @return 找到的对象列表
		 */
		static std::vector<GameObject*> FindGameObjectsByName(std::string _name, bool isActive = true, bool absolute = true);

		/**
		 * @brief 根据标签查找单个游戏对象
		 *
		 * @param _tag 对象标签
		 * @param isActive 是否只查找激活对象（默认true）
		 * @param absolute 是否精确匹配（默认true）
		 * @return 找到的对象指针，未找到返回nullptr
		 */
		static GameObject* FindGameObjectByTag(std::string _tag, bool isActive = true, bool absolute = true);

		/**
		 * @brief 根据标签查找多个游戏对象
		 *
		 * @param _tag 对象标签
		 * @param isActive 是否只查找激活对象（默认true）
		 * @param absolute 是否精确匹配（默认true）
		 * @return 找到的对象列表
		 */
		static std::vector<GameObject*> FindGameObjectsByTag(std::string _tag, bool isActive = true, bool absolute = true);

		/**
		 * @brief 根据组件类型查找单个游戏对象
		 *
		 * @tparam T 组件类型
		 * @param isActive 是否只查找激活对象（默认true）
		 * @return 找到的对象指针，未找到返回nullptr
		 */
		template<typename T>
		static GameObject* FindGameObjectByComponent(bool isActive = true);

		/**
		 * @brief 根据组件类型查找多个游戏对象
		 *
		 * @tparam T 组件类型
		 * @param isActive 是否只查找激活对象（默认true）
		 * @return 找到的对象列表
		 */
		template<typename T>
		static std::vector<GameObject*> FindGameObjectsByComponent(bool isActive = true);

	public:
		/**
		 * @brief 更新组件（内部使用）
		 *
		 * 在Transform变化时更新相关组件
		 */
		void _ComponentUpdate(float deltaTime)
		{
			bool& isOrderChange = mpTransform->OrderChange();
			bool& isNeedUpdate = mpTransform->NeedUpdate();

			if (isOrderChange || isNeedUpdate)
			{
				isOrderChange = false;
				
				SpriteRender* psr = GetComponent<SpriteRender>();
				if (psr != nullptr)
				{
					psr->CaclRenderMatrix();
				}

				Animator* panim = GetComponent<Animator>();
				if (panim != nullptr)
				{
					panim->CaclRenderMatrix();
				}

				VideoPlayer* pvp = GetComponent<VideoPlayer>();
				if (pvp != nullptr)
				{
					pvp->CaclRenderMatrix();
				}
			}

			if (isNeedUpdate)
			{
				Camera* pcam = GetComponent<Camera>();
				if (pcam != nullptr)
				{
					pcam->UpdateCamera();
				}

				Collider* pcollider = GetCollider();
				if (pcollider != nullptr)
				{
					pcollider->UpdateCollider();
				}

				FiniteStateMachine* pfsm = GetComponent<FiniteStateMachine>();
				if (pfsm != nullptr)
				{
					pfsm->Update(deltaTime); 
				}
			}
		}

	protected:
		bool mIsDestroy;           ///< 是否已销毁
		bool mIsAvtive;            ///< 是否激活
		GameLayer mLayer;          ///< 所属层级
		std::string mName;         ///< 对象名称
		std::string mTag;          ///< 对象标签
		std::string mTypeName;     ///< 类型名称
		bool mDestroyOnSceneExit = false;
		Transform* mpTransform = nullptr;    ///< Transform组件（必选）
		std::vector<LPCOMPONENT> mComponents;  ///< 其他组件列表
	};


	/**
	 * @brief 游戏对象管理器
	 *
	 * 单例类，负责管理所有游戏对象的创建、更新、渲染和销毁
	 */
	class GameObjectManager : public Singleton<GameObjectManager>
	{
		friend class GamePlay::GameObject;
		PRIVATE_CONSTRUCTOR_AND_COPY(GameObjectManager)
	public:
		/** @brief 析构函数 */
		~GameObjectManager();


		void _OnDebugRender()
		{
			for (auto obj : mObjects)
			{
				if (obj->Active() && !obj->IsDestroyed())
				{
					obj->OnDebugRender();
				}
			}
		}

		void _AddColliderObject(GamePlay::Collider* pc)
		{
			mColliderObjects.push_back(pc);
			mCollitionObjectCount++;
		}

		void _RemoveColliderObject(GamePlay::Collider* pc)
		{
			auto it = mColliderObjects.begin();
			GamePlay::Collider* p = nullptr;
			while (it != mColliderObjects.end())
			{
				p = *it;
				if (p == pc)
				{
					mColliderObjects.erase(it);
					mCollitionObjectCount--;
					break;
				}
				++it;
			}
		}

		/**
		 * @brief 添加游戏对象（内部使用）
		 *
		 * @param pObj 要添加的游戏对象指针
		 */
		void _AddObject(class GameObject* pObj)
		{
			mNewObjects.push_back(pObj);
		}

		void _MergeNewObjects()
		{
			if (!mNewObjects.empty())
			{
				// swap 到局部变量再迭代，防止 Awake/Start/OnEnable
				// 中调用 CreateGameObject 导致 mNewObjects 重分配而迭代器失效
				std::vector<GameObject*> newObjs;
				newObjs.swap(mNewObjects);
				for (auto& pObj : newObjs) { if (!pObj->IsDestroyed()) pObj->Awake(); }
				for (auto& pObj : newObjs) { if (!pObj->IsDestroyed() && pObj->Active()) pObj->OnEnable(); }
				for (auto& pObj : newObjs) { if (!pObj->IsDestroyed()) pObj->Start(); }
				mObjects.insert(mObjects.end(), newObjs.begin(), newObjs.end());
			}
		}
		bool HasPendingObjects() const { return !mNewObjects.empty(); }

		/** @brief 更新新添加的对象（调用Awake和Start） */
		void _UpdateNewObjects();

		/**
		 * @brief 更新所有对象
		 *
		 * @param deltaTime 帧间隔时间（秒）
		 */
		void _UpdateAllObjects(float deltaTime);


		void _LaterUpdateAllObjects(float deltaTime);

		/** @brief 处理碰撞和触发事件 */
		void _CollitionAndTriggerEvent();

		/** @brief 渲染所有可见对象 */
		void _RenderObjects();
		/** @brief 每帧重新建立渲染队列，暂停时也保留世界画面。 */
		void _PrepareRenderObjects();

		/** @brief 清理所有对象 */
		void ClearAllObjects(std::vector<GameObject*> ignores, bool clearPending = true);
		void DestroySceneTransientObjects();

		/** @brief 获取游戏对象总数 */
		unsigned int GameObjectCount() const { return (unsigned int)mObjects.size() + (unsigned int)mNewObjects.size(); }

		/** @brief 获取激活对象数量 */
		unsigned int ActiveObjectCount() const { return mActiveObjectCount; }

		/** @brief 获取碰撞对象数量 */
		unsigned int CollitionObjectCount() const { return mCollitionObjectCount; }

		/** @brief 获取渲染对象数量 */
		unsigned int RenderObjectCount() const { return mRenderObjectCount; }

		/** @brief 获取可见对象数量 */
		unsigned int VisibleObjectCount() const { return mVisibleObjectCount; }

		std::vector<Collider*>& ColliderObjects() { return mColliderObjects; }
	private:
		/**
		 * @brief 渲染组件比较器
		 *
		 * 用于按层级顺序对渲染组件进行排序
		 */
		class CompareGameObjectLayer
		{
		public:
			bool operator()(class RenderComponent* a, class RenderComponent* b);
		};

	private:
		unsigned int mActiveObjectCount = 0;     ///< 激活对象计数
		unsigned int mCollitionObjectCount = 0;  ///< 碰撞对象计数
		unsigned int mRenderObjectCount = 0;     ///< 渲染对象计数
		unsigned int mVisibleObjectCount = 0;    ///< 可见对象计数

		std::vector<class GameObject*> mNewObjects;     ///< 新添加的对象列表
		std::vector<class GameObject*> mObjects;        ///< 所有对象列表
		std::vector<class Collider*> mColliderObjects;  ///< 碰撞器对象列表

		std::priority_queue<class RenderComponent*, std::vector<class RenderComponent*>, CompareGameObjectLayer> mRenderList;  ///< 渲染队列（按层级排序）
	};

	

	/** @brief 游戏对象管理器宏定义 */
	#define GameObjectMgr GamePlay::GameObjectManager::Instance()

	template<typename T>
	inline T* GameObject::CreateGameObject(std::string _name)
	{
		auto pobj = new T();
		if (_name != "")
		{
			pobj->mName = _name;
		}
		pobj->mTypeName = typeid(T).name();

		GameObjectMgr._AddObject(pobj);
		return pobj;
	}

	template<typename T>
	inline GameObject* GameObject::FindGameObjectByComponent(bool isActive)
	{
		if (isActive)
		{
			for (auto& p : GameObjectMgr.mObjects)
			{
				if (p->Active())
				{
					if (p->GetComponent<T>())
					{
						return p;
					}
				}
			}
		}
		else
		{
			for (auto& p : GameObjectMgr.mObjects)
			{
				if (p->GetComponent<T>())
				{
					return p;
				}
			}
		}

		return nullptr;
	}

	//查找拥有某个组件的所有对象
	template<typename T>
	inline std::vector<GameObject*> GameObject::FindGameObjectsByComponent(bool isActive)
	{
		std::vector<GameObject*> ret;
		if (isActive)
		{
			for (auto& p : GameObjectMgr.mObjects)
			{
				if (p->Active())
				{
					if (p->GetComponent<T>())
					{
						ret.push_back(p);
					}
				}
			}
		}
		else
		{
			for (auto& p : GameObjectMgr.mObjects)
			{
				if (p->GetComponent<T>())
				{
					ret.push_back(p);
				}
			}
		}
		return ret;
	}
}
#define SAFE_DESTROY(p)		if(p!=nullptr){p->Destroy();p=nullptr;}
#endif
