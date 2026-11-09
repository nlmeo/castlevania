#pragma once
#ifndef __GAME_PLAY_FRAME_TRANSFORM_2D_H__
#define __GAME_PLAY_FRAME_TRANSFORM_2D_H__

#include "../TypeDefine/Matrix3x3.h"
#include "Component.h"

namespace GamePlay
{
	/**
	 * @brief Transform组件类
	 *
	 * 负责管理游戏对象的位置、旋转和缩放，提供变换矩阵的计算
	 */
	class Transform : public Component
	{
	public:
		/**
		 * @brief 矩阵组合顺序枚举
		 *
		 * 定义位置、旋转、缩放三种变换的组合顺序
		 */
		typedef enum class emMatrixOrder
		{
			SRT,  ///< Scale -> Rotate -> Translate（默认）
			RST,  ///< Rotate -> Scale -> Translate
			RTS,  ///< Rotate -> Translate -> Scale
			TRS,  ///< Translate -> Rotate -> Scale
			TSR,  ///< Translate -> Scale -> Rotate
			STR   ///< Scale -> Translate -> Rotate
		}MatrixOrder;

	public:
		/**
		 * @brief 构造函数
		 *
		 * @param pobj 所属游戏对象指针
		 */
		Transform(GameObject* pobj) : Component(pobj)
		{
			mType = COMTYPE::ComType_Transform;

			mPosition = MathType::vec2f::zero();
			mScale = MathType::vec2f::one();
			mRotation = 0;

			_update_Matrix_Translate();
			_update_Matrix_Scale();
			_update_Matrix_Rotate();

			mMatrixOrder = MatrixOrder::SRT;
			mOrderChange = true;
			mNeedUpdate = true;
		}

		/** @brief 获取右方向向量 */
		MathType::vec2f right() const { return mRight; }

		/** @brief 获取左方向向量 */
		MathType::vec2f left() const { return -mRight; }

		/** @brief 获取上方向向量 */
		MathType::vec2f up() const { return mUp; }

		/** @brief 获取下方向向量 */
		MathType::vec2f down() const { return -mUp; }

		/** @brief 获取本地缩放 */
		const MathType::vec2f& localScale() const { return mScale; }

		/** @brief 获取位置 */
		const MathType::vec2f& position() const { return mPosition; }

		/** @brief 获取旋转角度（弧度） */
		const float& rotation() const { return mRotation; }

		/** @brief 获取平移矩阵 */
		const MathType::Mat3x3f& MatT() const { return matrixTranslate; }

		/** @brief 获取缩放矩阵 */
		const MathType::Mat3x3f& MatS() const { return matrixScale; }

		/** @brief 获取旋转矩阵 */
		const MathType::Mat3x3f& MatR() const { return matrixRotate; }

		/**
		 * @brief 设置位置
		 *
		 * @param v 新的位置向量
		 */
		void SetPosition(const MathType::vec2f& v)
		{
			mPosition = v;
			_update_Matrix_Translate();
			mNeedUpdate = true;
		}

		/**
		 * @brief 设置缩放
		 *
		 * @param v 新的缩放向量
		 */
		void SetScale(const MathType::vec2f& v)
		{
			mScale = v;
			_update_Matrix_Scale();
			mNeedUpdate = true;
		}

		/**
		 * @brief 设置旋转角度
		 *
		 * @param v 新的旋转角度（弧度）
		 */
		void SetRotation(const float& v)
		{
			mRotation = v;
			_update_Matrix_Rotate();
			mNeedUpdate = true;
		}

		/**
		 * @brief 平移变换
		 *
		 * @param tv 平移向量
		 */
		void translate(const MathType::vec2f& tv)
		{
			mPosition += tv;
			_update_Matrix_Translate();
			mNeedUpdate = true;
		}

		/**
		 * @brief 缩放变换
		 *
		 * @param sv 缩放向量
		 */
		void scale(const MathType::vec2f& sv)
		{
			mScale *= sv;
			_update_Matrix_Scale();
			mNeedUpdate = true;
		}

		/**
		 * @brief 旋转变换
		 *
		 * @param angle 旋转角度增量（弧度）
		 */
		void rotate(const float& angle);

		/**
		 * @brief 获取是否需要更新标记
		 *
		 * @return 是否需要更新的引用
		 */
		bool& NeedUpdate()
		{
			return mNeedUpdate;
		}

		/**
		 * @brief 获取矩阵顺序是否改变的标记
		 *
		 * @return 矩阵顺序是否改变的引用
		 */
		bool& OrderChange()
		{
			return mOrderChange;
		}

		/** @brief 获取当前矩阵组合顺序 */
		const MatrixOrder& MatOrder() const { return mMatrixOrder; }

		/**
		 * @brief 设置矩阵组合顺序
		 *
		 * @param order 新的矩阵组合顺序
		 */
		void SetMatOrder(MatrixOrder order) { mMatrixOrder = order; mOrderChange = true; }

	protected:
		/** @brief 更新平移矩阵 */
		void _update_Matrix_Translate();

		/** @brief 更新缩放矩阵 */
		void _update_Matrix_Scale();

		/** @brief 更新旋转矩阵 */
		void _update_Matrix_Rotate();

	protected:
		MathType::vec2f mPosition;      ///< 位置
		MathType::vec2f mScale;         ///< 缩放值
		float mRotation;                ///< 旋转角度（弧度）

		MathType::vec2f mUp;            ///< 上方向向量（Y轴）
		MathType::vec2f mRight;         ///< 右方向向量（X轴）

		MathType::Mat3x3f matrixTranslate;  ///< 平移矩阵
		MathType::Mat3x3f matrixScale;      ///< 缩放矩阵
		MathType::Mat3x3f matrixRotate;     ///< 旋转矩阵

		MatrixOrder mMatrixOrder;       ///< 矩阵组合顺序

		bool mNeedUpdate;               ///< 是否需要更新
		bool mOrderChange;              ///< 矩阵顺序是否改变
	};
}
#endif
