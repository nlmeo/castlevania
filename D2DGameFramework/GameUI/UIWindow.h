#pragma once
#ifndef __GAME_UI_WINDOW_H__
#define __GAME_UI_WINDOW_H__

#include "UIElement.h"
#include "../D2DCore/Texture.h"
#include "../TypeDefine/Color.h"

namespace GameUI
{
	class UIWindow : public UIElement
	{
	public:
		UIWindow();

		virtual ~UIWindow();

		bool Create(float x, float y, float width, float height,
			float titleBarHeight = 30.0f, bool canMove = true,
			D2DCore::Texture* pBackGround = nullptr);

		virtual void Render() override;

		virtual void Update(float deltaTime) override;

		virtual bool AddChild(UIElement* child) override;

		virtual bool RemoveChild(UIElement* child) override;

		virtual void OnMouseDown(int x, int y) override;

		virtual void OnMouseUp(int x, int y) override;

		virtual void OnMouseMove(int x, int y) override;

		virtual void OnMouseWheel(int delta) override;

		void SetCanMove(bool canMove);

		void SetModal(bool modal);

		bool IsModal() const;

		void SetBackColor(const ColorType::COLORF& color);

		void SetTitleBarColor(const ColorType::COLORF& color);

		void SetBorderColor(const ColorType::COLORF& color);

		void SetTitleBarHeight(float height);

		float GetTitleBarHeight() const;

	protected:
		void _RenderBackground();

		void _RenderTitleBar();

		void _RenderChildren();

		void _DispatchToChildren(void (UIElement::*event)(int, int), int x, int y);

		void _DispatchWheelToChildren(int delta);

	protected:
		bool mCanMove;
		bool mModal;
		bool mIsDragging;
		float mTitleBarHeight;
		int mDragStartX;
		int mDragStartY;
		float mDragStartPosX;
		float mDragStartPosY;

		D2DCore::Texture* mPBackGround;
		ColorType::COLORF mBackColor;
		ColorType::COLORF mTitleBarColor;
		ColorType::COLORF mBorderColor;

		bool mUseTexture;
	};
}

#endif
