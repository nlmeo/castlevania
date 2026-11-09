#include "AfterImageTrail.h"
#include "AfterImageGhost.h"
#include "../Soma.h"
#include "../GameTool.h"
#include "Animator.h"

namespace GamePlay
{
	AfterImageTrail::AfterImageTrail(GamePlay::GameObject* pObj)
		: Component(pObj)
	{
		mType = COMTYPE::ComType_AfterImageTrail;
		m_owner = pObj;
	}

	void AfterImageTrail::Update(float deltaTime)
	{
		if (!m_owner || !m_owner->Active()) return;

		m_frameCounter++;
		if (m_frameCounter >= m_sampleInterval)
		{
			m_frameCounter = 0;
			if(m_isCapture)
				CaptureSnapshot();
		}
	}

	void AfterImageTrail::CaptureSnapshot()
	{
		if (!m_owner || !m_owner->Active()) return;

		D2DCore::Texture* texture = nullptr;
		MathType::vec2f anchor;
		do 
		{
			//优先通过spriteRender获取纹理
			auto* spriteRender = m_owner->GetComponent<GamePlay::SpriteRender>();
			if (spriteRender && spriteRender->texture())
			{
				texture = spriteRender->texture();
				anchor = spriteRender->Anchor();
				break;
			}

			auto* anim = m_owner->GetComponent<GamePlay::Animator>();
			if (anim)
			{
				const auto& frame = anim->GetCurrentFrame();
				if (frame.texture)
				{
					texture = frame.texture;
					anchor = frame.anchor;
					break;
				}
			}

			

		} while (0);


		

		AfterImageGhost* ghost = GT::CreateGameObject<AfterImageGhost>();

		MathType::vec2f pos = m_owner->transform().position();
		float sx = m_owner->transform().localScale().x;
		float sy = m_owner->transform().localScale().y;

		ghost->Init(texture, anchor, pos, sx, sy, m_lifeTime);
	}
}
