#include "AxeBulletP.h"
#include "./D2DCore/TextureManager.h"
#include "GameTool.h"
#include "./GamePlay/Camera2d.h"
#include "Enemy.h"
#include "./GamePlay/ChildObject.h"
AxeBulletP::AxeBulletP()
{
	SetDestroyOnSceneExit();
	auto a = AddComponent<GamePlay::AfterImageTrail>();
	a->SetCapState(true);
	mBodyCollider = GT::CreateGameObject<GamePlay::GameObject>();
	mBodyCollider->layer() = GamePlay::PlayerAtk;
	GamePlay::BoxCollider* c = mBodyCollider->AddComponent<GamePlay::BoxCollider>();
	c->SetColliderSize(100, 100);
	c->SetCollitionCallBack([&](GamePlay::Collider* c) { onCollision(c); });
	mLayer = GamePlay::GameLayer::Layer_5;
	AddComponent<GamePlay::SpriteRender>();

	mAttributes.cursp = 5.0f;
	mAttributes.curjp = 25.0f;
	mAttributes.cur_g = 1.0f;
	mAttributes.dx = 6;
	
}

void AxeBulletP::Awake() 
{


}

void AxeBulletP::Update(float deltaTime)
{
	if (!Active()) return;
	mElapsed += deltaTime;
	UpdatePhysics(deltaTime);

	float anglePlus = (mAttributes.dx == 4) ? -deltaTime * 700 : deltaTime * 700;
	transform().rotate(anglePlus);

	mBodyCollider->transform().SetPosition(transform().position());
	mBodyCollider->_ComponentUpdate(0);
	MathType::vec2f curPos = transform().position();
	float camSizeX = MainCamera->SizeX();
	float camSizeY = MainCamera->SizeY();
	MathType::RectF camBound = MainCamera->CameraBound();
	//camBound.left = curPos.x - camSizeX * 0.5f;
	//camBound.right = curPos.x + camSizeX * 0.5f;

	if (curPos.y > camBound.bottom)
	{
		Destroy();
	}


	//if (!camBound.Contains( curPos.x,curPos.y ))
	//{
	//	Destroy();
	//}
}

void AxeBulletP::OnDestroy() {
	if (mBodyCollider) { mBodyCollider->Destroy(); mBodyCollider = nullptr; }
}

void AxeBulletP::OnDebugRender()
{

	GamePlay::BoxCollider* bc = mBodyCollider->GetComponent<GamePlay::BoxCollider>();
	if (bc != nullptr)
	{
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

}

void AxeBulletP::UpdatePhysics(float deltaTime)
{
	
	mAttributes.cur_g = mAttributes.g;
	mAttributes.curjp -= mAttributes.cur_g;

	float dx = (mAttributes.dx == 6) ? mAttributes.cursp * deltaTime * 80.0f : -mAttributes.cursp * deltaTime * 80.0f;
	float dy = -mAttributes.curjp * deltaTime * 10.0f;

	transform().translate({ dx, dy });
}

void AxeBulletP::SetDirection(int dx, int dy)
{
	mAttributes.dx = dx;
	mAttributes.dy = dy;
}

void AxeBulletP::SetSpeed(float spd)
{
	mAttributes.cursp = spd;
}

void AxeBulletP::SetJumpPower(float jp)
{
	mAttributes.curjp = jp;
}

void AxeBulletP::SetGravity(float g)
{
	mAttributes.g = g;
}


void AxeBulletP::SetPosition(float x, float y)
{
	transform().SetPosition({ x, y });
	//mBodyCollider->transform().SetPosition({ x, y });
}

void AxeBulletP::SetLevel(int level)
{

	GamePlay::SpriteRender* spriteRender = GetComponent<GamePlay::SpriteRender>();
	
	switch (level)
	{
	case 1:
		spriteRender->SetTexture(TexMgr.LoadTextureWithoutColor(L"resource\\skills\\p_axe\\frame002.bmp",0x00ff00));
		break;
	case 2:
		spriteRender->SetTexture(TexMgr.LoadTextureWithoutColor(L"resource\\skills\\p_axe\\frame003.bmp", 0x00ff00));
		break;
	case 3:
		spriteRender->SetTexture(TexMgr.LoadTextureWithoutColor(L"resource\\skills\\p_axe\\frame001.bmp", 0x00ff00));
		break;
	default:
		break;
	}
	spriteRender->SetAnchor(0.5f, 0.5f);
	spriteRender->SetOrderInLayer(GameObjectMgr.GameObjectCount());
}

void AxeBulletP::onCollision(GamePlay::Collider* c)
{
	GamePlay::GameObject* other = c->gameobject();
	if (other->layer() == GamePlay::Enemy)
	{
		GamePlay::ChildObject* cp = dynamic_cast<GamePlay::ChildObject*>(other);
		Enemy* p = cp->GetOwner<Enemy>();
		auto& record = mHitRecords[p];
		if (record.count >= mMaxHitsPerEnemy)
			return;
		if (mElapsed - record.lastHitTime < mHitInterval)
			return;
		record.lastHitTime = mElapsed;
		record.count++;
		p->Hit(11, 1);
	}
}


