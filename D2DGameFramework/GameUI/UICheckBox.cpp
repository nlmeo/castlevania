#include "UICheckBox.h"
#include "../D2DCore/D2DRender.h"

namespace GameUI
{
	UICheckBox::UICheckBox()
	{
		mType = UIElementType::UI_CheckBox;
		mPTexture = nullptr;
		mChecked = false;
		mOnCheckedChanged = nullptr;
	}

	UICheckBox::~UICheckBox()
	{
	}

	bool UICheckBox::Create(float x, float y, float width, float height,
		D2DCore::Texture* pTexture, bool checked)
	{
		if (pTexture == nullptr)
		{
			return false;
		}

		SetPosition(x, y);
		SetSize(width, height);
		mPTexture = pTexture;
		mChecked = checked;
		mIsActive = true;
		return true;
	}

	bool UICheckBox::IsChecked() const
	{
		return mChecked;
	}

	void UICheckBox::SetChecked(bool checked)
	{
		if (mChecked != checked)
		{
			mChecked = checked;
			if (mOnCheckedChanged)
			{
				mOnCheckedChanged(mChecked);
			}
		}
	}

	void UICheckBox::SetOnCheckedChanged(std::function<void(bool)> callback)
	{
		mOnCheckedChanged = callback;
	}

	void UICheckBox::Render()
	{
		if (!mIsActive || !IsVisible())
		{
			return;
		}

		if (mPTexture == nullptr || !mPTexture->IsValid())
		{
			return;
		}

		auto* rt = D2DRd.GetD2DRenderTarget();
		if (rt == nullptr)
		{
			return;
		}

		float sliceWidth = mPTexture->GetWidth() / 2.0f;
		int stateIndex = mChecked ? 1 : 0;

		rt->DrawBitmap(
			mPTexture->GetD2DBitmap(),
			D2D1::RectF(mWorldRect.left, mWorldRect.top, mWorldRect.right, mWorldRect.bottom),
			1.0f,
			D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
			D2D1::RectF(stateIndex * sliceWidth, 0,
				(stateIndex + 1) * sliceWidth, mPTexture->GetHeight())
		);
	}

	void UICheckBox::Update(float deltaTime)
	{
	}

	void UICheckBox::OnMouseDown(int x, int y)
	{
		POINT pt = { x, y };
		RECT wr = {
			static_cast<LONG>(mWorldRect.left), static_cast<LONG>(mWorldRect.top),
			static_cast<LONG>(mWorldRect.right), static_cast<LONG>(mWorldRect.bottom)
		};

		if (PtInRect(&wr, pt))
		{
			SetChecked(!mChecked);
		}
	}
}
