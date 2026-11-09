#include "UIScrollView.h"
#include "../D2DCore/D2DRender.h"

namespace GameUI
{
	UIScrollView::UIScrollView()
	{
		mType = UIElementType::UI_ScrollView;
		mContentWidth = 0.0f;
		mContentHeight = 0.0f;
		mScrollOffset = 0.0f;
		mScrollSpeed = 20.0f;
		mMaxScrollOffset = 0.0f;
	}

	UIScrollView::~UIScrollView()
	{
	}

	bool UIScrollView::Create(float x, float y, float width, float height,
		float contentWidth, float contentHeight, float scrollSpeed)
	{
		SetPosition(x, y);
		SetSize(width, height);
		mContentWidth = contentWidth;
		mContentHeight = contentHeight;
		mScrollSpeed = scrollSpeed;
		mMaxScrollOffset = max(0.0f, contentHeight - height);
		mScrollOffset = 0.0f;
		mIsActive = true;
		return true;
	}

	void UIScrollView::Render()
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

		rt->PushAxisAlignedClip(
			D2D1::RectF(mWorldRect.left, mWorldRect.top, mWorldRect.right, mWorldRect.bottom),
			D2D1_ANTIALIAS_MODE_ALIASED
		);

		for (auto& child : mChildren)
		{
			if (child->IsActive())
			{
				float oldPosY = child->GetPosition().y;
				child->SetPosition(child->GetPosition().x, oldPosY - mScrollOffset);
				child->CalcWorldRect();

				if (child->IsVisible())
				{
					child->Render();
				}

				child->SetPosition(child->GetPosition().x, oldPosY);
			}
		}

		rt->PopAxisAlignedClip();
	}

	void UIScrollView::Update(float deltaTime)
	{
		if (!mIsActive)
		{
			return;
		}

		for (auto& child : mChildren)
		{
			if (child->IsActive())
			{
				child->Update(deltaTime);
			}
		}
	}

	void UIScrollView::OnMouseDown(int x, int y)
	{
		POINT pt = { x, y };
		RECT wr = {
			static_cast<LONG>(mWorldRect.left), static_cast<LONG>(mWorldRect.top),
			static_cast<LONG>(mWorldRect.right), static_cast<LONG>(mWorldRect.bottom)
		};

		if (PtInRect(&wr, pt))
		{
			for (auto& child : mChildren)
			{
				if (child->IsActive() && child->HitsTest(x, static_cast<int>(y + mScrollOffset)))
				{
					child->OnMouseDown(x, static_cast<int>(y + mScrollOffset));
					return;
				}
			}
		}
	}

	void UIScrollView::OnMouseUp(int x, int y)
	{
		for (auto& child : mChildren)
		{
			if (child->IsActive())
			{
				child->OnMouseUp(x, static_cast<int>(y + mScrollOffset));
			}
		}
	}

	void UIScrollView::OnMouseMove(int x, int y)
	{
		for (auto& child : mChildren)
		{
			if (child->IsActive())
			{
				child->OnMouseMove(x, static_cast<int>(y + mScrollOffset));
			}
		}
	}

	void UIScrollView::OnMouseWheel(int delta)
	{
		mScrollOffset -= static_cast<float>(delta) / 120.0f * mScrollSpeed;
		_ClampScrollOffset();
	}

	float UIScrollView::GetScrollOffset() const
	{
		return mScrollOffset;
	}

	void UIScrollView::SetScrollOffset(float offset)
	{
		mScrollOffset = offset;
		_ClampScrollOffset();
	}

	void UIScrollView::_ClampScrollOffset()
	{
		if (mScrollOffset < 0.0f)
		{
			mScrollOffset = 0.0f;
		}
		if (mScrollOffset > mMaxScrollOffset)
		{
			mScrollOffset = mMaxScrollOffset;
		}
	}
}
