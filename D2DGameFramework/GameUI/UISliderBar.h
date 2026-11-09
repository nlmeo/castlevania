#pragma once
#ifndef __GAME_UI_SLIDERBAR_H__
#define __GAME_UI_SLIDERBAR_H__

#include "UIElement.h"
#include "../D2DCore/Texture.h"
#include "../TypeDefine/Color.h"
#include <functional>

namespace GameUI
{
	
	class UISliderBar : public UIElement
	{
	public:
		UISliderBar();

		virtual ~UISliderBar();

		
		bool CreateWithTexture(float x, float y, float width, float height,
			D2DCore::Texture* pBackGround, D2DCore::Texture* pCover,
			D2DCore::Texture* pHandle, float value = 0.0f);

		
		bool CreateWithColor(float x, float y, float width, float height,
			float handleWidth,
			const ColorType::COLORF& backColor,
			const ColorType::COLORF& fillColor,
			const ColorType::COLORF& handleColor,
			float value = 0.0f);

		float GetValue() const;

		
		void SetValue(float value);

		
		void SetOnValueChanged(std::function<void(float)> callback);

		virtual void Render() override;

		virtual void Update(float deltaTime) override;

		virtual void OnMouseDown(int x, int y) override;

		virtual void OnMouseUp(int x, int y) override;

		virtual void OnMouseMove(int x, int y) override;

	protected:
		float _CalcValueFromMouse(int mouseX);

	protected:
		bool mUseTextureMode;

		D2DCore::Texture* mPBackGround;
		D2DCore::Texture* mPCover;
		D2DCore::Texture* mPHandle;

		ColorType::COLORF mBackColor;
		ColorType::COLORF mFillColor;
		ColorType::COLORF mHandleColor;

		float mValue;
		float mHandleWidth;
		bool mCanMove;

		MathType::RectF mBackRect;
		MathType::RectF mCoverRect;
		MathType::RectF mHandleRect;

		std::function<void(float)> mOnValueChanged;  
	};
}

#endif
