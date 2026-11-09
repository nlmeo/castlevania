#pragma once
#ifndef __CHILD_OBJECT_H__
#define __CHILD_OBJECT_H__

#include "GameObject.h"

namespace GamePlay
{
	/**
	 * @brief 子对象类，继承自 GameObject
	 *
	 * 持有 mOwner 父对象指针，碰撞回调中可以通过 GetOwner()
	 * 获取到真正的父对象（例如敌人、子弹、攻击特效等），
	 * 替代之前使用独立 GameObject 时 dynamic_cast 为空的问题。
	 */
	class ChildObject : public GameObject
	{
	public:
		ChildObject() = default;

		void SetOwner(GameObject* owner) { mOwner = owner; }

		GameObject* GetOwner() const { return mOwner; }

		template<typename T>
		T* GetOwner() const { return dynamic_cast<T*>(mOwner); }

	private:
		GameObject* mOwner = nullptr; 
	};
}

#endif // !__CHILD_OBJECT_H__
