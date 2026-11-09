#pragma once
#ifndef __SPRITE_RENDER_CORE_H__
#define __SPRITE_RENDER_CORE_H__

#include <windows.h>
#include <d2d1.h>
#include "../HelpTool/SingletonTemplate.h"

namespace D2DCore
{
	/**
	 * @brief 精灵绘制类
	 *
	 * 提供带变换（位置、旋转、缩放、透明度、翻转）的纹理绘制功能
	 */
	class Sprite : public Singleton<Sprite>
	{
		PRIVATE_COPY(Sprite)
	public:
		/** @brief 构造函数 */
		Sprite();

		/** @brief 析构函数 */
		~Sprite();

		/**
		 * @brief 初始化精灵绘制器
		 *
		 * @param pD2DFactory D2D工厂接口
		 */
		void Initialize();

		/**
		 * @brief 绘制精灵（基础版本）
		 *
		 * @param texture 纹理对象
		 * @param x X坐标
		 * @param y Y坐标
		 * @param opacity 透明度（默认1.0f，完全不透明）
		 */
		void Draw(class Texture* texture, float x, float y, float opacity = 1.0f);

		/**
		 * @brief 绘制精灵（缩放版本）
		 *
		 * @param texture 纹理对象
		 * @param x X坐标
		 * @param y Y坐标
		 * @param width 目标宽度
		 * @param height 目标高度
		 * @param opacity 透明度（默认1.0f）
		 */
		void Draw(class Texture* texture, float x, float y, float width, float height, float opacity = 1.0f);

		/**
		 * @brief 绘制精灵（带旋转缩放版本）
		 *
		 * @param texture 纹理对象
		 * @param x X坐标
		 * @param y Y坐标
		 * @param scaleX X方向缩放因子
		 * @param scaleY Y方向缩放因子
		 * @param rotation 旋转角度（弧度）
		 * @param opacity 透明度（默认1.0f）
		 */
		void Draw(class Texture* texture, float x, float y, float scaleX, float scaleY, float rotation, float opacity = 1.0f);

		/**
		 * @brief 绘制精灵（完整版）
		 *
		 * @param texture 纹理对象
		 * @param srcRect 源纹理区域
		 * @param destRect 目标绘制区域
		 * @param rotation 旋转角度（弧度）
		 * @param rotationCenter 旋转中心点
		 * @param opacity 透明度（默认1.0f）
		 * @param flipX 是否水平翻转（默认false）
		 * @param flipY 是否垂直翻转（默认false）
		 */
		void Draw(class Texture* texture, const D2D1_RECT_F& srcRect, const D2D1_RECT_F& destRect,
			float rotation = 0.0f, const D2D1_POINT_2F& rotationCenter = D2D1::Point2F(),
			float opacity = 1.0f, bool flipX = false, bool flipY = false);

		/** @brief 设置渲染目标 */
		void SetRenderTarget(ID2D1RenderTarget* pRenderTarget) { m_pRenderTarget = pRenderTarget; }

		/** @brief 设置位图画刷 */
		void SetBitmapBrush(ID2D1BitmapBrush* pBrush) { m_pBitmapBrush = pBrush; }

	private:
		/**
		 * @brief 应用变换矩阵
		 *
		 * @param x X坐标
		 * @param y Y坐标
		 * @param scaleX X方向缩放因子
		 * @param scaleY Y方向缩放因子
		 * @param rotation 旋转角度（弧度）
		 * @param rotationCenterX 旋转中心X坐标
		 * @param rotationCenterY 旋转中心Y坐标
		 * @param flipX 是否水平翻转
		 * @param flipY 是否垂直翻转
		 */
		void ApplyTransform(float x, float y, float scaleX, float scaleY, float rotation,
			float rotationCenterX, float rotationCenterY, bool flipX = false, bool flipY = false);

		ID2D1RenderTarget* m_pRenderTarget;  ///< D2D渲染目标
		ID2D1BitmapBrush* m_pBitmapBrush;     ///< 位图画刷
		ID2D1Factory* m_pD2DFactory;         ///< D2D工厂接口
	};
}

#define D2DSp D2DCore::Sprite::Instance()
#endif 


