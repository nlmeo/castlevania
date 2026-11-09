#include "GameTool.h"
#include "GamePlay/SpriteRender.h"
#include "D2DCore/TextureManager.h"
#include "GamePlay/GameObject.h"
#include "GameCore/AudioLib.h"
#include "GameCore/InputMgr.h"
GamePlay::SpriteRender* GameTool::DrawPicture(const wchar_t* textureKey, MathType::vec2f position, UINT orderInLayer, MathType::vec2f anchor,UINT colorKey, GamePlay::GameObject* obj,GamePlay::GameLayer layer)
{
	if (obj == nullptr)
	{
		obj = CreateGameObject<GamePlay::GameObject>();
	}
	obj->layer() = layer;
	GamePlay::SpriteRender* spriteRender = obj->GetComponent<GamePlay::SpriteRender>();
	if (!spriteRender)
	{
		spriteRender = obj->AddComponent<GamePlay::SpriteRender>();
	}
	D2DCore::Texture* t = TexMgr.LoadTextureWithoutColor(textureKey, colorKey);
	if (!t)
	{
		return nullptr;
	}
	spriteRender->SetTexture(t);
	spriteRender->transform()->SetPosition(position);
	spriteRender->SetAnchor(anchor.x, anchor.y);
	spriteRender->SetOrderInLayer(orderInLayer);
	return spriteRender;
}

GamePlay::SpriteRender* GameTool::DrawSprite(const wchar_t* textureKey, MathType::vec2f position, UINT orderInLayer, MathType::RectF srcRect, MathType::vec2f anchor, UINT colorKey, GamePlay::GameObject* obj,GamePlay::GameLayer layer)
{
	if (obj == nullptr)
	{
		obj = CreateGameObject<GamePlay::GameObject>();
	}
	obj->layer() = layer;
	GamePlay::SpriteRender* spriteRender = obj->GetComponent<GamePlay::SpriteRender>();
	if (!spriteRender)
	{
		spriteRender = obj->AddComponent<GamePlay::SpriteRender>();
	}
	D2DCore::Texture* t = TexMgr.LoadTextureWithoutColor(textureKey, colorKey);
	if (!t)
	{
		return nullptr;
	}
	spriteRender->SetTexture(t);
	spriteRender->transform()->SetPosition(position);
	spriteRender->SetAnchor(anchor.x, anchor.y);
	spriteRender->SetSrcRect(srcRect.left, srcRect.top, srcRect.Width(), srcRect.Height());
	spriteRender->SetOrderInLayer(orderInLayer);
	return spriteRender;
}

GamePlay::Animator* GameTool::PlayAnimation(GamePlay::AnimationClip& clip, std::wstring aniKey, UINT orderInLayer, GamePlay::GameObject* obj)
{
	if (obj == nullptr)
	{
		obj = CreateGameObject<GamePlay::GameObject>();
	}
	GamePlay::Animator* animator = obj->GetComponent<GamePlay::Animator>();
	if (!animator)
	{
		animator = obj->AddComponent<GamePlay::Animator>();
	}
	animator->AddClip(aniKey.c_str(), clip);
	animator->Play(aniKey.c_str());
	animator->SetOrderInLayer(orderInLayer);

	return animator;
}

bool GameTool::SetOrderInLayer(unsigned int i, GamePlay::GameObject& go)
{
	bool res = false;
	GamePlay::SpriteRender* spriteRender = go.GetComponent<GamePlay::SpriteRender>();
	if (spriteRender)
	{
		spriteRender->SetOrderInLayer(i);
		res = true;
	}
	
	GamePlay::Animator* animator = go.GetComponent<GamePlay::Animator>();
	if (animator)
	{
		animator->SetOrderInLayer(i);
		res = true;
	}


	return res;
}

bool GameTool::PlayBGM(const char* path, bool restart)
{
	return AudioLib.PlayBGM(path, restart);
}

bool GameTool::PlayVoice(const char* path)
{
	return AudioLib.PlayVoice(path);
}

bool GameTool::PlaySE(const char* path)
{
	return AudioLib.PlaySE(path);
}

void GameTool::SetBGMVolume(float v)
{
	AudioLib.SetBGMVolume(v);	
}

void GameTool::SetSEVolume(float v)
{
	AudioLib.SetSEVolume(v);	
}

void GameTool::SetVoiceVolume(float v)
{
	AudioLib.SetVoiceVolume(v);
}

void GameTool::StopAllMusic()
{
	AudioLib.StopAllMusic();
}

float GameTool::GetBGMVolume()
{
	return AudioLib.GetBGMVolume();
}

float GameTool::GetVoiceVolume()
{
	return AudioLib.GetVoiceVolume();
}

float GameTool::GetSEVolume()
{
	return AudioLib.GetSEVolume();
}

bool GameTool::KeyDown(unsigned char vk)
{
	return GameInput.KeyDown(vk);
}

bool GameTool::KeyJustDown(unsigned char vk)
{
	return GameInput.KeyJustDown(vk);
}

bool GameTool::KeyUp(unsigned char vk)
{
	return GameInput.KeyUp(vk);
}

bool GameTool::KeyJustUp(unsigned char vk)
{
	return GameInput.KeyJustUp(vk);
}

void GameTool::SwitchTo(const std::string& name)
{
	return SceneMgr.SwitchTo(name);
}


bool GameTool::CollisionDetect(GamePlay::Collider& self, unsigned int mask, GamePlay::Collider** _collider)
{

	for (auto collider : GameObjectMgr.ColliderObjects())
	{
		if (!collider || !collider->gameobject()->Active() || collider->gameobject()->IsDestroyed()) continue;
		 
		if (
			(GamePlay::ToBit(collider->gameobject()->layer()) & mask) == 0 ||
			collider->Type() != GamePlay::COMTYPE::ComType_BoxCollider || 
			collider == &self ||
			collider == nullptr
			)
		{
			continue;
		}
		//目前仅针对BoxCollider
		if (collider->Overlap(&self))
		{
			if (collider)
			{
				*_collider = collider;
			}
			return true;
		}


	}

	return false;
}

bool GameTool::CollisionDetect(GamePlay::Collider& self, unsigned int mask, std::vector<GamePlay::Collider*>& _collider_list)
{
	bool res = false;
	
	if (_collider_list.size() > 0)
	{
		_collider_list.clear();
	}
	
	for (auto collider : GameObjectMgr.ColliderObjects())
	{

		if (
			(GamePlay::ToBit(collider->gameobject()->layer()) & mask) == 0 ||
			collider->Type() != GamePlay::COMTYPE::ComType_BoxCollider ||
			collider == &self || 
			collider == nullptr
			)
		{
			continue;
		}
		//目前仅针对BoxCollider
		if (collider->Overlap(&self))
		{
			_collider_list.push_back(collider);
			res = true;
		}


	}

	return res;
}

void GameTool::ClearAllGameObject(std::vector<GamePlay::GameObject*> ignores)
{
	GameObjectMgr.ClearAllObjects(ignores);
}


