#pragma once
#ifndef __MY_UIOBJ_ARROW_H__
#define __MY_UIOBJ_ARROW_H__

#include "GamePlay/GameObject.h"


class Arrow : public GamePlay::GameObject
{
public:
	Arrow() { SetDestroyOnSceneExit(); }
	~Arrow() = default;
	virtual void Awake() override;
};


#endif // !__MY_UIOBJ_ARROW_H__

