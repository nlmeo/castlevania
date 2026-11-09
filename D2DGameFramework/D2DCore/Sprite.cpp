#include "Sprite.h"
#include "D2DRender.h"
#include "Texture.h"

namespace D2DCore
{
	/**
 * @brief 构造函数
 *
 * 初始化成员变量为默认值
 */
	Sprite::Sprite()
		: m_pRenderTarget(nullptr)
		, m_pBitmapBrush(nullptr)
		, m_pD2DFactory(nullptr)
	{
	}

	/**
	 * @brief 析构函数
	 *
	 * 释放位图画刷资源
	 */
	Sprite::~Sprite()
	{
		SAFE_RELEASE(m_pBitmapBrush);
	}

	/**
	 * @brief 初始化精灵绘制器
	 *
	 * @param pD2DFactory D2D工厂接口
	 */
	void Sprite::Initialize()
	{
		//先初始化D2DRd,从D2DRd获取工厂和渲染目标
		m_pD2DFactory = D2DRd.GetD2DFactory();
		m_pRenderTarget = D2DRd.GetD2DRenderTarget();
	}

	/**
	 * @brief 绘制精灵（基础版本）
	 *
	 * 使用原始尺寸绘制纹理
	 *
	 * @param texture 纹理对象
	 * @param x X坐标
	 * @param y Y坐标
	 * @param opacity 透明度
	 */
	void Sprite::Draw(Texture* texture, float x, float y, float opacity)
	{
		if (!m_pRenderTarget || !texture || !texture->IsValid())
			return;

		// 获取纹理尺寸
		D2D1_SIZE_F size = texture->GetD2DBitmap()->GetSize();
		D2D1_RECT_F destRect = D2D1::RectF(x, y, x + size.width, y + size.height);
		D2D1_RECT_F srcRect = D2D1::RectF(0.0f, 0.0f, size.width, size.height);

		// 绘制位图
		m_pRenderTarget->DrawBitmap(
			texture->GetD2DBitmap(),
			destRect,
			opacity,
			D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
			&srcRect
		);
	}

	/**
	 * @brief 绘制精灵（缩放版本）
	 *
	 * 将纹理缩放到指定尺寸绘制
	 *
	 * @param texture 纹理对象
	 * @param x X坐标
	 * @param y Y坐标
	 * @param width 目标宽度
	 * @param height 目标高度
	 * @param opacity 透明度
	 */
	void Sprite::Draw(Texture* texture, float x, float y, float width, float height, float opacity)
	{
		if (!m_pRenderTarget || !texture || !texture->IsValid())
			return;

		// 获取纹理尺寸
		D2D1_SIZE_F size = texture->GetD2DBitmap()->GetSize();
		D2D1_RECT_F destRect = D2D1::RectF(x, y, x + width, y + height);
		D2D1_RECT_F srcRect = D2D1::RectF(0.0f, 0.0f, size.width, size.height);

		// 绘制位图
		m_pRenderTarget->DrawBitmap(
			texture->GetD2DBitmap(),
			destRect,
			opacity,
			D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
			&srcRect
		);
	}

	/**
	 * @brief 绘制精灵（带旋转缩放版本）
	 *
	 * 支持缩放和旋转的精灵绘制，旋转中心默认为目标矩形中心
	 *
	 * @param texture 纹理对象
	 * @param x X坐标
	 * @param y Y坐标
	 * @param scaleX X方向缩放因子
	 * @param scaleY Y方向缩放因子
	 * @param rotation 旋转角度（弧度）
	 * @param opacity 透明度
	 */
	void Sprite::Draw(Texture* texture, float x, float y, float scaleX, float scaleY, float rotation, float opacity)
	{
		if (!m_pRenderTarget || !texture || !texture->IsValid())
			return;

		ID2D1Bitmap* pBitmap = texture->GetD2DBitmap();
		D2D1_SIZE_F size = pBitmap->GetSize();

		// 计算旋转中心（目标矩形中心）
		float centerX = x + size.width * scaleX * 0.5f;
		float centerY = y + size.height * scaleY * 0.5f;

		ApplyTransform(x, y, scaleX, scaleY, rotation, centerX, centerY);

		// 计算目标矩形
		D2D1_RECT_F destRect = D2D1::RectF(x, y, x + size.width * scaleX, y + size.height * scaleY);
		D2D1_RECT_F srcRect = D2D1::RectF(0.0f, 0.0f, size.width, size.height);

		// 保存原始变换矩阵
		D2D1_MATRIX_3X2_F originalTransform;
		m_pRenderTarget->GetTransform(&originalTransform);

		// 设置旋转变换
		m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(rotation, D2D1::Point2F(centerX, centerY)));

		// 绘制位图
		m_pRenderTarget->DrawBitmap(
			pBitmap,
			destRect,
			opacity,
			D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
			&srcRect
		);

		// 恢复原始变换矩阵
		m_pRenderTarget->SetTransform(originalTransform);
	}

	/**
	 * @brief 绘制精灵（完整版）
	 *
	 * 支持源区域裁剪、目标区域定位、旋转、透明度和翻转
	 *
	 * @param texture 纹理对象
	 * @param srcRect 源纹理区域
	 * @param destRect 目标绘制区域
	 * @param rotation 旋转角度（弧度）
	 * @param rotationCenter 旋转中心点
	 * @param opacity 透明度
	 * @param flipX 是否水平翻转
	 * @param flipY 是否垂直翻转
	 */
	void Sprite::Draw(Texture* texture, const D2D1_RECT_F& srcRect, const D2D1_RECT_F& destRect,
		float rotation, const D2D1_POINT_2F& rotationCenter, float opacity, bool flipX, bool flipY)
	{
		if (!m_pRenderTarget || !texture || !texture->IsValid())
			return;

		ID2D1Bitmap* pBitmap = texture->GetD2DBitmap();

		// 如果需要旋转
		if (rotation != 0.0f)
		{
			// 保存原始变换矩阵
			D2D1_MATRIX_3X2_F originalTransform;
			m_pRenderTarget->GetTransform(&originalTransform);

			// 设置旋转变换
			m_pRenderTarget->SetTransform(
				D2D1::Matrix3x2F::Rotation(rotation, rotationCenter)
			);

			// 绘制位图
			m_pRenderTarget->DrawBitmap(
				pBitmap,
				destRect,
				opacity,
				D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
				&srcRect
			);

			// 恢复原始变换矩阵
			m_pRenderTarget->SetTransform(originalTransform);
		}
		else
		{
			// 不需要旋转，直接绘制
			m_pRenderTarget->DrawBitmap(
				pBitmap,
				destRect,
				opacity,
				D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
				&srcRect
			);
		}
	}

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
	void Sprite::ApplyTransform(float x, float y, float scaleX, float scaleY, float rotation,
		float rotationCenterX, float rotationCenterY, bool flipX, bool flipY)
	{
		// 预留接口，可在此实现复杂变换
	}
}
