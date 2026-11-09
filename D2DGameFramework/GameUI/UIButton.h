#pragma once
#ifndef __GAME_UI_BUTTON_H__
#define __GAME_UI_BUTTON_H__

#include "UIElement.h"
#include "../D2DCore/Texture.h"
#include "../D2DCore/FontRenderer.h"
#include <functional>

namespace GameUI
{
	
	typedef enum class emUIButtonState
	{
		UIBS_Normal,  
		UIBS_Hover,   
		UIBS_Pressed  
	}UIButtonState;

	
	class UIButton : public UIElement
	{
	public:
		UIButton();
		virtual ~UIButton();

		bool CreateWithTexture(float x, float y, float width, float height,
			D2DCore::Texture* pTexture);

		bool CreateWithColor(float x, float y, float width, float height,
			const ColorType::COLORF& normalColor,
			const ColorType::COLORF& hoverColor,
			const ColorType::COLORF& pressedColor,
			float cornerRadius = 4.0f);

		void SetText(const std::string& text, const std::wstring& fontName, float fontSize);

		virtual void Render() override;

		virtual void Update(float deltaTime) override;

		virtual void OnMouseDown(int x, int y) override;

		virtual void OnMouseUp(int x, int y) override;

		virtual void OnMouseMove(int x, int y) override;

	protected:
		void _RenderTexture();

		void _RenderColor();

	protected:
		bool mUseTextureMode;         
		D2DCore::Texture* mPTexture;  
		UIButtonState mState;         

		ColorType::COLORF mNormalColor;   
		ColorType::COLORF mHoverColor;    
		ColorType::COLORF mPressedColor;  
		float mCornerRadius;              

		bool mHasText;
		std::string mText;
		int mFontId;
		float mFontSize;
	};
}

#endif
