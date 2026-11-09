#include "UITextArea.h"
#include "../D2DCore/D2DRender.h"

namespace GameUI
{
	UITextArea::UITextArea()
	{
		mType = UIElementType::UI_TextArea;
		mIsActive = true;
		mColor = ColorType::COLORF::White();
	}

	UITextArea::~UITextArea()
	{
	}

	void UITextArea::Create(const std::wstring& text, int fontId, const ColorType::COLORF& color,
		float x, float y, float width, float height)
	{
		mText = text;
		mFontId = fontId;
		mColor = color;
		SetPosition(x, y);
		SetSize(width, height);
		mIsActive = true;
	}

	void UITextArea::SetAlignment(DWRITE_TEXT_ALIGNMENT hAlign, DWRITE_PARAGRAPH_ALIGNMENT vAlign)
	{
		mHAlign = hAlign;
		mVAlign = vAlign;
	}

	void UITextArea::Render()
	{
		if (!mIsActive || !IsVisible() || mFontId < 0)
			return;

		auto* rt = D2DRd.GetD2DRenderTarget();
		if (rt == nullptr)
			return;

		D2D1_RECT_F layoutRect = D2D1::RectF(
			mWorldRect.left, mWorldRect.top,
			mWorldRect.right, mWorldRect.bottom);

		ColorType::COLORF finalColor = mColor;
		finalColor.a *= mOpacity;

		FontMgr.DrawText(mText.c_str(), mFontId, layoutRect, finalColor, mHAlign, mVAlign, true);
	}

	void UITextArea::Update(float deltaTime)
	{
	}
}
