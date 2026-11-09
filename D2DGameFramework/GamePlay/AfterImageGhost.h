#pragma once
#ifndef __GAME_PLAY_AFTER_IMAGE_GHOST_H__
#define __GAME_PLAY_AFTER_IMAGE_GHOST_H__

#include "GameObject.h"

namespace GamePlay
{
	class AfterImageGhost : public GamePlay::GameObject
	{
	public:
		AfterImageGhost();
		virtual void Awake() override;
		virtual void Update(float deltaTime) override;
		void Init(D2DCore::Texture* tex, const MathType::vec2f& anchor,
			const MathType::vec2f& position, float scaleX, float scaleY,
			float lifeTime);
	private:
		D2DCore::Texture* CreateTintedTexture(D2DCore::Texture* src);
		D2DCore::Texture* LoadAndTintFromFile(const std::wstring& filePath);
		float m_lifeTime = 0.3f;
		float m_elapsed = 0;
		static int s_tintId;
	};
}
#endif
