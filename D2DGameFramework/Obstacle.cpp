#include "Obstacle.h"
#include "GamePlay/Debug.h"

Obstacle::Obstacle()
{
	mLayer = GamePlay::Obstacle;
	AddComponent<GamePlay::BoxCollider>();
}

void Obstacle::SetSize(float w, float h)
{
	GetComponent<GamePlay::BoxCollider>()->SetColliderSize(w,h);
}

void Obstacle::SetPosition(float x, float y)
{
	transform().SetPosition({ x, y });
}

void Obstacle::OnDebugRender()
{
	GamePlay::BoxCollider* bc = GetComponent<GamePlay::BoxCollider>();

	float posx = transform().position().x;
	float posy = transform().position().y;
	Debug.DebugDrawLine(
		posx - bc->Width() / 2,
		posy - bc->Height()/ 2, 
		posx + bc->Width() / 2, 
		posy - bc->Height() / 2,
		ColorType::COLORF::Red()
	);
	Debug.DebugDrawLine(
		posx - bc->Width() / 2,
		posy - bc->Height() / 2,
		posx - bc->Width() / 2,
		posy + bc->Height() / 2,
		ColorType::COLORF::Red()
	);
	Debug.DebugDrawLine(
		posx + bc->Width() / 2,
		posy - bc->Height() / 2,
		posx + bc->Width() / 2,
		posy + bc->Height() / 2,
		ColorType::COLORF::Red()
	);
	Debug.DebugDrawLine(
		posx - bc->Width() / 2,
		posy + bc->Height() / 2,
		posx + bc->Width() / 2,
		posy + bc->Height() / 2,
		ColorType::COLORF::Red()
	);
}

