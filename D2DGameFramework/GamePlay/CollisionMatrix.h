#pragma once
#ifndef __COLLISION_MATRIX_H__
#define __COLLISION_MATRIX_H__
#include "GameLayer.h"

namespace GamePlay
{
	constexpr auto None         = GameLayer::Layer_10;
	constexpr auto Player       = GameLayer::Layer_11; 
	constexpr auto PlayerAtk    = GameLayer::Layer_12;
	constexpr auto Enemy        = GameLayer::Layer_13;
	constexpr auto EnemyAtk     = GameLayer::Layer_14;
	constexpr auto Obstacle     = GameLayer::Layer_15;
	constexpr auto Platform     = GameLayer::Layer_16;
	constexpr auto LeftSlop     = GameLayer::Layer_17;
	constexpr auto RightSlop    = GameLayer::Layer_18;
	constexpr auto EdgePlatform = GameLayer::Layer_19;
	constexpr auto Drop         = GameLayer::Layer_9;
	//帮助函数
	// ===================== 全局碰撞规则（位运算矩阵） =====================
	// 每一项都是：该层能与【哪些层】碰撞（用 | 组合）
	constexpr unsigned int Mask_Player          = ToBit(Enemy)  | ToBit(EnemyAtk) | ToBit(Obstacle) | ToBit(Platform) | ToBit(LeftSlop) | ToBit(RightSlop) | ToBit(EdgePlatform) | ToBit(Drop);
	constexpr unsigned int Mask_PlayerAtk       = ToBit(Enemy)  | ToBit(EnemyAtk);
	constexpr unsigned int Mask_Enemy           = ToBit(Player) | ToBit(PlayerAtk) | ToBit(Obstacle) | ToBit(Platform) | ToBit(LeftSlop) | ToBit(RightSlop);
	constexpr unsigned int Mask_EnemyAtk        = ToBit(Player) | ToBit(PlayerAtk);
	constexpr unsigned int Mask_Obstacle        = ToBit(Player) | ToBit(Enemy);
	constexpr unsigned int Mask_Platform        = ToBit(Player) | ToBit(Enemy);
	constexpr unsigned int Mask_LeftSlop        = ToBit(Player) | ToBit(Enemy);
	constexpr unsigned int Mask_RightSlop       = ToBit(Player) | ToBit(Enemy);
	constexpr unsigned int Mask_EdgePlatform   = ToBit(Player) | ToBit(Enemy);
	constexpr unsigned int Mask_Drop            = ToBit(Player);
	constexpr unsigned int Mask_None = 0;

	/**
	* @brief 核心判断：两个物体是否可以碰撞（位运算，超快）
	* @param selfLayer 自身碰撞层
	* @param targetLayer 目标碰撞层
	* @return true=可碰撞 / false=忽略
	*/
	inline bool CanCollide(GameLayer selfLayer, GameLayer targetLayer)
	{
		unsigned int targetBit = ToBit(targetLayer);

		// 根据自身层，取出对应的碰撞掩码
		switch (selfLayer)
		{
		case Player:                    return (Mask_Player & targetBit)        != 0;
		case PlayerAtk:                 return (Mask_PlayerAtk & targetBit)     != 0;
		case Enemy:                     return (Mask_Enemy & targetBit)         != 0;
		case EnemyAtk:                  return (Mask_EnemyAtk & targetBit)      != 0;
		case Obstacle:                  return (Mask_Obstacle & targetBit)      != 0;
		case Platform:                  return (Mask_Platform & targetBit)      != 0;
		case LeftSlop:                  return (Mask_LeftSlop & targetBit)      != 0;
		case RightSlop:                 return (Mask_RightSlop & targetBit)     != 0;
		case EdgePlatform:              return (Mask_EdgePlatform & targetBit)  != 0;
		case Drop:                      return (Mask_Drop & targetBit)          != 0;
		default:                        return false;
		}
	}

}



#endif // !__COLLISION_MATRIX_H__
