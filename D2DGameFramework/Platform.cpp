#include "Platform.h"
#include "GamePlay/Debug.h"

Platform::Platform()
{
	mLayer = GamePlay::Platform;
	AddComponent<GamePlay::BoxCollider>();
}

void Platform::SetSize(float w, float h)
{
	GetComponent<GamePlay::BoxCollider>()->SetColliderSize(w, h);
}

void Platform::SetPosition(float x, float y)
{
	transform().SetPosition({ x, y });
}

void Platform::OnDebugRender()
{
	GamePlay::BoxCollider* bc = GetComponent<GamePlay::BoxCollider>();

	float posx = transform().position().x;
	float posy = transform().position().y;
	Debug.DebugDrawLine(
		posx - bc->Width() / 2,
		posy - bc->Height() / 2,
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

