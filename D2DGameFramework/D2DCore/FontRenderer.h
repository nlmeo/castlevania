#pragma once

#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <vector>
#include "../TypeDefine/Color.h"
#include "../HelpTool/SingletonTemplate.h"

namespace D2DCore
{
	/**
 * @brief 字体渲染器类
 *
 * 通过DirectWrite提供不同字体的文字绘制功能，支持从系统字体和字体文件加载
 */
	class FontRenderer:public Singleton<FontRenderer>
	{
		PRIVATE_COPY(FontRenderer)
	public:
		/** @brief 构造函数 */
		FontRenderer();

		/** @brief 析构函数 */
		~FontRenderer();

		/**
		 * @brief 初始化字体渲染器
		 *
		 * @param pRenderTarget D2D渲染目标
		 * @return 初始化成功返回true，失败返回false
		 */
		bool Initialize();

		/**
		 * @brief 从系统字体创建字体
		 *
		 * @param fontName 字体名称
		 * @param fontSize 字体大小
		 * @param bold 是否粗体（默认false）
		 * @param italic 是否斜体（默认false）
		 * @return 成功返回字体ID，失败返回-1
		 */
		int CreateFont(const wchar_t* fontName, float fontSize, bool bold = false, bool italic = false);

		/**
		 * @brief 从字体文件创建字体
		 *
		 * @param filePath 字体文件路径（支持TTF格式）
		 * @param fontSize 字体大小
		 * @param bold 是否粗体（默认false）
		 * @param italic 是否斜体（默认false）
		 * @return 成功返回字体ID，失败返回-1
		 */
		int CreateFontFromFile(const wchar_t* filePath, float fontSize, bool bold = false, bool italic = false);

		/**
		 * @brief 绘制文字（简单版本）
		 *
		 * @param text 要绘制的文字
		 * @param fontId 字体ID
		 * @param x X坐标
		 * @param y Y坐标
		 * @param color 文字颜色
		 */
		void DrawText(const wchar_t* text, int fontId, float x, float y, const ColorType::COLORF& color);

		/**
		 * @brief 绘制文字（带对齐版本）
		 *
		 * @param text 要绘制的文字
		 * @param fontId 字体ID
		 * @param layoutRect 布局矩形
		 * @param color 文字颜色
		 * @param hAlign 水平对齐方式（默认左对齐）
		 * @param vAlign 垂直对齐方式（默认顶部对齐）
		 */
		void DrawText(const wchar_t* text, int fontId, const D2D1_RECT_F& layoutRect, const ColorType::COLORF& color,
			DWRITE_TEXT_ALIGNMENT hAlign = DWRITE_TEXT_ALIGNMENT_LEADING,
			DWRITE_PARAGRAPH_ALIGNMENT vAlign = DWRITE_PARAGRAPH_ALIGNMENT_NEAR,
			bool wordWrap = false);

		/**
		 * @brief 测量文字尺寸
		 *
		 * @param text 要测量的文字
		 * @param fontId 字体ID
		 * @param maxWidth 最大宽度（默认FLT_MAX）
		 * @param maxHeight 最大高度（默认FLT_MAX）
		 * @return 文字尺寸
		 */
		D2D1_SIZE_F MeasureText(const wchar_t* text, int fontId, float maxWidth = FLT_MAX, float maxHeight = FLT_MAX);

	private:
		ID2D1RenderTarget* m_pRenderTarget;    ///< D2D渲染目标
		IDWriteFactory* m_pDWriteFactory;       ///< DirectWrite工厂
		ID2D1SolidColorBrush* m_pBrush;         ///< 画刷
		std::vector<IDWriteTextFormat*> m_fontFormats;  ///< 字体格式列表
	};
#define FontMgr D2DCore::FontRenderer::Instance()
}