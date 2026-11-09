#pragma once
#ifndef __GAME_UI_TEXT_H__
#define __GAME_UI_TEXT_H__

#include "UIElement.h"
#include "../D2DCore/FontRenderer.h"

namespace GameUI
{
	/**
	 * @brief 纯文字 UI 元素
	 *
	 * 用于在 UI 层绘制一段文字，支持字体、颜色、对齐方式设置。
	 * 无交互、无状态，是最轻量的文本 UI 元素。
	 */
	class UIText : public UIElement
	{
	public:
		/** @brief 构造函数 */
		UIText();

		/** @brief 析构函数 */
		virtual ~UIText();

		/**
		 * @brief 初始化文本
		 *
		 * @param text   要显示的文字（宽字符）
		 * @param fontId 字体 ID（由 FontMgr.CreateFont 返回）
		 * @param color  文字颜色
		 * @param x      左上角 X 坐标
		 * @param y      左上角 Y 坐标
		 */
		void Create(const std::wstring& text, int fontId, const ColorType::COLORF& color,
			float x = 0.0f, float y = 0.0f);

		/**
		 * @brief 设置水平/垂直对齐方式
		 *
		 * @param hAlign 水平对齐（默认左对齐）
		 * @param vAlign 垂直对齐（默认顶部对齐）
		 */
		void SetAlignment(DWRITE_TEXT_ALIGNMENT hAlign, DWRITE_PARAGRAPH_ALIGNMENT vAlign);

		/** @brief 渲染文字 */
		virtual void Render() override;

		/** @brief 空实现（纯文字无需更新） */
		virtual void Update(float deltaTime) override;

		/** @brief 修改文字内容 */
		void SetText(const std::wstring& text)
		{
			mText = text;
			D2D1_SIZE_F size = FontMgr.MeasureText(mText.c_str(), mFontId);
			SetSize(size.width, size.height);
		}

		/** @brief 获取当前文字 */
		const std::wstring& GetText() const { return mText; }

		/** @brief 设置透明度（0~1，影响整体文字渲染） */
		void SetOpacity(float opacity) { mOpacity = MathType::clamp(opacity, 0.0f, 1.0f); }

		/** @brief 获取当前透明度 */
		float GetOpacity() const { return mOpacity; }

		/** @brief 直接设置颜色（不受 opacity 影响） */
		void SetColor(const ColorType::COLORF& color) { mColor = color; }

		/** @brief 获取颜色 */
		const ColorType::COLORF& GetColor() const { return mColor; }

	private:
		std::wstring mText;                                   ///< 文字内容
		int mFontId = -1;                                     ///< 字体 ID
		ColorType::COLORF mColor;                              ///< 文字颜色（原始颜色）
		float mOpacity = 1.0f;                                 ///< 透明度
		DWRITE_TEXT_ALIGNMENT mHAlign = DWRITE_TEXT_ALIGNMENT_LEADING;          ///< 水平对齐
		DWRITE_PARAGRAPH_ALIGNMENT mVAlign = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;   ///< 垂直对齐
	};
}

#endif
