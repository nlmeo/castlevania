#include "UISliderBar.h"
#include "../D2DCore/D2DRender.h"

namespace GameUI
{
	UISliderBar::UISliderBar()
	{
		mType = UIElementType::UI_SliderBar;
		mUseTextureMode = false;
		mPBackGround = nullptr;
		mPCover = nullptr;
		mPHandle = nullptr;
		mBackColor = ColorType::COLORF(0.25f, 0.25f, 0.3f, 1.0f);
		mFillColor = ColorType::COLORF(0.3f, 0.6f, 0.9f, 1.0f);
		mHandleColor = ColorType::COLORF(0.5f, 0.7f, 1.0f, 1.0f);
		mValue = 0.0f;
		mHandleWidth = 20.0f;
		mCanMove = false;
		mOnValueChanged = nullptr;
	}

	UISliderBar::~UISliderBar()
	{
	}

	bool UISliderBar::CreateWithTexture(float x, float y, float width, float height,
		D2DCore::Texture* pBackGround, D2DCore::Texture* pCover,
		D2DCore::Texture* pHandle, float value)
	{
		mUseTextureMode = true;
		SetPosition(x, y);
		SetSize(width, height);
		mPBackGround = pBackGround;
		mPCover = pCover;
		mPHandle = pHandle;
		mValue = max(0.0f, min(1.0f, value));
		mHandleWidth = (pHandle != nullptr) ? pHandle->GetWidth() : 20.0f;
		mIsActive = true;
		return true;
	}

	bool UISliderBar::CreateWithColor(float x, float y, float width, float height,
		float handleWidth,
		const ColorType::COLORF& backColor,
		const ColorType::COLORF& fillColor,
		const ColorType::COLORF& handleColor,
		float value)
	{
		mUseTextureMode = false;
		SetPosition(x, y);
		SetSize(width, height);
		mHandleWidth = handleWidth;
		mBackColor = backColor;
		mFillColor = fillColor;
		mHandleColor = handleColor;
		mValue = max(0.0f, min(1.0f, value));
		mIsActive = true;
		return true;
	}

	float UISliderBar::GetValue() const
	{
		return mValue;
	}

	void UISliderBar::SetValue(float value)
	{
		mValue = max(0.0f, min(1.0f, value));

		if (mOnValueChanged)
		{
			mOnValueChanged(mValue);
		}
	}

	void UISliderBar::SetOnValueChanged(std::function<void(float)> callback)
	{
		mOnValueChanged = callback;
	}

	void UISliderBar::Render()
	{
		if (!mIsActive || !IsVisible())
		{
			return;
		}

		auto* rt = D2DRd.GetD2DRenderTarget();
		if (rt == nullptr)
		{
			return;
		}

		float barTop = mWorldRect.top + (mWorldRect.Height() - 8.0f) / 2.0f;
		float barBottom = barTop + 8.0f;
		float fillRight = mWorldRect.left + mValue * (mWorldRect.Width() - mHandleWidth) + mHandleWidth / 2.0f;

		if (mUseTextureMode)
		{
			if (mPBackGround != nullptr && mPBackGround->IsValid())
			{
				rt->DrawBitmap(
					mPBackGround->GetD2DBitmap(),
					D2D1::RectF(mWorldRect.left, barTop, mWorldRect.right, barBottom),
					1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
					D2D1::RectF(0, 0, mPBackGround->GetWidth(), mPBackGround->GetHeight())
				);
			}

			if (mPCover != nullptr && mPCover->IsValid())
			{
				rt->DrawBitmap(
					mPCover->GetD2DBitmap(),
					D2D1::RectF(mWorldRect.left, barTop, fillRight, barBottom),
					1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
					D2D1::RectF(0, 0, mPCover->GetWidth(), mPCover->GetHeight())
				);
			}

			if (mPHandle != nullptr && mPHandle->IsValid())
			{
				float handleLeft = mWorldRect.left + mValue * (mWorldRect.Width() - mHandleWidth);
				rt->DrawBitmap(
					mPHandle->GetD2DBitmap(),
					D2D1::RectF(handleLeft, mWorldRect.top, handleLeft + mHandleWidth, mWorldRect.bottom),
					1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
					D2D1::RectF(0, 0, mPHandle->GetWidth(), mPHandle->GetHeight())
				);
			}
		}
		else
		{
			D2DRd.SetBrushColor(mBackColor);
			rt->FillRectangle(
				D2D1::RectF(mWorldRect.left, barTop, mWorldRect.right, barBottom),
				D2DRd.GetBrush()
			);

			D2DRd.SetBrushColor(mFillColor);
			rt->FillRectangle(
				D2D1::RectF(mWorldRect.left, barTop, fillRight, barBottom),
				D2DRd.GetBrush()
			);

			float handleLeft = mWorldRect.left + mValue * (mWorldRect.Width() - mHandleWidth);
			D2DRd.SetBrushColor(mHandleColor);
			rt->FillRectangle(
				D2D1::RectF(handleLeft, mWorldRect.top, handleLeft + mHandleWidth, mWorldRect.bottom),
				D2DRd.GetBrush()
			);
		}
	}

	void UISliderBar::Update(float deltaTime)
	{
	}

	void UISliderBar::OnMouseDown(int x, int y)
	{
		POINT pt = { x, y };
		RECT wr = {
			static_cast<LONG>(mWorldRect.left), static_cast<LONG>(mWorldRect.top),
			static_cast<LONG>(mWorldRect.right), static_cast<LONG>(mWorldRect.bottom)
		};

		if (PtInRect(&wr, pt))
		{
			mCanMove = true;
			SetValue(_CalcValueFromMouse(x));
		}
	}

	void UISliderBar::OnMouseUp(int x, int y)
	{
		mCanMove = false;
	}

	void UISliderBar::OnMouseMove(int x, int y)
	{
		if (mCanMove)
		{
			SetValue(_CalcValueFromMouse(x));
		}
	}

	float UISliderBar::_CalcValueFromMouse(int mouseX)
	{
		float relX = static_cast<float>(mouseX) - mWorldRect.left - mHandleWidth / 2.0f;
		float range = mWorldRect.Width() - mHandleWidth;
		if (range <= 0.0f)
		{
			return mValue;
		}
		return max(0.0f, min(1.0f, relX / range));
	}
}
