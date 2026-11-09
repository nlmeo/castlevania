#pragma once
#include "./GameCore/Scene.h"
#include "./GamePlay/GameObject.h"
#include "./GamePlay/Animator.h"
class SoundConfigScene : public GameCore::Scene
{
public:
	virtual void OnEnter() override;
	virtual void OnExit() override;
	virtual void Update(float deltaTimeSecond) override;
	virtual void Render(float deltaTimeSecond) override;
	virtual void OnKeyDown(WPARAM key) override;
private:
	int m_cur_opt_index = 0;
	std::vector<std::pair<int, int>> m_options;
	std::vector<int> m_option_num;
	GamePlay::GameObject* m_pointer;
	GamePlay::GameObject* m_a1;
	GamePlay::GameObject* m_a2;
	GamePlay::GameObject* m_a3;
	GamePlay::GameObject* m_a4;
	GamePlay::GameObject* m_a5;
	GamePlay::GameObject* m_a6;
	GamePlay::GameObject* m_camera;
	//音量格子
	GamePlay::SpriteRender* m_bgm_objs[32];
	GamePlay::SpriteRender* m_voice_objs[32];
	GamePlay::SpriteRender* m_se_objs[32];
};