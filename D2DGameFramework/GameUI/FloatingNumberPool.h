#pragma once
#ifndef __FLOATING_NUMBER_POOL_H__
#define __FLOATING_NUMBER_POOL_H__

#include "./UIFloatingNumber.h"
#include "../HelpTool/SingletonTemplate.h"

/**
 * @brief 全局浮动数字池（单例）
 *
 * 独立于场景的 UICanvas，在 GameLoop 中自行 Update/Render。
 * 各个场景通过 FLOATINGNUMS.Spawn(...) 直接调用。
 *
 * 使用流程：
 *   1. 游戏启动时调用 InitWithTextures / InitWithStrip 初始化
 *   2. 受击时 FLOATINGNUMS.Spawn(damage, screenX, screenY)
 *   3. GameLoop 自动 Update + Render
 */
class FloatingNumberPool : public Singleton<FloatingNumberPool>
{
	PRIVATE_COPY(FloatingNumberPool);
	FloatingNumberPool();
public:
	/** @brief 用 10 张独立纹理初始化（每张对应数字 0~9） */
	void InitWithTextures(D2DCore::Texture* digitTexs[10]);

	/** @brief 用单张纹理条初始化（横向 10 等份：0~9） */
	void InitWithStrip(D2DCore::Texture* stripTex, float digitW, float digitH);

	/** @brief 生成一个浮动数字 */
	void Spawn(int number, float startX, float startY,
		float floatHeight = 60.0f, float duration = 1.0f,bool isUseWordPos = false);

	/** @brief 每帧更新（GameLoop 中调用） */
	void Update(float dt);

	/** @brief 每帧渲染（需在 BeginDraw / EndDraw 之间，GameLoop 中调用） */
	void Render();
	/** @brief 取消旧场景的浮动数字，保留池和纹理以供复用。 */
	void Clear();

	/** @brief 是否已初始化 */
	bool IsInitialized() const { return mInitialized; }

	static FloatingNumberPool& HitInstance()
	{
		static FloatingNumberPool hitInstance;
		return hitInstance;
	}

private:
	GameUI::UIFloatingNumber* mDmgPool[10] = {};
	int   mPoolIdx = 0;
	bool  mInitialized = false;
};

#define FLOATINGNUMS FloatingNumberPool::Instance()
#define FLOATINGNUMS_HIT FloatingNumberPool::HitInstance()
#endif
