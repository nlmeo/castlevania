#pragma once
#ifndef __GAME_PLAYER_FRAME_BOX_COLLIDER_H__
#define __GAME_PLAYER_FRAME_BOX_COLLIDER_H__

#include "../TypeDefine/Geometric.h"
#include "Component.h"
#include <functional>

namespace GamePlay
{
	/**
	 * @brief 碰撞器基类
	 *
	 * 所有碰撞器组件的基类，定义碰撞检测和触发事件的接口
	 */
	class Collider : public Component
	{
	public:
		/**
		 * @brief 构造函数
		 *
		 * @param pobj 所属游戏对象指针
		 */
		Collider(class GameObject* pobj);

		virtual ~Collider();

		/** @brief 判断是否为触发器 */
		bool IsTrigger() { return mIsTrigger; }

		/**
		 * @brief 设置是否为触发器
		 *
		 * 触发器不产生物理碰撞事件，只触发 OnTrigger 回调
		 *
		 * @param isTrigger 是否为触发器
		 */
		void SetIsTrigger(bool isTrigger) { mIsTrigger = isTrigger; }

		/**
		 * @brief 设置碰撞回调函数
		 *
		 * @param callback 碰撞回调
		 */
		void SetCollitionCallBack(std::function<void(Collider*)> callback) { mInCollitionCallBack = callback; }

		/**
		 * @brief 设置触发回调函数
		 *
		 * @param callback 触发回调
		 */
		void SetTriggerCallBack(std::function<void(Collider*)> callback) { mInTriggerCallBack = callback; }

		/**
		 * @brief 触发碰撞事件
		 *
		 * @param c 碰撞的另一个碰撞器
		 */
		void OnCollition(Collider* c)
		{
			if (mInCollitionCallBack)
			{
				mInCollitionCallBack(c);
			}
		}

		/**
		 * @brief 触发触发器事件
		 *
		 * @param c 触发的另一个碰撞器
		 */
		void OnTrigger(Collider* c)
		{
			if (mInTriggerCallBack)
			{
				mInTriggerCallBack(c);
			}
		}

		/**
		 * @brief 碰撞检测
		 *
		 * @param other 另一个碰撞器
		 * @return 是否发生碰撞
		 */
		virtual bool Overlap(const Collider* other) = 0;

		/** @brief 更新碰撞器状态 */
		virtual void UpdateCollider() = 0;

		/** @brief 获取碰撞区域的AABB包围盒 */
		const MathType::RectF& CollitionAABBRect() const { return mCollitonRect; }

	protected:
		MathType::RectF mCollitonRect;                        ///< 碰撞区域的AABB包围盒
		bool mIsTrigger;                                      ///< 是否为触发器
		std::function<void(Collider*)> mInCollitionCallBack;  ///< 碰撞事件回调
		std::function<void(Collider*)> mInTriggerCallBack;    ///< 触发事件回调
	};

	/**
	 * @brief 矩形碰撞器
	 *
	 * 矩形形状的碰撞器，支持自定义宽度和高度
	 */
	class BoxCollider : public Collider
	{
	public:
		/**
		 * @brief 构造函数
		 *
		 * @param pobj 所属游戏对象指针
		 */
		BoxCollider(class GameObject* pobj);

		/**
		 * @brief 设置碰撞器尺寸
		 *
		 * @param width 宽度
		 * @param height 高度
		 */
		void SetColliderSize(float width, float height);

		/** @brief 更新碰撞器 */
		void UpdateCollider();

		/**
		 * @brief 碰撞检测（重写）
		 *
		 * @param other 另一个碰撞器
		 * @return 是否发生碰撞
		 */
		bool Overlap(const Collider* other) override;

		float  Width() const { return mWidth; };
		float  Height() const { return mHeight; };
	protected:
		float mWidth;   ///< 宽度
		float mHeight;  ///< 高度
	};

	/**
	 * @brief 圆形碰撞器
	 *
	 * 圆形形状的碰撞器，支持自定义半径
	 */
	class SphereCollider : public Collider
	{
	public:
		/**
		 * @brief 构造函数
		 *
		 * @param pobj 所属游戏对象指针
		 */
		SphereCollider(class GameObject* pobj);

		/**
		 * @brief 设置碰撞器半径
		 *
		 * @param radius 半径
		 */
		void SetColliderRadius(float radius);

		/** @brief 更新碰撞器 */
		void UpdateCollider();

		/**
		 * @brief 碰撞检测（重写）
		 *
		 * @param other 另一个碰撞器
		 * @return 是否发生碰撞
		 */
		bool Overlap(const Collider* other) override;

		/** @brief 获取半径 */
		const float& Radius() const { return mRadius; }

	protected:
		float mRadius;  ///< 半径
	};

	/**
	 * @brief 旋转矩形碰撞器
	 *
	 * 支持任意旋转角度的矩形碰撞器，采用分离轴定理(SAT)进行精确碰撞检测。
	 * 采用组合形式：直接继承Collider而非BoxCollider，内部自行维护宽高数据，
	 * 旋转角度取自已所属GameObject的Transform组件。
	 */
	class RotationBoxCollider : public Collider
	{
	public:
		/**
		 * @brief 构造函数
		 *
		 * @param pobj 所属游戏对象指针
		 */
		RotationBoxCollider(class GameObject* pobj);

		/**
		 * @brief 设置碰撞器尺寸
		 *
		 * @param width 宽度
		 * @param height 高度
		 */
		void SetColliderSize(float width, float height);

		/** @brief 更新碰撞器（根据旋转角度计算AABB包围盒） */
		void UpdateCollider() override;

		/**
		 * @brief 碰撞检测（重写，使用投影/SAT检测）
		 *
		 * @param other 另一个碰撞器
		 * @return 是否发生碰撞
		 */
		bool Overlap(const Collider* other) override;

		/** @brief 获取宽度 */
		float Width() const { return mWidth; }

		/** @brief 获取高度 */
		float Height() const { return mHeight; }

		/** @brief 获取当前旋转角度（弧度） */
		float Rotation() const;

		/**
		 * @brief 获取世界坐标系下的四个顶点
		 *
		 * @param corners 输出的4个顶点数组
		 */
		void GetWorldCorners(MathType::vec2f corners[4]) const;

	protected:
		float mWidth;   ///< 宽度
		float mHeight;  ///< 高度
	};


}
#endif