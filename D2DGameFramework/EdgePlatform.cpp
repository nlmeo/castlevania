#include "EdgePlatform.h"
#include "GamePlay/Debug.h"

EdgePlatform::EdgePlatform()
{
	mLayer = GamePlay::EdgePlatform;
	AddComponent<GamePlay::BoxCollider>();
}

void EdgePlatform::SetSize(float w, float h)
{
	GetComponent<GamePlay::BoxCollider>()->SetColliderSize(w, h);
}

void EdgePlatform::SetPosition(float x, float y)
{
	transform().SetPosition({ x, y });
}

void EdgePlatform::OnDebugRender()
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

