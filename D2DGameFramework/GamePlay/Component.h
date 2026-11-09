#pragma once
#ifndef __GAME_PLAY_FRAME_COMPONENT_H__
#define __GAME_PLAY_FRAME_COMPONENT_H__

#include <map>
#include <string>
#include "../HelpTool/SingletonTemplate.h"
#include "../TypeDefine/Geometric.h"
#include "../TypeDefine/Matrix3x3.h"

namespace GamePlay
{
	/**
	 * @brief 组件类型枚举
	 *
	 * 定义所有组件类型的枚举值，用于组件的识别和管理
	 */
	typedef enum class emComponentType
	{
		ComType_None = 0,           ///< 无类型
		ComType_Transform,          ///< 变换组件
		ComType_SpriteRender,       ///< 精灵渲染组件
		ComType_Camera,             ///< 摄像机组件
		ComType_Collider,           ///< 碰撞器组件（基类）
		ComType_BoxCollider,        ///< 矩形碰撞器组件
		ComType_SphereCollider,     ///< 圆形碰撞器组件
		ComType_RotationBoxCollider,///< 旋转矩形碰撞器组件
		ComType_AudioSource,        ///< 音频源组件
		ComType_RenderComponent,    ///< 渲染组件（基类）
		ComType_VideoPlayer,        ///< 视频播放器组件
		ComType_Animator,           ///< 动画组件（目前只支持动画）
		ComType_FSM,                ///< 有限状态机组件
		ComType_AfterImageTrail,	///< 拖影
		ComType_HitFlash,			///< 受击泛红闪烁
		ComType_AttackCollider,		///< 攻击碰撞器组件
		ComType_Limit,              ///< 组件类型数量限制（非有效类型）
	}COMTYPE, * LPCOMTYPE;

	/**
	 * @brief 组件类型管理类
	 *
	 * 单例类，负责组件类型的注册和查询，实现类型名称到枚举值的映射
	 */
	typedef class ComponentClassType :public Singleton<ComponentClassType>
	{
		PRIVATE_CONSTRUCTOR_AND_COPY(ComponentClassType)
	public:
		/**
		 * @brief 获取组件类型（模板版本）
		 *
		 * @tparam T 组件类型
		 * @return 对应的组件类型枚举值
		 */
		template<typename T>
		emComponentType GetComponentClassType()
		{
			return GetComponentClassType(typeid(T).name());
		}
	protected:
		/**
		 * @brief 初始化所有组件类型注册
		 */
		static void InitAllComponent();

		/**
		 * @brief 根据类型名称获取组件类型
		 *
		 * @param type_name 类型名称（由typeid获取）
		 * @return 对应的组件类型枚举值
		 */
		emComponentType GetComponentClassType(std::string type_name);

		/**
		 * @brief 注册组件类型
		 *
		 * @param type_name 类型名称
		 * @param t 对应的组件类型枚举值
		 */
		static void RegComponentClassType(std::string type_name, emComponentType t);
	private:
		static bool mIsInitComponent;                              ///< 是否已初始化组件类型注册
		static std::map<std::string, emComponentType> mComponentClassType;  ///< 类型名称到枚举值的映射表
	}COMPONENT_CLASS_TYPE;

	/**
	 * @brief 组件基类
	 *
	 * 所有组件的基类，提供组件的基本接口和属性
	 */
	typedef class Component
	{
	public:
		/** @brief 析构函数 */
		virtual ~Component() = default;
		
		/** @brief 拷贝构造函数（禁用） */
		Component(const Component& that) = delete;
		
		/** @brief 赋值运算符（禁用） */
		Component& operator = (const Component& that) = delete;

		/**
		 * @brief 构造函数
		 *
		 * @param pObject 所属的游戏对象指针
		 */
		Component(class GameObject* pObject);

		/** @brief 获取组件类型 */
		COMTYPE Type() const{ return mType; }

		/** @brief 获取所属游戏对象 */
		class GameObject* gameobject() const { return mOwner; }

		/** @brief 获取所属游戏对象的Transform组件 */
		class Transform* transform() const;

	protected:
		class GameObject* mOwner;  ///< 所属游戏对象
		COMTYPE mType;             ///< 组件类型
	}COMPONENT, * LPCOMPONENT;

	/**
	 * @brief 渲染组件基类
	 *
	 * 所有需要渲染的组件的基类，定义渲染相关的接口
	 */
	class RenderComponent : public Component
	{
	public:
		/**
		 * @brief 构造函数
		 *
		 * @param pObject 所属的游戏对象指针
		 */
		RenderComponent(class GameObject* pObject);

		/**
		 * @brief 获取渲染层级顺序
		 *
		 * @return 渲染层级值，越小越先渲染
		 */
		virtual const int& OrderInLayer() const = 0;

		/**
		 * @brief 渲染组件
		 *
		 * @param vpv 视图-投影-视口变换矩阵
		 */
		virtual void Render(const MathType::Mat3x3f& vpv) = 0;

		/**
		 * @brief 获取渲染矩形区域
		 *
		 * @return 渲染区域的AABB包围盒
		 */
		virtual const MathType::RectF& GetRenderRect() const = 0;

		virtual void CaclRenderMatrix() = 0;
	};
}
#endif