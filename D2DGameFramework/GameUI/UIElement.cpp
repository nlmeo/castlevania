#include "UIElement.h"

namespace GameUI
{
	unsigned int UIElement::msNextUID = 1;
	unsigned int UIElement::mUIElementCount = 0;
	UIElement::UIElement()
	{
		mPosition = MathType::vec2f::zero();
		mSize = MathType::vec2f::zero();
		mAnchor = MathType::vec2f::zero();
		mWorldRect = MathType::RectF();
		mParent = nullptr;
		mIsActive = false;
		mType = UIElementType::UI_Base;
		mUID = msNextUID++;
		mOnClick = nullptr;
		++mUIElementCount;
	}

	UIElement::~UIElement()
	{
		--mUIElementCount;
		mChildren.clear();
	}

	void UIElement::CalcWorldRect()
	{
		if (mParent != nullptr)
		{
			MathType::RectF parentRect = mParent->GetWorldRect();

			float worldX = parentRect.left + mPosition.x - mSize.x * mAnchor.x;
			float worldY = parentRect.top + mPosition.y - mSize.y * mAnchor.y;

			mWorldRect.left = worldX;
			mWorldRect.top = worldY;
			mWorldRect.right = worldX + mSize.x;
			mWorldRect.bottom = worldY + mSize.y;

			MathType::RectF clipped;
			if (mWorldRect.Overlap(parentRect, &clipped))
			{
				mWorldRect = clipped;
			}
		}
		else
		{
			mWorldRect.left = mPosition.x - mSize.x * mAnchor.x;
			mWorldRect.top = mPosition.y - mSize.y * mAnchor.y;
			mWorldRect.right = mWorldRect.left + mSize.x;
			mWorldRect.bottom = mWorldRect.top + mSize.y;
		}
	}

	MathType::RectF UIElement::GetWorldRect() const
	{
		return mWorldRect;
	}

	MathType::RectF UIElement::GetSelfRect() const
	{
		float x = mPosition.x - mSize.x * mAnchor.x;
		float y = mPosition.y - mSize.y * mAnchor.y;
		return MathType::RectF(x, y, mSize.x, mSize.y);
	}

	bool UIElement::IsActive() const
	{
		return mIsActive;
	}

	void UIElement::SetActive(bool active)
	{
		mIsActive = active;
	}

	bool UIElement::IsVisible()
	{
		if (!mIsActive)
		{
			return false;
		}

		if (mParent != nullptr)
		{
			if (!mParent->IsVisible())
			{
				return false;
			}

			if (mWorldRect.Width() <= 0.0f || mWorldRect.Height() <= 0.0f)
			{
				return false;
			}
		}

		return true;
	}

	UIElementType UIElement::GetType() const
	{
		return mType;
	}

	unsigned int UIElement::GetUID() const
	{
		return mUID;
	}

	void UIElement::SetPosition(const MathType::vec2f& pos)
	{
		mPosition = pos;
	}

	void UIElement::SetPosition(float x, float y)
	{
		mPosition.x = x;
		mPosition.y = y;
	}

	void UIElement::SetSize(const MathType::vec2f& size)
	{
		mSize = size;
	}

	void UIElement::SetSize(float w, float h)
	{
		mSize.x = w;
		mSize.y = h;
	}

	void UIElement::SetAnchor(const MathType::vec2f& anchor)
	{
		mAnchor = anchor;
	}

	const MathType::vec2f& UIElement::GetPosition() const
	{
		return mPosition;
	}

	const MathType::vec2f& UIElement::GetSize() const
	{
		return mSize;
	}

	bool UIElement::AddChild(UIElement* child)
	{
		if (child == nullptr)
		{
			return false;
		}

		child->SetParent(this);
		mChildren.push_back(std::unique_ptr<UIElement>(child));
		return true;
	}

	void UIElement::RemoveAllChildren()
	{
		mChildren.clear();
	}

	bool UIElement::RemoveChild(UIElement* child)
	{
		if (child == nullptr)
		{
			return false;
		}

		for (auto it = mChildren.begin(); it != mChildren.end(); ++it)
		{
			if (it->get() == child)
			{
				child->SetParent(nullptr);
				mChildren.erase(it);
				return true;
			}
		}

		return false;
	}

	UIElement* UIElement::GetChild(unsigned int uid)
	{
		for (auto& child : mChildren)
		{
			if (child->GetUID() == uid)
			{
				return child.get();
			}

			UIElement* found = child->GetChild(uid);
			if (found != nullptr)
			{
				return found;
			}
		}

		return nullptr;
	}

	void UIElement::OnMouseDown(int x, int y)
	{
	}

	void UIElement::OnMouseUp(int x, int y)
	{
		if (HitsTest(x, y) && mOnClick)
		{
			mOnClick();
		}
	}

	void UIElement::OnMouseMove(int x, int y)
	{
	}

	void UIElement::OnMouseWheel(int delta)
	{
	}

	void UIElement::OnKeyDown(WPARAM key)
	{
	}

	void UIElement::OnKeyUp(WPARAM key)
	{
	}

	void UIElement::SetParent(UIElement* parent)
	{
		mParent = parent;
	}

	UIElement* UIElement::GetParent() const
	{
		return mParent;
	}

	const std::vector<std::unique_ptr<UIElement>>& UIElement::GetChildren() const
	{
		return mChildren;
	}

	bool UIElement::HitsTest(int x, int y) const
	{
		return mWorldRect.Contains(static_cast<float>(x), static_cast<float>(y));
	}

	void UIElement::SetOnClick(std::function<void()> callback)
	{
		mOnClick = callback;
	}
}
