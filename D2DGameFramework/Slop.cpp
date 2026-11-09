#include "Slop.h"
#include "GamePlay/Debug.h"

LeftSlop::LeftSlop()
{
	
	mLayer = GamePlay::LeftSlop;
	AddComponent<GamePlay::BoxCollider>();
}

void LeftSlop::SetSize(float w, float h)
{
	GetComponent<GamePlay::BoxCollider>()->SetColliderSize(w, h);
}

void LeftSlop::SetPosition(float x, float y)
{
	transform().SetPosition({ x, y });
}

void LeftSlop::OnDebugRender()
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
	Debug.DebugDrawLine(
		posx - bc->Width() / 2,
		posy + bc->Height() / 2,
		posx + bc->Width() / 2,
		posy - bc->Height() / 2,
		ColorType::COLORF::Red()
	);
}



RightSlop::RightSlop()
{
	mLayer = GamePlay::RightSlop;
	AddComponent<GamePlay::BoxCollider>();
}

void RightSlop::SetSize(float w, float h)
{
	GetComponent<GamePlay::BoxCollider>()->SetColliderSize(w, h);
}

void RightSlop::SetPosition(float x, float y)
{
	transform().SetPosition({ x, y });
}

void RightSlop::OnDebugRender()
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
	Debug.DebugDrawLine(
		posx - bc->Width() / 2,
		posy - bc->Height() / 2,
		posx + bc->Width() / 2,
		posy + bc->Height() / 2,
		ColorType::COLORF::Red()
	);
}


