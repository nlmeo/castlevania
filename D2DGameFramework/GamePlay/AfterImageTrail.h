#pragma once
#ifndef __GAME_PLAY_AFTER_IMAGE_TRAIL_H__
#define __GAME_PLAY_AFTER_IMAGE_TRAIL_H__

#include "Component.h"

namespace GamePlay
{
	class AfterImageTrail : public GamePlay::Component
	{
	public:
		AfterImageTrail(GamePlay::GameObject* pObj);
		void Update(float deltaTime);
		void SetSampleInterval(int frames) { m_sampleInterval = frames; }
		void SetLifeTime(float seconds) { m_lifeTime = seconds; }
		void SetCapState(bool isCap) { m_isCapture = isCap; }
	private:
		void CaptureSnapshot();
		GamePlay::GameObject* m_owner = nullptr;
		int m_sampleInterval = 5;
		int m_frameCounter = 0;
		float m_lifeTime = 0.3f;
		bool m_isCapture = false;
	};
}
#endif
