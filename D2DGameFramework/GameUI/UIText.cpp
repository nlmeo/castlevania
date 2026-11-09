#include "UIText.h"
#include "../D2DCore/D2DRender.h"

namespace GameUI
{
	UIText::UIText()
	{
		mType = UIElementType::UI_Text;
		mIsActive = true;
		mColor = ColorType::COLORF::White();
	}

	UIText::~UIText()
	{
	}

	void UIText::Create(const std::wstring& text, int fontId, const ColorType::COLORF& color,
		float x, float y)
	{
		mText = text;
		mFontId = fontId;
		mColor = color;
		SetPosition(x, y);
		mIsActive = true;

		// 自动根据文字内容测量尺寸
		D2D1_SIZE_F size = FontMgr.MeasureText(mText.c_str(), mFontId);
		SetSize(size.width, size.height);
	}

	void UIText::SetAlignment(DWRITE_TEXT_ALIGNMENT hAlign, DWRITE_PARAGRAPH_ALIGNMENT vAlign)
	{
		mHAlign = hAlign;
		mVAlign = vAlign;
	}

	void UIText::Render()
	{
		if (!mIsActive || !IsVisible() || mFontId < 0)
		{
			return;
		}

		auto* rt = D2DRd.GetD2DRenderTarget();
		if (rt == nullptr)
		{
			return;
		}

		D2D1_RECT_F layoutRect = D2D1::RectF(
			mWorldRect.left, mWorldRect.top,
			mWorldRect.right, mWorldRect.bottom);

		ColorType::COLORF finalColor = mColor;
		finalColor.a *= mOpacity;

		FontMgr.DrawText(mText.c_str(), mFontId, layoutRect, finalColor, mHAlign, mVAlign);
	}

	void UIText::Update(float deltaTime)
	{
	}
}
