#include "UIButton.h"
#include "../D2DCore/D2DRender.h"
#include "../HelpTool/EnCoding.h"

namespace GameUI
{
	UIButton::UIButton()
	{
		mType = UIElementType::UI_Button;
		mUseTextureMode = false;
		mPTexture = nullptr;
		mState = UIButtonState::UIBS_Normal;
		mNormalColor = ColorType::COLORF(0.3f, 0.3f, 0.35f, 1.0f);
		mHoverColor = ColorType::COLORF(0.4f, 0.4f, 0.45f, 1.0f);
		mPressedColor = ColorType::COLORF(0.2f, 0.2f, 0.25f, 1.0f);
		mCornerRadius = 4.0f;
		mHasText = false;
		mFontId = -1;
		mFontSize = 16.0f;
	}

	UIButton::~UIButton()
	{
	}

	bool UIButton::CreateWithTexture(float x, float y, float width, float height,
		D2DCore::Texture* pTexture)
	{
		if (pTexture == nullptr)
		{
			return false;
		}

		mUseTextureMode = true;
		SetPosition(x, y);
		SetSize(width, height);
		mPTexture = pTexture;
		mIsActive = true;
		return true;
	}

	bool UIButton::CreateWithColor(float x, float y, float width, float height,
		const ColorType::COLORF& normalColor,
		const ColorType::COLORF& hoverColor,
		const ColorType::COLORF& pressedColor,
		float cornerRadius)
	{
		mUseTextureMode = false;
		SetPosition(x, y);
		SetSize(width, height);
		mNormalColor = normalColor;
		mHoverColor = hoverColor;
		mPressedColor = pressedColor;
		mCornerRadius = cornerRadius;
		mIsActive = true;
		return true;
	}

	void UIButton::SetText(const std::string& text, const std::wstring& fontName, float fontSize)
	{
		mHasText = true;
		mText = text;
		mFontSize = fontSize;

		mFontId = FontMgr.CreateFont(fontName.c_str(), fontSize);
	}

	void UIButton::Render()
	{
		if (!mIsActive || !IsVisible())
		{
			return;
		}

		if (mUseTextureMode)
		{
			_RenderTexture();
		}
		else
		{
			_RenderColor();
		}

		if (mHasText && mFontId >= 0)
		{
			std::wstring wtext = HelpTool::EnCodeing::C2W(mText.c_str());
			FontMgr.DrawText(
				wtext.c_str(),
				mFontId,
				D2D1::RectF(mWorldRect.left, mWorldRect.top, mWorldRect.right, mWorldRect.bottom),
				ColorType::COLORF::White(),
				DWRITE_TEXT_ALIGNMENT_CENTER,
				DWRITE_PARAGRAPH_ALIGNMENT_CENTER
			);
		}
	}

	void UIButton::Update(float deltaTime)
	{
	}

	void UIButton::OnMouseDown(int x, int y)
	{
		POINT pt = { x, y };
		RECT wr = {
			static_cast<LONG>(mWorldRect.left), static_cast<LONG>(mWorldRect.top),
			static_cast<LONG>(mWorldRect.right), static_cast<LONG>(mWorldRect.bottom)
		};

		if (PtInRect(&wr, pt))
		{
			mState = UIButtonState::UIBS_Pressed;
		}
	}

	void UIButton::OnMouseUp(int x, int y)
	{
		POINT pt = { x, y };
		RECT wr = {
			static_cast<LONG>(mWorldRect.left), static_cast<LONG>(mWorldRect.top),
			static_cast<LONG>(mWorldRect.right), static_cast<LONG>(mWorldRect.bottom)
		};

		if (PtInRect(&wr, pt))
		{
			if (mState == UIButtonState::UIBS_Pressed && mOnClick)
			{
				mOnClick();
			}
			mState = UIButtonState::UIBS_Hover;
		}
		else
		{
			mState = UIButtonState::UIBS_Normal;
		}
	}

	void UIButton::OnMouseMove(int x, int y)
	{
		POINT pt = { x, y };
		RECT wr = {
			static_cast<LONG>(mWorldRect.left), static_cast<LONG>(mWorldRect.top),
			static_cast<LONG>(mWorldRect.right), static_cast<LONG>(mWorldRect.bottom)
		};

		if (PtInRect(&wr, pt))
		{
			if (mState != UIButtonState::UIBS_Pressed)
			{
				mState = UIButtonState::UIBS_Hover;
			}
		}
		else
		{
			if (mState != UIButtonState::UIBS_Pressed)
			{
				mState = UIButtonState::UIBS_Normal;
			}
		}
	}

	void UIButton::_RenderTexture()
	{
		if (mPTexture == nullptr || !mPTexture->IsValid())
		{
			return;
		}

		auto* rt = D2DRd.GetD2DRenderTarget();
		if (rt == nullptr)
		{
			return;
		}

		float sliceWidth = mPTexture->GetWidth() / 3.0f;
		int stateIndex = static_cast<int>(mState);

		rt->DrawBitmap(
			mPTexture->GetD2DBitmap(),
			D2D1::RectF(mWorldRect.left, mWorldRect.top, mWorldRect.right, mWorldRect.bottom),
			1.0f,
			D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
			D2D1::RectF(stateIndex * sliceWidth, 0,
				(stateIndex + 1) * sliceWidth, mPTexture->GetHeight())
		);
	}

	void UIButton::_RenderColor()
	{
		auto* rt = D2DRd.GetD2DRenderTarget();
		if (rt == nullptr)
		{
			return;
		}

		ColorType::COLORF color;
		switch (mState)
		{
		case UIButtonState::UIBS_Hover:
			color = mHoverColor;
			break;
		case UIButtonState::UIBS_Pressed:
			color = mPressedColor;
			break;
		default:
			color = mNormalColor;
			break;
		}

		D2DRd.DrawRoundedRect(
			mWorldRect.left, mWorldRect.top,
			mWorldRect.Width(), mWorldRect.Height(),
			mCornerRadius, mCornerRadius, color
		);
	}
}
