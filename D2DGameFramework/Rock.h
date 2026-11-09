#pragma once
#ifndef __GAME_CONTENT_OBJ_ROCK_H__
#define __GAME_CONTENT_OBJ_ROCK_H__

#include "./GamePlay/GameObject.h"

class Rock:public GamePlay::GameObject
{
public:
	virtual void Awake() override;
	virtual void OnEnable()  override;
	virtual void Start()  override;
	virtual void Update(float deltaTime)  override;
	virtual void OnDisable()  override;
	virtual void OnDestroy()  override;

protected:
private:
	float speed = 30.0f;
	float lifetime = 1.0f;
	float livetime = 0.0f;
};

#endif