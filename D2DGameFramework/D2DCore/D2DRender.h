#pragma once
#ifndef __D2D_CORE_RENDER_H__
#define __D2D_CORE_RENDER_H__

#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include "../HelpTool/SingletonTemplate.h"
#include "../TypeDefine/Color.h"

namespace D2DCore
{
	class D2DRender:public Singleton<D2DRender>
	{
		PRIVATE_COPY(D2DRender)
	public:
		D2DRender();
		~D2DRender();

		bool Initialize();
		void Resize(int width, int height);
		void BeginDraw();
		void EndDraw();

		void Clear(float r, float g, float b, float a = 1.0f);
		void Clear(const  ColorType::COLORF& color);
		/**
		 * @brief 绘制线段
		 *
		 * @param x1 起点X坐标
		 * @param y1 起点Y坐标
		 * @param x2 终点X坐标
		 * @param y2 终点Y坐标
		 * @param color 线段颜色
		 * @param strokeWidth 线宽（默认1.0f）
		 */
		void DrawLine(float x1, float y1, float x2, float y2, 
			const ColorType::COLORF& color, float strokeWidth = 1.0f);
		/**
		 * @brief 绘制矩形（填充+描边）
		 *
		 * @param x 左上角X坐标
		 * @param y 左上角Y坐标
		 * @param w 宽度
		 * @param h 高度
		 * @param strokeColor 描边颜色
		 * @param strokeWidth 描边宽度
		 * @param fillColor 填充颜色
		 */
		void DrawRect(float x, float y, float w, float h, 
			const ColorType::COLORF& strokeColor, float strokeWidth, 
			const ColorType::COLORF& fillColor);
		/**
		 * @brief 绘制矩形（仅填充）
		 *
		 * @param x 左上角X坐标
		 * @param y 左上角Y坐标
		 * @param w 宽度
		 * @param h 高度
		 * @param fillColor 填充颜色
		 */
		void DrawRect(float x, float y, float w, float h, const ColorType::COLORF& fillColor);
		/**
		 * @brief 绘制圆形（仅填充）
		 *
		 * @param cx 圆心X坐标
		 * @param cy 圆心Y坐标
		 * @param radius 半径
		 * @param fillColor 填充颜色
		 */
		void DrawCircle(float cx, float cy, float radius, const ColorType::COLORF& fillColor);
		/**
		 * @brief 绘制圆形（填充+描边）
		 *
		 * @param cx 圆心X坐标
		 * @param cy 圆心Y坐标
		 * @param radius 半径
		 * @param strokeColor 描边颜色
		 * @param strokeWidth 描边宽度
		 * @param fillColor 填充颜色
		 */
		void DrawCircle(float cx, float cy, float radius, 
			const ColorType::COLORF& strokeColor, float strokeWidth,
			const ColorType::COLORF& fillColor);
		/**
		 * @brief 绘制圆角矩形（仅填充）
		 *
		 * @param x 左上角X坐标
		 * @param y 左上角Y坐标
		 * @param w 宽度
		 * @param h 高度
		 * @param radiusX X方向圆角半径
		 * @param radiusY Y方向圆角半径
		 * @param fillColor 填充颜色
		 */
		void DrawRoundedRect(float x, float y, float w, float h, 
			float radiusX, float radiusY, const ColorType::COLORF& fillColor);
		/**
		 * @brief 绘制圆角矩形（填充+描边）
		 *
		 * @param x 左上角X坐标
		 * @param y 左上角Y坐标
		 * @param w 宽度
		 * @param h 高度
		 * @param radiusX X方向圆角半径
		 * @param radiusY Y方向圆角半径
		 * @param strokeColor 描边颜色
		 * @param strokeWidth 描边宽度
		 * @param fillColor 填充颜色
		 */
		void DrawRoundedRect(float x, float y, float w, float h, 
			float radiusX, float radiusY, const ColorType::COLORF& strokeColor,
			float strokeWidth, const ColorType::COLORF& fillColor);

		/** @brief 获取渲染宽度 */
		int GetTargetWidth() const { return m_width; }

		/** @brief 获取渲染高度 */
		int GetTargetHeight() const { return m_height; }

		/** @brief 获取底层D2D RenderTarget */
		ID2D1HwndRenderTarget* GetD2DRenderTarget() const { return m_pRenderTarget; }

		/** @brief 获取底层D2D Factory */
		ID2D1Factory* GetD2DFactory() const { return m_pD2DFactory; }

		/** @brief 获取当前画刷 */
		ID2D1SolidColorBrush* GetBrush() const { return m_pBrush; }

		/** @brief 设置画刷颜色 */
		void SetBrushColor(const ColorType::COLORF& color);

		/**
		 * @brief 将当前渲染目标内容复制到Texture对象
		 *
		 * 将RenderTarget当前内容（BeginDraw和EndDraw之间的绘制结果）复制到指定的Texture对象
		 *
		 * @param texture 目标Texture对象指针（必须为有效对象）
		 * @return 成功返回true，失败返回false
		 */
		bool RenderToTexture(class Texture* texture);

		bool IsInit() const { return m_isInit; }
	protected:
		/** @brief 创建设备相关资源 */
		bool _CreateDeviceResources();
	private:
		bool m_isInit;						///< 是否渲染
		HWND m_hwnd;                        ///< 窗口句柄
		int m_width;                        ///< 渲染宽度
		int m_height;                       ///< 渲染高度

		ID2D1Factory* m_pD2DFactory;               ///< D2D工厂接口
		ID2D1HwndRenderTarget* m_pRenderTarget;    ///< D2D渲染目标
		ID2D1SolidColorBrush* m_pBrush;            ///< 纯色画刷
	};
#define SAFE_RELEASE(p)		if(p!=nullptr){p->Release();p=nullptr;}
#define D2DRd D2DCore::D2DRender::Instance()
}
#endif