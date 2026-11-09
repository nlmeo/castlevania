#pragma once
#include "./GamePlay/GameObject.h"
#include <string>

// 特效 key 定义
#define EFFECT_DUST_01      L"e000"
#define EFFECT_DUST_02      L"e001"
#define EFFECT_BLUE_FIRE_01 L"e002"
#define EFFECT_BLUE_FIRE_02 L"e003"
#define EFFECT_BLUE_FIRE_03 L"e004"
#define EFFECT_WHITE_CIRCLE L"e005"
#define EFFECT_BLUE_HIT     L"e006"
#define EFFECT_RED_HIT		L"e007"
#define EFFECT_CLAIMH_SOLAIS L"e008" 
// 特效对象：播放一次动画后自动失活，不使用对象池
class Effect final : public GamePlay::GameObject
{
public:
	Effect();

	// 预热所有特效纹理到缓存，避免运行时 IO 卡顿（场景初始化时调用一次）
	static void PreloadTextures();

	// 在指定位置生成特效，flipX 控制水平翻转
	static Effect* Spawn(const std::wstring& effectKey, MathType::vec2f position, bool flipX = false,float opacity = 1.0f);

	void Update(float deltaTime) override;

private:
	GamePlay::Animator* mAnimator = nullptr;
	void setupEffect(const std::wstring& effectKey,float opacity);
};
