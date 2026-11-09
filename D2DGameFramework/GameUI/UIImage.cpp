#include "UIImage.h"
#include "../D2DCore/D2DRender.h"

namespace GameUI
{
	UIImage::UIImage()
	{
		mType = UIElementType::UI_Image;
		mIsActive = true;
	}

	UIImage::~UIImage()
	{
	}

	bool UIImage::Create(float x, float y, float width, float height,
		D2DCore::Texture* pTexture)
	{
		if (pTexture == nullptr)
		{
			return false;
		}

		SetPosition(x, y);
		SetSize(width, height);
		mPTexture = pTexture;
		mIsActive = true;
		return true;
	}

	void UIImage::Render()
	{
		if (!mIsActive || !IsVisible() || mPTexture == nullptr || !mPTexture->IsValid())
		{
			return;
		}

		auto* rt = D2DRd.GetD2DRenderTarget();
		if (rt == nullptr)
		{
			return;
		}

		const float texW = mPTexture->GetWidth();
		const float texH = mPTexture->GetHeight();
		if (texW <= 0.0f || texH <= 0.0f || mSize.x <= 0.0f || mSize.y <= 0.0f)
		{
			return;
		}

		// 元素未被父级裁剪前的世界矩形（mWorldRect 可能已被裁剪缩窄）
		MathType::RectF fullRect(
			mPosition.x - mSize.x * mAnchor.x,
			mPosition.y - mSize.y * mAnchor.y,
			mSize.x, mSize.y);
		if (mParent != nullptr)
		{
			const MathType::RectF& pr = mParent->GetWorldRect();
			fullRect.left += pr.left;
			fullRect.top += pr.top;
		}

		// 可见区域 = 元素矩形 ∩ 父级矩形
		// 越界图片按"窗口裁剪"处理（只绘制可见部分，不压缩变形）
		MathType::RectF parentRect = (mParent != nullptr)
			? mParent->GetWorldRect()
			: MathType::RectF(0.0f, 0.0f, fullRect.Width(), fullRect.Height());
		MathType::RectF clip;
		if (!fullRect.Overlap(parentRect, &clip) || clip.Width() <= 0.0f || clip.Height() <= 0.0f)
		{
			return;
		}

		// 按比例映射纹理上对应的可见源区域
		const float sx = (clip.left - fullRect.left) / fullRect.Width() * texW;
		const float sy = (clip.top - fullRect.top) / fullRect.Height() * texH;
		const float sw = clip.Width() / fullRect.Width() * texW;
		const float sh = clip.Height() / fullRect.Height() * texH;

		// 水平翻转：以可见区域中心为轴镜像绘制（用于方向相反的装饰箭头等）
		D2D1::Matrix3x2F previousTransform;
		rt->GetTransform(&previousTransform);
		if (mFlipX)
		{
			float cx = (clip.left + clip.right) * 0.5f;
			rt->SetTransform(D2D1::Matrix3x2F::Scale(-1.0f, 1.0f, D2D1::Point2F(cx, 0.0f)) * previousTransform);
		}

		rt->DrawBitmap(
			mPTexture->GetD2DBitmap(),
			D2D1::RectF(clip.left, clip.top, clip.right, clip.bottom),
			mOpacity,
			D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
			D2D1::RectF(sx, sy, sx + sw, sy + sh)
		);

		if (mFlipX)
		{
			rt->SetTransform(previousTransform);
		}
	}

	void UIImage::Update(float deltaTime)
	{
	}
}
