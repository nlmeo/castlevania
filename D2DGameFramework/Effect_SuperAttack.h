#pragma once
#include "GamePlay/GameObject.h"
// 特效对象：播放一次动画后自动失活，不使用对象池
class Effect_SuperAttack final : public GamePlay::GameObject
{
public:
	Effect_SuperAttack();

	virtual void Awake();


	virtual void OnDestroy();

	// 在指定位置生成特效，flipX 控制水平翻转
	static Effect_SuperAttack* Spawn(MathType::vec2f position, bool flipX = false);

	void Update(float deltaTime) override;

private:
	GamePlay::Animator* mAnimator = nullptr;
	GamePlay::GameObject* mEffectObject1 = nullptr;
	GamePlay::GameObject* mEffectObject2 = nullptr;
};
