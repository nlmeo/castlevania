#include "UIWindow.h"
#include "../D2DCore/D2DRender.h"

namespace GameUI
{
	UIWindow::UIWindow()
	{
		mType = UIElementType::UI_Window;
		mCanMove = false;
		mModal = false;
		mIsDragging = false;
		mTitleBarHeight = 30.0f;
		mDragStartX = 0;
		mDragStartY = 0;
		mDragStartPosX = 0.0f;
		mDragStartPosY = 0.0f;
		mPBackGround = nullptr;
		mBackColor = ColorType::COLORF(0.2f, 0.2f, 0.25f, 0.95f);
		mTitleBarColor = ColorType::COLORF(0.15f, 0.15f, 0.2f, 1.0f);
		mBorderColor = ColorType::COLORF(0.3f, 0.3f, 0.35f, 1.0f);
		mUseTexture = false;
	}

	UIWindow::~UIWindow()
	{
	}

	bool UIWindow::Create(float x, float y, float width, float height,
		float titleBarHeight, bool canMove, D2DCore::Texture* pBackGround)
	{
		SetPosition(x, y);
		SetSize(width, height);
		mTitleBarHeight = titleBarHeight;
		mCanMove = canMove;
		mPBackGround = pBackGround;
		mUseTexture = (pBackGround != nullptr);
		mIsActive = true;
		return true;
	}

	void UIWindow::Render()
	{
		if (!mIsActive || !IsVisible())
		{
			return;
		}

		_RenderBackground();
		_RenderTitleBar();
		_RenderChildren();
	}

	void UIWindow::Update(float deltaTime)
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

	bool UIWindow::AddChild(UIElement* child)
	{
		return UIElement::AddChild(child);
	}

	bool UIWindow::RemoveChild(UIElement* child)
	{
		return UIElement::RemoveChild(child);
	}

	void UIWindow::OnMouseDown(int x, int y)
	{
		POINT pt = { x, y };
		RECT titleRect = {
			static_cast<LONG>(mWorldRect.left),
			static_cast<LONG>(mWorldRect.top),
			static_cast<LONG>(mWorldRect.right),
			static_cast<LONG>(mWorldRect.top + mTitleBarHeight)
		};

		if (PtInRect(&titleRect, pt) && mCanMove)
		{
			mIsDragging = true;
			mDragStartX = x;
			mDragStartY = y;
			mDragStartPosX = mPosition.x;
			mDragStartPosY = mPosition.y;
			return;
		}

		_DispatchToChildren(&UIElement::OnMouseDown, x, y);
	}

	void UIWindow::OnMouseUp(int x, int y)
	{
		mIsDragging = false;
		_DispatchToChildren(&UIElement::OnMouseUp, x, y);
	}

	void UIWindow::OnMouseMove(int x, int y)
	{
		if (mIsDragging)
		{
			float dx = static_cast<float>(x - mDragStartX);
			float dy = static_cast<float>(y - mDragStartY);
			mPosition.x = mDragStartPosX + dx;
			mPosition.y = mDragStartPosY + dy;
			return;
		}

		_DispatchToChildren(&UIElement::OnMouseMove, x, y);
	}

	void UIWindow::OnMouseWheel(int delta)
	{
		_DispatchWheelToChildren(delta);
	}

	void UIWindow::SetCanMove(bool canMove)
	{
		mCanMove = canMove;
	}

	void UIWindow::SetModal(bool modal)
	{
		mModal = modal;
	}

	bool UIWindow::IsModal() const
	{
		return mModal;
	}

	void UIWindow::SetBackColor(const ColorType::COLORF& color)
	{
		mBackColor = color;
	}

	void UIWindow::SetTitleBarColor(const ColorType::COLORF& color)
	{
		mTitleBarColor = color;
	}

	void UIWindow::SetBorderColor(const ColorType::COLORF& color)
	{
		mBorderColor = color;
	}

	void UIWindow::SetTitleBarHeight(float height)
	{
		mTitleBarHeight = height;
	}

	float UIWindow::GetTitleBarHeight() const
	{
		return mTitleBarHeight;
	}

	void UIWindow::_RenderBackground()
	{
		auto* rt = D2DRd.GetD2DRenderTarget();
		if (rt == nullptr)
		{
			return;
		}

		if (mUseTexture && mPBackGround != nullptr && mPBackGround->IsValid())
		{
			rt->DrawBitmap(
				mPBackGround->GetD2DBitmap(),
				D2D1::RectF(mWorldRect.left, mWorldRect.top, mWorldRect.right, mWorldRect.bottom),
				1.0f,
				D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
				D2D1::RectF(0, 0, mPBackGround->GetWidth(), mPBackGround->GetHeight())
			);
		}
		else
		{
			D2DRd.SetBrushColor(mBackColor);
			rt->FillRectangle(
				D2D1::RectF(mWorldRect.left, mWorldRect.top, mWorldRect.right, mWorldRect.bottom),
				D2DRd.GetBrush()
			);

			D2DRd.SetBrushColor(mBorderColor);
			rt->DrawRectangle(
				D2D1::RectF(mWorldRect.left, mWorldRect.top, mWorldRect.right, mWorldRect.bottom),
				D2DRd.GetBrush(), 1.0f
			);
		}
	}

	void UIWindow::_RenderTitleBar()
	{
		if (mUseTexture)
		{
			return;
		}

		auto* rt = D2DRd.GetD2DRenderTarget();
		if (rt == nullptr)
		{
			return;
		}

		D2DRd.SetBrushColor(mTitleBarColor);
		rt->FillRectangle(
			D2D1::RectF(mWorldRect.left, mWorldRect.top,
				mWorldRect.right, mWorldRect.top + mTitleBarHeight),
			D2DRd.GetBrush()
		);
	}

	void UIWindow::_RenderChildren()
	{
		for (auto& child : mChildren)
		{
			if (child->IsActive() && child->IsVisible())
			{
				child->Render();
			}
		}
	}

	void UIWindow::_DispatchToChildren(void (UIElement::* event)(int, int), int x, int y)
	{
		for (auto& child : mChildren)
		{
			if (child->IsActive() && child->HitsTest(x, y))
			{
				(child.get()->*event)(x, y);
				return;
			}
		}
	}

	void UIWindow::_DispatchWheelToChildren(int delta)
	{
		for (auto& child : mChildren)
		{
			if (child->IsActive())
			{
				child->OnMouseWheel(delta);
			}
		}
	}
}
