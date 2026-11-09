#include "AxeBulletE.h"
#include "GameTool.h"
#include "./D2DCore/TextureManager.h"
#include "./GamePlay/Camera2d.h"

AxeBulletE::AxeBulletE()
{
	SetDestroyOnSceneExit();
	mLayer = GamePlay::EnemyAtk; // EnemyAttack
	AddComponent<GamePlay::SpriteRender>();
}

void AxeBulletE::Awake()
{
	mBodyCollider = GT::CreateGameObject<GamePlay::GameObject>();
	mBodyCollider->layer() = GamePlay::EnemyAtk;
	GamePlay::BoxCollider* c = mBodyCollider->AddComponent<GamePlay::BoxCollider>();
	c->SetColliderSize(50, 50);
	c->SetCollitionCallBack([&](GamePlay::Collider* c) { return onCollision(c); });

	if (rand() % 2)
		GT::PlaySE(SE_SOMA_THROW_1);
	else
		GT::PlaySE(SE_SOMA_THROW_2);
}

void AxeBulletE::OnDebugRender()
{
	if (mBodyCollider)
	{
		GamePlay::BoxCollider* bc = mBodyCollider->GetComponent<GamePlay::BoxCollider>();
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


void AxeBulletE::Init(int type, int dirX, MathType::vec2f startPos, float speed, float jumpPower, const wchar_t* textureKey)
{
	m_atkType = type;
	m_dirX = dirX;
	m_cursp = speed;
	m_curjp = jumpPower;
	m_g = 1.0f;
	transform().SetPosition(startPos);
	if (mBodyCollider)
	{
		mBodyCollider->transform().SetPosition(startPos);
		mBodyCollider->_ComponentUpdate(0);
	}

	GamePlay::SpriteRender* sprite = GetComponent<GamePlay::SpriteRender>();
	if (sprite && textureKey)
	{
		sprite->SetTexture(TexMgr.LoadTextureWithoutColor(textureKey, 0x00FF00));
		sprite->SetAnchor(0.5f, 0.5f);
		sprite->SetOrderInLayer(GameObjectMgr.GameObjectCount());
	}
}

void AxeBulletE::SetAtkType(int t)
{
	m_atkType = t;
}

void AxeBulletE::Update(float deltaTime)
{
	if (!Active()) return;

	UpdatePhysics(deltaTime);

	float anglePlus = (m_dirX == 4) ? -deltaTime * 600 : deltaTime * 600;
	m_angle += anglePlus;
	transform().rotate(anglePlus);


	if (mBodyCollider)
	{
		mBodyCollider->transform().SetPosition(transform().position());
		mBodyCollider->_ComponentUpdate(deltaTime);


		GamePlay::Collider& self = *mBodyCollider->GetComponent<GamePlay::BoxCollider>();
		std::vector<GamePlay::Collider*> hits;
		if (GT::CollisionDetect(self, ToBit(GamePlay::Player) | ToBit(GamePlay::Obstacle), hits))
		{
			for (GamePlay::Collider* c : hits)
			{
				onCollision(c);
			}
		}
	}


	MathType::vec2f curPos = transform().position();
	const MathType::RectF& camBound = MainCamera->CameraBound();
	if (!camBound.Contains(curPos.x, curPos.y))
	{
		Destroy();
	}
}

void AxeBulletE::OnDestroy()
{
	if (mBodyCollider)
	{
		mBodyCollider->Destroy();
		mBodyCollider = nullptr;
	}
}

void AxeBulletE::UpdatePhysics(float deltaTime)
{
	if (!mBodyCollider->Active())
	{
		float dx = (m_dirX == 6) ? m_cursp * deltaTime * 80.0f : -m_cursp * deltaTime * 80.0f;
		float dy = -m_curjp * deltaTime * 10.0f;
		transform().translate({ dx, dy });
		m_curjp -= m_g * 1.5f;
	}
	else
	{
		if (m_atkType == 1)
		{

			float dx = (m_dirX == 6) ? m_cursp * deltaTime * 80.0f : -m_cursp * deltaTime * 80.0f;
			float dy = -m_curjp * deltaTime * 10.0f;
			transform().translate({ dx, dy });
			m_curjp -= m_g * 1.5f;
		}
		else
		{

			float dx = (m_dirX == 6) ? (m_cursp)*deltaTime * 80.0f : -(m_cursp)*deltaTime * 80.0f;
			transform().translate({ dx, 0 });
			m_curjp -= m_g;
			m_cursp -= m_g * 0.2f;
		}
	}
	
}

void AxeBulletE::onCollision(GamePlay::Collider* c)
{
	if (!c || !c->gameobject()) return;

	if (c->gameobject()->layer() == GamePlay::PlayerAtk)
	{
		mIsHited = true;
		m_cursp = 1.0f;
		m_curjp = 2;
		mBodyCollider->SetActive(false);
	}

	//if (c->gameobject()->layer() == GamePlay::Player)
	//{


	//	Destroy();
	//	return;
	//}
	//else if (c->gameobject()->layer() == GamePlay::Obstacle)
	//{

	//	Destroy();
	//	return;
	//}
}
