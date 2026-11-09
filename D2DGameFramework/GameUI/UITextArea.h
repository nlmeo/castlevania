#pragma once
#ifndef __GAME_UI_TEXT_AREA_H__
#define __GAME_UI_TEXT_AREA_H__

#include "UIElement.h"
#include "../D2DCore/FontRenderer.h"

namespace GameUI
{
	/**
	 * @brief 多行文本区域 UI 元素
	 *
	 * 在固定尺寸的矩形区域内渲染文字，超出宽度自动换行，超出高度被裁剪。
	 * 底层使用 FontMgr.DrawText(..., wordWrap=true)。
	 * 与单行 UIText 独立，互不影响。
	 */
	class UITextArea : public UIElement
	{
	public:
		/** @brief 构造函数 */
		UITextArea();

		/** @brief 析构函数 */
		virtual ~UITextArea();

		/**
		 * @brief 初始化多行文本区域
		 *
		 * @param text   要显示的文字
		 * @param fontId 字体 ID
		 * @param color  文字颜色
		 * @param x      左上角 X 坐标
		 * @param y      左上角 Y 坐标
		 * @param width  文本区域宽度（换行边界）
		 * @param height 文本区域高度（超出裁剪）
		 */
		void Create(const std::wstring& text, int fontId, const ColorType::COLORF& color,
			float x, float y, float width, float height);

		/**
		 * @brief 设置水平/垂直对齐方式
		 *
		 * @param hAlign 水平对齐（默认左对齐）
		 * @param vAlign 垂直对齐（默认顶部对齐）
		 */
		void SetAlignment(DWRITE_TEXT_ALIGNMENT hAlign = DWRITE_TEXT_ALIGNMENT_LEADING,
			DWRITE_PARAGRAPH_ALIGNMENT vAlign = DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

		/** @brief 渲染文字（使用 wordWrap=true） */
		virtual void Render() override;

		/** @brief 空实现 */
		virtual void Update(float deltaTime) override;

		/** @brief 修改文字内容 */
		void SetText(const std::wstring& text)
		{
			mText = text;
		}

		/** @brief 获取当前文字 */
		const std::wstring& GetText() const { return mText; }

		/** @brief 设置透明度 */
		void SetOpacity(float opacity) { mOpacity = MathType::clamp(opacity, 0.0f, 1.0f); }

		/** @brief 获取当前透明度 */
		float GetOpacity() const { return mOpacity; }

		/** @brief 设置颜色 */
		void SetColor(const ColorType::COLORF& color) { mColor = color; }

		/** @brief 获取颜色 */
		const ColorType::COLORF& GetColor() const { return mColor; }

	private:
		std::wstring mText;                                      ///< 文字内容
		int mFontId = -1;                                        ///< 字体 ID
		ColorType::COLORF mColor;                                 ///< 文字颜色
		float mOpacity = 1.0f;                                    ///< 透明度
		DWRITE_TEXT_ALIGNMENT mHAlign = DWRITE_TEXT_ALIGNMENT_LEADING;          ///< 水平对齐
		DWRITE_PARAGRAPH_ALIGNMENT mVAlign = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;   ///< 垂直对齐
	};
}

#endif
