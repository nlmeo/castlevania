#include "Enemy.h"
#include "Player.h"
#include "Soma.h"
#include "GameTool.h"

void Enemy::Hit(float atk,int atkType)
{
	switch (atkType)
	{
	case 1:
		GT::PlaySE(SE_ENEMY_HIT_MEAT);
		break;
	case 2:
		GT::PlaySE(SE_ENEMY_HIT_SOLID);
		break;
	case 3:
		GT::PlaySE(SE_HIT_GREAT_SWORD2);
		break;
	}
}

void Enemy::CaptureDeathScatterDirection()
{
	mDeathScatterDirection = PlayerDistanceX() > 0 ? -1.0f : 1.0f;
}

void Enemy::FacePlayer()
{
}

bool Enemy::PlayerInRange()
{
	Player* player = MainPlayer;
	if (player)
	{
		MathType::vec2f playerPos = player->transform().position();
		MathType::vec2f curPos = transform().position();
		return playerPos.x > curPos.x - mDetectRange.x &&
			playerPos.x < curPos.x + mDetectRange.x &&
			playerPos.y > curPos.y - mDetectRange.y &&
			playerPos.y < curPos.y + mDetectRange.y;
	}
	return false;
}

float Enemy::PlayerDistanceX()
{
	Player* player = MainPlayer;
	if (player)
	{
		return player->transform().position().x - transform().position().x;
	}
	return 0.0f;
}

float Enemy::PlayerDistanceY()
{
	Player* player = MainPlayer;
	if (player)
	{
		return player->transform().position().y - transform().position().y;
	}
	return 0.0f;
}



float Enemy::DetectWith() const
{
	return mDetectRange.x;
}

float Enemy::DetectHeight() const
{
	return mDetectRange.y;
}
