#include "UINumber.h"
#include "../D2DCore/D2DRender.h"

namespace GameUI
{
	UINumber::UINumber()
	{
		mType = UIElementType::UI_Number;
		mIsActive = true;
	}

	UINumber::~UINumber()
	{
	}

	bool UINumber::Create(float x, float y, float digitWidth, float digitHeight,
		D2DCore::Texture* pTexture, float spacing)
	{
		if (pTexture == nullptr)
		{
			return false;
		}

		mUseIndividual = false;
		SetPosition(x, y);
		SetSize(digitWidth, digitHeight);
		mDigitWidth = digitWidth;
		mDigitHeight = digitHeight;
		mPTexture = pTexture;
		mSpacing = spacing;
		mIsActive = true;
		return true;
	}

	bool UINumber::CreateWithTextures(float x, float y, float digitWidth, float digitHeight,
		D2DCore::Texture* textures[10], float spacing)
	{
		if (textures == nullptr)
		{
			return false;
		}

		// 确保 10 张纹理都不为空
		for (int i = 0; i < 10; ++i)
		{
			if (textures[i] == nullptr)
			{
				return false;
			}
			mDigitTextures[i] = textures[i];
		}

		mUseIndividual = true;
		SetPosition(x, y);
		SetSize(digitWidth, digitHeight);
		mDigitWidth = digitWidth;
		mDigitHeight = digitHeight;
		mPTexture = nullptr;
		mSpacing = spacing;
		mIsActive = true;
		return true;
	}

	void UINumber::SetNumber(int number)
	{
		mNumber = abs(number);
	}

	int UINumber::_GetDigitAt(int index, int displayCount) const
	{
		// index: 0=最高位, displayCount-1=个位
		int divisor = 1;
		for (int j = 0; j < displayCount - 1 - index; ++j)
			divisor *= 10;
		return (mNumber / divisor) % 10;
	}

	void UINumber::_DrawDigit(int digit, float left, float top, float right, float bottom)
	{
		auto* rt = D2DRd.GetD2DRenderTarget();
		if (rt == nullptr)
		{
			return;
		}

		if (mUseIndividual)
		{
			// 独立纹理模式：直接从 mDigitTextures[digit] 绘制
			D2DCore::Texture* tex = mDigitTextures[digit];
			if (tex == nullptr || !tex->IsValid())
			{
				return;
			}

			rt->DrawBitmap(
				tex->GetD2DBitmap(),
				D2D1::RectF(left, top, right, bottom),
				mOpacity,
				D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
				D2D1::RectF(0, 0, tex->GetWidth(), tex->GetHeight())
			);
		}
		else
		{
			// 纹理条模式：从 mPTexture 截取对应段
			if (mPTexture == nullptr || !mPTexture->IsValid())
			{
				return;
			}

			float texSlice = mPTexture->GetWidth() / 10.0f;

			rt->DrawBitmap(
				mPTexture->GetD2DBitmap(),
				D2D1::RectF(left, top, right, bottom),
				mOpacity,
				D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
				D2D1::RectF(digit * texSlice, 0,
					(digit + 1) * texSlice, mPTexture->GetHeight())
			);
		}
	}

	void UINumber::Render()
	{
		if (!mIsActive || !IsVisible())
		{
			return;
		}

		// 两种模式下都确保至少有一种纹理可用
		if (!mUseIndividual)
		{
			if (mPTexture == nullptr || !mPTexture->IsValid())
				return;
		}

		auto* rt = D2DRd.GetD2DRenderTarget();
		if (rt == nullptr)
		{
			return;
		}

		// ----- 计算显示位数 -----
		int displayCount;
		if (mDigitCount > 0)
		{
			displayCount = mDigitCount;                          // 固定位数
		}
		else
		{
			displayCount = (mNumber == 0) ? 1 : 0;              // 自动：0 → 1 位
			int temp = mNumber;
			while (temp > 0)
			{
				++displayCount;
				temp /= 10;
			}
		}

		// ----- 逐位绘制 -----
		float curX = mWorldRect.left;

		for (int i = 0; i < displayCount; ++i)
		{
			int digit = _GetDigitAt(i, displayCount);

			_DrawDigit(digit,
				curX, mWorldRect.top,
				curX + mDigitWidth, mWorldRect.bottom);

			curX += mDigitWidth + mSpacing;
		}
	}

	void UINumber::Update(float deltaTime)
	{
	}
}
