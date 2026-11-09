#include "GameObject.h"
#include "../D2DCore/D2DRender.h"


namespace GamePlay
{
	void SpriteRender::CaclRenderMatrix()
	{
		auto& matrixOrder = transform()->MatOrder();
		auto& matT = transform()->MatT();
		auto& matS = transform()->MatS();
		auto& matR = transform()->MatR();

		switch (matrixOrder)
		{
		case GamePlay::Transform::MatrixOrder::RST:
		{
			mTransformMatrix.SetTranslate(-mDestRect.right * mAnchor.x, -mDestRect.bottom * mAnchor.y);
			((mTransformMatrix *= matR) *= matS) *= matT;
		}break;
		case GamePlay::Transform::MatrixOrder::SRT:
		{
			mTransformMatrix.SetTranslate(-mDestRect.right * mAnchor.x, -mDestRect.bottom * mAnchor.y);
			((mTransformMatrix *= matS) *= matR) *= matT;
		}break;
		case GamePlay::Transform::MatrixOrder::STR:
		{
			mTransformMatrix.SetTranslate(-mDestRect.right * mAnchor.x, -mDestRect.bottom * mAnchor.y);
			((mTransformMatrix *= matS) *= matT) *= matR;
		}break;
		case GamePlay::Transform::MatrixOrder::TSR:
		{
			mTransformMatrix.SetTranslate(-mDestRect.right * mAnchor.x, -mDestRect.bottom * mAnchor.y);
			((mTransformMatrix *= matT) *= matS) *= matR;
		}break;
		case GamePlay::Transform::MatrixOrder::TRS:
		{
			mTransformMatrix.SetTranslate(-mDestRect.right * mAnchor.x, -mDestRect.bottom * mAnchor.y);
			((mTransformMatrix *= matT) *= matR) *= matS;
		}break;
		case GamePlay::Transform::MatrixOrder::RTS:
		{
			mTransformMatrix.SetTranslate(-mDestRect.right * mAnchor.x, -mDestRect.bottom * mAnchor.y);
			((mTransformMatrix *= matR) *= matT) *= matS;
		}break;
		}

		MathType::vec2f corner[4] = {
			{0,0},//左上
			{mDestRect.right,0},//右上
			{mDestRect.right,mDestRect.bottom},//右下
			{0,mDestRect.bottom}//左下
		};

		float& minx = mRenderRect.left;
		float& maxx = mRenderRect.right;
		float& miny = mRenderRect.top;
		float& maxy = mRenderRect.bottom;

		for (int i = 0; i < 4; ++i)
		{
			corner[i] *= mTransformMatrix;
		}

		minx = corner[0].x;
		maxx = corner[0].x;
		miny = corner[0].y;
		maxy = corner[0].y;

		for (int i=1;i<4;++i)
		{
			if (minx > corner[i].x) { minx = corner[i].x; }
			if (maxx < corner[i].x) { maxx = corner[i].x; }
			if (miny > corner[i].y) { miny = corner[i].y; }
			if (maxy < corner[i].y) { maxy = corner[i].y; }
		}
	}

	SpriteRender::~SpriteRender()
	{
		InvalidateTintResources();
	}

	void SpriteRender::SetTexture(D2DCore::Texture* tex)
	{
		if (tex)
		{
			// 如果底层位图发生变化，释放旧的遮罩资源
			if (mTexture == nullptr || mTexture->GetD2DBitmap() != tex->GetD2DBitmap())
			{
				InvalidateTintResources();
			}
			mTexture = tex;
			mSrcRect = { 0, 0, mTexture->GetWidth(), mTexture->GetHeight() };
			mDestRect = mSrcRect;
		}
	}

	void SpriteRender::InvalidateTintResources()
	{
		SAFE_RELEASE(mpTintMaskBrush);
		SAFE_RELEASE(mpTintLayer);
	}

	void SpriteRender::Render(const MathType::Mat3x3f& vpv)
	{
		
		D2D1_MATRIX_3X2_F originalTransform; //原始变换矩阵
		auto pD2DTarget = D2DRd.GetD2DRenderTarget();
		if (pD2DTarget!=nullptr && mTexture != nullptr)
		{
			// 保存原始变换矩阵
			pD2DTarget->GetTransform(&originalTransform);			
			// 设置矩阵
			pD2DTarget->SetTransform((D2D1_MATRIX_3X2_F*)(mTransformMatrix * vpv).GetMat3x2());
			// 使用D2D进行绘制
			pD2DTarget->DrawBitmap(mTexture->GetD2DBitmap(), mDestRect,
				mOpacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, mSrcRect);

			// 着色叠加层：使用 PushLayer + BitmapBrush 作为逐像素 Alpha 遮罩
			if (mTintColor.a > 0.001f)
			{
				ID2D1Bitmap* pBitmap = mTexture->GetD2DBitmap();

				// 延迟创建遮罩画刷（只在位图首次使用时创建）
				if (mpTintMaskBrush == nullptr && pBitmap != nullptr)
				{
					D2D1_BITMAP_BRUSH_PROPERTIES bmpBrushProps = D2D1::BitmapBrushProperties(
						D2D1_EXTEND_MODE_CLAMP,
						D2D1_EXTEND_MODE_CLAMP,
						D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
					pD2DTarget->CreateBitmapBrush(
						pBitmap,
						bmpBrushProps,
						&mpTintMaskBrush			
					);
				}

				// 延迟创建 Layer
				if (mpTintLayer == nullptr)
				{
					pD2DTarget->CreateLayer(&mpTintLayer);
				}

				if (mpTintLayer != nullptr && mpTintMaskBrush != nullptr)
				{
					// 以位图画刷作为不透明度遮罩，逐像素控制着色可见性
					D2D1_LAYER_PARAMETERS layerParams = D2D1::LayerParameters(
						mDestRect,                 // 裁剪范围
						nullptr,                   // 不使用几何遮罩
						D2D1_ANTIALIAS_MODE_ALIASED,
						D2D1::IdentityMatrix(),
						1.0f,                      // 层整体不透明度
						mpTintMaskBrush,           // 逐像素 Alpha 遮罩
						D2D1_LAYER_OPTIONS_NONE
					);

					pD2DTarget->PushLayer(layerParams, mpTintLayer);
					D2DRd.SetBrushColor(mTintColor);
					pD2DTarget->FillRectangle(mDestRect, D2DRd.GetBrush());
					pD2DTarget->PopLayer();
				}
			}

			// 恢复原始变换矩阵
			pD2DTarget->SetTransform(originalTransform);
		}
	}
	const MathType::RectF& SpriteRender::GetRenderRect() const
	{
		return mRenderRect;
	}
}
