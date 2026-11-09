#pragma once
#ifndef __GAME_UI_CANVAS_H__
#define __GAME_UI_CANVAS_H__

#include "UIElement.h"

namespace GameUI
{
	
	class UICanvas : public UIElement
	{
	public:
		
		UICanvas();

		
		virtual ~UICanvas();

		
		bool Initialize(int screenWidth, int screenHeight);

		
		void Resize(int screenWidth, int screenHeight);
		void SetScaleWithCamera(bool enabled = true) { mScaleWithCamera = enabled; }

		
		virtual void Render() override;

		
		virtual void Update(float deltaTime) override;

		
		virtual void CalcWorldRect() override;

		virtual bool AddChild(UIElement* child) override;

		virtual bool RemoveChild(UIElement* child) override;

		virtual void RemoveAllChildren() override;

		virtual void OnMouseDown(int x, int y) override;

		virtual void OnMouseUp(int x, int y) override;

		virtual void OnMouseMove(int x, int y) override;

		virtual void OnMouseWheel(int delta) override;

		virtual void OnKeyDown(WPARAM key) override;

		virtual void OnKeyUp(WPARAM key) override;


		unsigned int GetUIElementCount() const { return mUIElementCount; }
	protected:
		
		UIElement* _HitTest(int x, int y);

		
		void _UpdateWorldRectsRecursive(UIElement* element);

	protected:
		MathType::vec2f GetRenderScale() const;
		void ScreenToCanvas(int& x, int& y) const;
		bool mScaleWithCamera = false;
		std::vector<UIElement*> mSubWindows;  
		UIElement* mFocusedWindow;            
	};
}
#define SAFE_REMOVECHILDE(p,q)  if(p != nullptr && q != nullptr) {p->RemoveChild(q);  q = nullptr;}
#endif
