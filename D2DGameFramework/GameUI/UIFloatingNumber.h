#pragma once
#ifndef __GAME_UI_FLOATING_NUMBER_H__
#define __GAME_UI_FLOATING_NUMBER_H__

#include "UINumber.h"
#include <functional>

namespace GameUI
{
	/**
	 * @brief 浮动数字（受击伤害数字等）
	 *
	 * 继承 UINumber 的数字渲染能力，增加"上浮 + 渐隐"动画。
	 * 适用于受击伤害、治疗、经验值等浮动提示。
	 *
	 * 用法：
	 *   1. Create/CreateWithTextures 初始化纹理（同 UINumber）
	 *   2. Spawn(value, x, y) 启动动画
	 *   3. 每帧检查 IsFinished()，完成后从 Canvas 移除
	 */
	typedef class UIFloatingNumber : public UINumber
	{
	public:
		/** @brief 构造函数 */
		UIFloatingNumber();

		/** @brief 析构函数 */
		virtual ~UIFloatingNumber();

		/**
		 * @brief 启动浮动动画
		 *
		 * @param number        显示的数字
		 * @param screenX       起始屏幕 X 坐标
		 * @param screenY       起始屏幕 Y 坐标
		 * @param floatHeight   上浮总高度（像素，默认 60）
		 * @param duration      动画总时长（秒，默认 1.0）
		 */
		void Spawn(int number, float startX, float startY,
			float floatHeight = 60.0f, float duration = 1.0f, float isWordPos = false);

		/** @brief 是否已完成动画 */
		bool IsFinished() const { return mIsFinished; }

		/**
		 * @brief 设置动画完成回调
		 * @param callback 回调签名 void(UIFloatingNumber* self)
		 */
		void SetOnFinished(std::function<void(UIFloatingNumber*)> callback);

		/**
		 * @brief 设置浮动速度曲线（可选）
		 *
		 * 默认：先快后慢（缓出）
		 * @param useEaseOut  true=缓出（默认）, false=线性
		 */
		void SetUseEaseOut(bool useEaseOut) { mUseEaseOut = useEaseOut; }

		/** @brief 更新动画 */
		virtual void Update(float deltaTime) override;
		void Render() override;

	protected:
		/** @brief 缓出函数：t 在 [0,1] 内输出缓动后的进度 */
		static float _EaseOutCubic(float t);

	protected:
		MathType::vec2f mStartPos = {0,0};
		//float mStartX = 0.0f;            ///< 起始 X
		//float mStartY = 0.0f;            ///< 起始 Y
		float mFloatHeight = 60.0f;      ///< 上浮高度
		float mDuration = 1.0f;          ///< 动画时长
		float mElapsed = 0.0f;           ///< 已耗时
		bool  mIsFinished = false;       ///< 是否完成
		bool  mUseEaseOut = true;        ///< 是否使用缓出曲线
		bool mIsUseWordPos = false;      ///< 是否使用世界变换


		std::function<void(UIFloatingNumber*)> mOnFinished; ///< 完成回调
	} UIFloatingNumber, * LPUIFloatingNumber;
}

#endif
