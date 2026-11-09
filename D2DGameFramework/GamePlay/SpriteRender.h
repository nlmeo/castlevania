#pragma once
#ifndef __GAME_PLAY_FRAME_SPRITE_RENDER_H__
#define __GAME_PLAY_FRAME_SPRITE_RENDER_H__

#include "./Component.h"
#include "../D2DCore/Texture.h"
#include "../TypeDefine/Matrix3x3.h"
#include "../TypeDefine/Geometric.h"
#include "../TypeDefine/Color.h"

namespace GamePlay
{
	class SpriteRender : public RenderComponent
	{
	public:
		
		SpriteRender(class GameObject* pObj) : RenderComponent(pObj)
		{
			mType = COMTYPE::ComType_SpriteRender;

			mTexture = nullptr;
			mAnchor = { 0.5f, 0.5f };
			mSrcRect = {};
			mOpacity = 1.0f;
			mOrderInLayer = 0;
			mTintColor = { 1.0f, 1.0f, 1.0f, 0.0f }; 
			mTransformMatrix.Identity();

			mpTintMaskBrush = nullptr;
			mpTintLayer = nullptr;
		}

		~SpriteRender();

	
		void SetTexture(class D2DCore::Texture* tex);

		void InvalidateTintResources();

		void SetSrcRect(float x, float y, float w, float h)
		{
			mSrcRect = { x, y, x + w, y + h };
			mDestRect = { 0, 0, w, h };
		}


		void SetAnchor(float x, float y) { mAnchor = { x, y }; }

	
		void SetOpcity(float op) { mOpacity = MathType::clamp(op); }

	
		void SetOrderInLayer(int layerorder) { mOrderInLayer = layerorder; }

	
		void SetTintColor(float r, float g, float b, float a) { mTintColor = { r, g, b, a }; }

		void SetTintColor(const ColorType::COLORF& color) { mTintColor = color; }

		const ColorType::COLORF& GetTintColor() const { return mTintColor; }

		class D2DCore::Texture* texture() const { return mTexture; }

		MathType::vec2f Anchor() const { return mAnchor; }

		const MathType::RectF& SrcRect() const { return mSrcRect; }

		float Opacity() const { return mOpacity; }

		virtual void CaclRenderMatrix();

	
		void Render(const MathType::Mat3x3f& vpv) override;

		const int& OrderInLayer() const override { return mOrderInLayer; }

		const MathType::RectF& GetRenderRect() const override;
	protected:
		class D2DCore::Texture* mTexture;
		MathType::vec2f mAnchor;
		MathType::RectF mSrcRect;
		MathType::RectF mDestRect{};
		MathType::RectF mRenderRect{};

		float mOpacity;
		int mOrderInLayer;
		ColorType::COLORF mTintColor;

		ID2D1BitmapBrush* mpTintMaskBrush; 
		ID2D1Layer* mpTintLayer;           

		MathType::Mat3x3f mTransformMatrix;
	};
}
#endif
