#pragma once

#include "GameUI/UICanvas.h"
#include "GameUI/UIImage.h"
#include "GameUI/UINumber.h"

class PlayerState
{
	GameUI::UIImage* mBackGround = nullptr;
	GameUI::UIImage* mHPBar = nullptr;
	GameUI::UIImage* mMPBar = nullptr;
	GameUI::UINumber* mLevelNumber = nullptr;
	bool mIsInit = false;
public:

	PlayerState() = default;
	~PlayerState();
	PlayerState(const PlayerState&) = delete;
	PlayerState& operator=(const PlayerState&) = delete;

	void Init(GameUI::UICanvas* p);
	void Update();
	bool IsInit() { return mIsInit; }
};