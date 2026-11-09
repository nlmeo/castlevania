#pragma once
#ifndef __GAME_UI_NUMBER_H__
#define __GAME_UI_NUMBER_H__

#include "UIElement.h"
#include "../D2DCore/Texture.h"

namespace GameUI
{
	/**
	 * @brief 数字图片显示器
	 *
	 * 支持两种加载方式：
	 *   1. 单张纹理条（横向等宽 10 等份，0~9 从左到右排列）
	 *      纹理布局：| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 |
	 *   2. 10 张独立纹理（每张对应一个数字 0~9）
	 *
	 * 常用于血量数字、分数、计时器等 UI 显示。
	 */
	typedef class UINumber : public UIElement
	{
	public:
		/** @brief 构造函数 */
		UINumber();

		/** @brief 析构函数 */
		virtual ~UINumber();

		/**
		 * @brief 用单张纹理条初始化（横向 10 等份：0~9）
		 *
		 * @param x          左上角 X 坐标
		 * @param y          左上角 Y 坐标
		 * @param digitWidth 单个数字的绘制宽度
		 * @param digitHeight 单个数字的绘制高度
		 * @param pTexture  纹理条（水平 10 等份：0 1 2 ... 9）
		 * @param spacing   数字之间的像素间隔（默认 0）
		 * @return true 成功
		 */
		bool Create(float x, float y, float digitWidth, float digitHeight,
			D2DCore::Texture* pTexture, float spacing = 0.0f);

		/**
		 * @brief 用 10 张独立纹理初始化
		 *
		 * @param x          左上角 X 坐标
		 * @param y          左上角 Y 坐标
		 * @param digitWidth 单个数字的绘制宽度
		 * @param digitHeight 单个数字的绘制高度
		 * @param textures   10 张纹理数组，textures[0]=数字0, textures[1]=数字1, ...
		 * @param spacing   数字之间的像素间隔（默认 0）
		 * @return true 成功
		 */
		bool CreateWithTextures(float x, float y, float digitWidth, float digitHeight,
			D2DCore::Texture* textures[10], float spacing = 0.0f);

		/**
		 * @brief 设置要显示的数字
		 * @param number 整数（负数取绝对值）
		 */
		void SetNumber(int number);

		/** @brief 获取当前显示的数字 */
		int GetNumber() const { return mNumber; }

		/**
		 * @brief 设置数字间隔
		 * @param spacing 像素值
		 */
		void SetSpacing(float spacing) { mSpacing = spacing; }

		/**
		 * @brief 设置固定位数（不足补前导 0）
		 *
		 * 例：digitCount=3, number=5 → 显示"005"
		 * digitCount=0（默认）→ 自动按实际位数显示，不补前导零
		 *
		 * @param count 位数（0 = 自动）
		 */
		void SetDigitCount(int count) { mDigitCount = (count < 0) ? 0 : count; }

		/** @brief 获取当前设置的固定位数 */
		int GetDigitCount() const { return mDigitCount; }

		/**
		 * @brief 设置透明度
		 * @param opacity 0.0（完全透明）~ 1.0（不透明）
		 */
		void SetOpacity(float opacity) { mOpacity = MathType::clamp(opacity, 0.0f, 1.0f); }

		/** @brief 获取当前透明度 */
		float GetOpacity() const { return mOpacity; }

		/** @brief 渲染数字 */
		virtual void Render() override;

		/** @brief 空实现 */
		virtual void Update(float deltaTime) override;

	protected:
		/** @brief 取指定索引位的数字值（从高位到低位） */
		int _GetDigitAt(int index, int displayCount) const;

		/** @brief 绘制单个数字 */
		void _DrawDigit(int digit, float left, float top, float right, float bottom);

	protected:
		bool mUseIndividual = false;                   ///< true=独立纹理模式

		D2DCore::Texture* mPTexture = nullptr;         ///< 纹理条模式：单张纹理条
		D2DCore::Texture* mDigitTextures[10] = {};      ///< 独立纹理模式：每张对应 0~9

		int    mNumber = 0;                             ///< 当前数字
		float  mDigitWidth = 0.0f;                     ///< 单字宽度
		float  mDigitHeight = 0.0f;                    ///< 单字高度
		float  mSpacing = 0.0f;                        ///< 字间距
		int    mDigitCount = 0;                         ///< 固定位数（0=自动）
		float  mOpacity = 1.0f;                        ///< 透明度
	} UINumber, * LPUINumber;
}

#endif
