#pragma once
#ifndef __GAME_UI_IMAGE_H__
#define __GAME_UI_IMAGE_H__

#include "UIElement.h"
#include "../D2DCore/Texture.h"

namespace GameUI
{
	/**
	 * @brief 纯图片 UI 元素
	 *
	 * 仅用于在 UI 层绘制一张静态纹理（如血条背景、图标等），
	 * 无交互、无状态，是最轻量的 UI 元素。
	 */
	typedef class UIImage : public UIElement
	{
	public:
		/** @brief 构造函数 */
		UIImage();

		/** @brief 析构函数 */
		virtual ~UIImage();

		/**
		 * @brief 初始化
		 *
		 * @param x       左上角 X 坐标
		 * @param y       左上角 Y 坐标
		 * @param width   绘制宽度
		 * @param height  绘制高度
		 * @param pTexture 纹理指针
		 * @return true 成功
		 */
		bool Create(float x, float y, float width, float height,
			D2DCore::Texture* pTexture);

		/** @brief 渲染图片 */
		virtual void Render() override;

		/** @brief 空实现（纯图片无需更新） */
		virtual void Update(float deltaTime) override;

		/**
		 * @brief 设置透明度
		 * @param opacity 0.0~1.0
		 */
		void SetOpacity(float opacity) { mOpacity = MathType::clamp(opacity, 0.0f, 1.0f); }

		/** @brief 获取当前透明度 */
		float GetOpacity() const { return mOpacity; }

		/** @brief 运行时替换纹理（用于序列帧动画） */
		void SetTexture(D2DCore::Texture* pTexture) { mPTexture = pTexture; }

		/** @brief 获取当前纹理 */
		D2DCore::Texture* GetTexture() const { return mPTexture; }

	protected:
		D2DCore::Texture* mPTexture = nullptr; ///< 纹理
		float mOpacity = 1.0f;                 ///< 透明度
	} UIImage, * LPUIImage;
}

#endif
