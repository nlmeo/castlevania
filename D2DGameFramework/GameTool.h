#pragma once
#ifndef __MY_GAME_OUT_PUT_H__
#define __MY_GAME_OUT_PUT_H__
#include <string>
#include "GamePlay/GameObject.h"
#include "TypeDefine/Vector2.h"
#include "TypeDefine/Geometric.h"
#include "GameCore/SceneManager.h"
#include "ResHeader.h"
#include "Obstacle.h"
#include "Platform.h"
#include "Slop.h"
#include "EdgePlatform.h"
#include "GamePlay/Debug.h"

//一些工具类的封装，整合一些常用的绘制函数，方便在场景中调用
typedef class GameTool
{
public:
	//绘制相关
	template<typename T>
	static T* CreateGameObject(std::string name = "");
	static void ClearAllGameObject(std::vector<GamePlay::GameObject*> ignores = {});
	static GamePlay::SpriteRender* DrawPicture(const wchar_t* textureKey, MathType::vec2f position, UINT orderInLayer,MathType::vec2f anchor = {0,0}, UINT colorKey = 0x00FF00, GamePlay::GameObject* obj = nullptr,GamePlay::GameLayer layer = GamePlay::GameLayer::Layer_0);
	static GamePlay::SpriteRender* DrawSprite(const wchar_t* textureKey, MathType::vec2f position, UINT orderInLayer, MathType::RectF srcRect,MathType::vec2f anchor = {0,0}, UINT colorKey = 0x00FF00, GamePlay::GameObject* obj = nullptr, GamePlay::GameLayer layer = GamePlay::GameLayer::Layer_0);
	static GamePlay::Animator* PlayAnimation(GamePlay::AnimationClip& clip, std::wstring aniKey, UINT orderInLayer, GamePlay::GameObject* obj = nullptr);
	static bool SetOrderInLayer(unsigned int i, GamePlay::GameObject& go);

	//对象相关
	//posx posy width height
	template<typename T>
	static std::vector<T*> InitObstacles(const std::vector<std::tuple<float,float,float,float>> tasks);
	//音乐相关
	static bool PlayBGM(const char* path, bool restart = false);//播放背景音乐
	static bool PlayVoice(const char* path);//播放人声
	static bool PlaySE(const char* path);//播放音效
	static void SetBGMVolume(float v);//设置背景音乐音量
	static void SetSEVolume(float v);//设置音效音量
	static void SetVoiceVolume(float v);//设置人声音量
	static void StopAllMusic();
	static float GetBGMVolume();
	static float GetVoiceVolume();
	static float GetSEVolume();
	//输入相关
	static bool KeyDown(unsigned char vk);
	static bool KeyJustDown(unsigned char vk);
	static bool KeyUp(unsigned char vk);
	static bool KeyJustUp(unsigned char vk);
	//场景相关
	static void SwitchTo(const std::string& name);
	//碰撞相关
	static bool CollisionDetect(GamePlay::Collider& self, unsigned int mask, GamePlay::Collider** _collider = nullptr);
	static bool CollisionDetect(GamePlay::Collider& self, unsigned int mask, std::vector<GamePlay::Collider*>& _collider_list);
}GT,*LPGT;


#endif // !__MY_GAME_OUT_PUT_H__


template<typename T>
inline T* GameTool::CreateGameObject(std::string name)
{
	return GamePlay::GameObject::CreateGameObject<T>(name);
}

template<typename T>
inline std::vector<T*> GameTool::InitObstacles(const std::vector<std::tuple<float, float, float, float>> tasks)
{
	std::vector<T*> tv;
	for (int i = 0; i < tasks.size(); ++i)
	{
		T* t = GamePlay::GameObject::CreateGameObject<T>("");
		float posx = std::get<0>(tasks[i]);
		float posy = std::get<1>(tasks[i]);
		float width = std::get<2>(tasks[i]);
		float height = std::get<3>(tasks[i]);
		t->transform().SetPosition({ posx,posy });
		t->GetComponent<GamePlay::BoxCollider>()->SetColliderSize(width, height);
		tv.push_back(t);
	}

	return tv;
}
