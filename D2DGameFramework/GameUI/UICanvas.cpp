#include "UICanvas.h"
#include "../GameCore/Application.h"
#include "../GamePlay/Camera2d.h"
#include "../D2DCore/D2DRender.h"

namespace GameUI
{
	MathType::vec2f UICanvas::GetRenderScale() const
	{
		auto* camera = GamePlay::Camera::main();
		if (!mScaleWithCamera || !camera || App.GetWidth() <= 0 || App.GetHeight() <= 0 ||
			camera->SizeX() <= 0 || camera->SizeY() <= 0) return { 1, 1 };
		return { App.GetWidth() / camera->SizeX(), App.GetHeight() / camera->SizeY() };
	}

	void UICanvas::ScreenToCanvas(int& x, int& y) const
	{
		const auto scale = GetRenderScale();
		x = static_cast<int>(x / scale.x);
		y = static_cast<int>(y / scale.y);
	}
	UICanvas::UICanvas()
	{
		mType = UIElementType::UI_Canvas;
		mFocusedWindow = nullptr;
	}

	UICanvas::~UICanvas()
	{
		for (auto* win : mSubWindows)
		{
			delete win;
		}
		mSubWindows.clear();
	}

	bool UICanvas::Initialize(int screenWidth, int screenHeight)
	{
		mPosition = MathType::vec2f::zero();
		mSize = MathType::vec2f(static_cast<float>(screenWidth), static_cast<float>(screenHeight));
		mAnchor = MathType::vec2f::zero();

		mWorldRect.left = 0.0f;
		mWorldRect.top = 0.0f;
		mWorldRect.right = static_cast<float>(screenWidth);
		mWorldRect.bottom = static_cast<float>(screenHeight);

		mIsActive = true;
		return true;
	}

	void UICanvas::Resize(int screenWidth, int screenHeight)
	{
		const auto scale = GetRenderScale();
		mSize.x = static_cast<float>(screenWidth) / scale.x;
		mSize.y = static_cast<float>(screenHeight) / scale.y;

		mWorldRect.right = mSize.x;
		mWorldRect.bottom = mSize.y;
	}

	void UICanvas::Render()
	{
		if (!mIsActive)
		{
			return;
		}

		auto* target = D2DRd.GetD2DRenderTarget();
		D2D1_MATRIX_3X2_F previous;
		if (mScaleWithCamera && target)
		{
			target->GetTransform(&previous);
			const auto scale = GetRenderScale();
			target->SetTransform(D2D1::Matrix3x2F::Scale(scale.x, scale.y));
		}

		for (auto& child : mChildren)
		{
			if (child->IsActive() && child->IsVisible())
			{
				child->Render();
			}
		}

		for (auto* window : mSubWindows)
		{
			if (window != mFocusedWindow && window->IsActive() && window->IsVisible())
			{
				window->Render();
			}
		}

		if (mFocusedWindow != nullptr && mFocusedWindow->IsActive() && mFocusedWindow->IsVisible())
		{
			mFocusedWindow->Render();
		}
		if (mScaleWithCamera && target) target->SetTransform(previous);
	}

	void UICanvas::Update(float deltaTime)
	{
		if (!mIsActive)
		{
			return;
		}

		if (mScaleWithCamera) Resize(App.GetWidth(), App.GetHeight());
		_UpdateWorldRectsRecursive(this);

		for (auto* window : mSubWindows)
		{
			if (window->IsActive())
			{
				window->Update(deltaTime);
			}
			else
			{
				if (window == mFocusedWindow)
				{
					mFocusedWindow = nullptr;
				}
			}
		}

		for (auto& child : mChildren)
		{
			if (child->IsActive())
			{
				child->Update(deltaTime);
			}
		}
	}

	void UICanvas::CalcWorldRect()
	{
		mWorldRect = MathType::RectF(0.0f, 0.0f, mSize.x, mSize.y);
	}

	bool UICanvas::AddChild(UIElement* child)
	{
		if (child == nullptr)
		{
			return false;
		}

		if (child->GetType() == UIElementType::UI_Window)
		{
			mFocusedWindow = child;
			child->SetParent(this);
			mSubWindows.push_back(child);
			return true;
		}
		else
		{
			return UIElement::AddChild(child);
		}
	}

	void UICanvas::RemoveAllChildren()
	{
		for (auto* win : mSubWindows)
		{
			delete win;
		}
		mSubWindows.clear();
		mFocusedWindow = nullptr;

		UIElement::RemoveAllChildren();
	}

	bool UICanvas::RemoveChild(UIElement* child)
	{
		if (child == nullptr)
		{
			return false;
		}

		if (child->GetType() == UIElementType::UI_Window)
		{
			for (auto it = mSubWindows.begin(); it != mSubWindows.end(); ++it)
			{
				if (*it == child)
				{
					if (*it == mFocusedWindow)
					{
						mFocusedWindow = nullptr;
					}
					child->SetParent(nullptr);
					mSubWindows.erase(it);
					return true;
				}
			}
			return false;
		}
		else
		{
			return UIElement::RemoveChild(child);
		}
	}

	void UICanvas::OnMouseDown(int x, int y)
	{
		ScreenToCanvas(x, y);
		if (mFocusedWindow != nullptr)
		{
			if (mFocusedWindow->IsActive() && mFocusedWindow->HitsTest(x, y))
			{
				mFocusedWindow->OnMouseDown(x, y);
				return;
			}

			bool hasNewFocus = false;
			for (auto* window : mSubWindows)
			{
				if (window->IsActive() && window->HitsTest(x, y))
				{
					hasNewFocus = true;
					mFocusedWindow = window;
					mFocusedWindow->OnMouseDown(x, y);
					return;
				}
			}

			if (!hasNewFocus)
			{
				mFocusedWindow = nullptr;
			}
		}
		else
		{
			for (auto* window : mSubWindows)
			{
				if (window->IsActive() && window->HitsTest(x, y))
				{
					mFocusedWindow = window;
					mFocusedWindow->OnMouseDown(x, y);
					return;
				}
			}
		}

		for (auto& child : mChildren)
		{
			if (child->IsActive() && child->HitsTest(x, y))
			{
				child->OnMouseDown(x, y);
				return;
			}
		}
	}

	void UICanvas::OnMouseUp(int x, int y)
	{
		ScreenToCanvas(x, y);
		for (auto* window : mSubWindows)
		{
			if (window->IsActive())
			{
				window->OnMouseUp(x, y);
			}
		}

		for (auto& child : mChildren)
		{
			if (child->IsActive())
			{
				child->OnMouseUp(x, y);
			}
		}
	}

	void UICanvas::OnMouseMove(int x, int y)
	{
		ScreenToCanvas(x, y);
		for (auto* window : mSubWindows)
		{
			if (window->IsActive())
			{
				window->OnMouseMove(x, y);
			}
		}

		for (auto& child : mChildren)
		{
			if (child->IsActive())
			{
				child->OnMouseMove(x, y);
			}
		}
	}

	void UICanvas::OnMouseWheel(int delta)
	{
		UIElement* hit = _HitTest(0, 0);

		for (auto* window : mSubWindows)
		{
			if (window->IsActive())
			{
				window->OnMouseWheel(delta);
			}
		}

		for (auto& child : mChildren)
		{
			if (child->IsActive())
			{
				child->OnMouseWheel(delta);
			}
		}
	}

	void UICanvas::OnKeyDown(WPARAM key)
	{
		if (mFocusedWindow != nullptr && mFocusedWindow->IsActive())
		{
			mFocusedWindow->OnKeyDown(key);
			return;
		}

		for (auto& child : mChildren)
		{
			if (child->IsActive())
			{
				child->OnKeyDown(key);
			}
		}
	}

	void UICanvas::OnKeyUp(WPARAM key)
	{
		if (mFocusedWindow != nullptr && mFocusedWindow->IsActive())
		{
			mFocusedWindow->OnKeyUp(key);
			return;
		}

		for (auto& child : mChildren)
		{
			if (child->IsActive())
			{
				child->OnKeyUp(key);
			}
		}
	}

	UIElement* UICanvas::_HitTest(int x, int y)
	{
		for (auto* window : mSubWindows)
		{
			if (window->IsActive() && window->HitsTest(x, y))
			{
				return window;
			}
		}

		for (auto& child : mChildren)
		{
			if (child->IsActive() && child->HitsTest(x, y))
			{
				return child.get();
			}
		}

		return this;
	}

	void UICanvas::_UpdateWorldRectsRecursive(UIElement* element)
	{
		if (element == nullptr)
		{
			return;
		}

		element->CalcWorldRect();

		for (auto& child : element->GetChildren())
		{
			_UpdateWorldRectsRecursive(child.get());
		}

		if (element->GetType() == UIElementType::UI_Canvas)
		{
			for (auto* window : mSubWindows)
			{
				_UpdateWorldRectsRecursive(window);
			}
		}
	}
}
