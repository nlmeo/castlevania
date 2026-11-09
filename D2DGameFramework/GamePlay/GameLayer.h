#pragma once
#ifndef __MY_GAME_LAYER_H__
#define __MY_GAME_LAYER_H__


namespace GamePlay
{
	/**
* @brief 游戏层级枚举
*
* 使用位掩码方式定义32个游戏层级，支持层级的组合和过滤
*/
	enum class GameLayer : unsigned int
	{											
		Layer_0  = 0x00000001,   ///< 层级0		背景
		Layer_1  = 0x00000002,   ///< 层级1		远景
		Layer_2  = 0x00000004,   ///< 层级2		中景
		Layer_3  = 0x00000008,   ///< 层级3		逻辑
		Layer_4  = 0x00000010,   ///< 层级4		近景
		Layer_5  = 0x00000020,   ///< 层级5		UI
		Layer_6  = 0x00000040,   ///< 层级6		
		Layer_7  = 0x00000080,   ///< 层级7		
		Layer_8  = 0x00000100,   ///< 层级8		
		Layer_9  = 0x00000200,   ///< 层级9		Drop -- 碰撞层级 --
		Layer_10 = 0x00000400,   ///< 层级10	None 
		Layer_11 = 0x00000800,   ///< 层级11	Player
		Layer_12 = 0x00001000,   ///< 层级12	PlayerAtk
		Layer_13 = 0x00002000,   ///< 层级13	Enemy		
		Layer_14 = 0x00004000,   ///< 层级14	EnemyAtk
		Layer_15 = 0x00008000,   ///< 层级15	Obstacle
		Layer_16 = 0x00010000,   ///< 层级16	Platform
		Layer_17 = 0x00020000,   ///< 层级17	Slop
		Layer_18 = 0x00040000,   ///< 层级18	
		Layer_19 = 0x00080000,   ///< 层级19
		Layer_20 = 0x00100000,   ///< 层级20
		Layer_21 = 0x00200000,   ///< 层级21
		Layer_22 = 0x00400000,   ///< 层级22
		Layer_23 = 0x00800000,   ///< 层级23
		Layer_24 = 0x01000000,   ///< 层级24
		Layer_25 = 0x02000000,   ///< 层级25
		Layer_26 = 0x04000000,   ///< 层级26
		Layer_27 = 0x08000000,   ///< 层级27
		Layer_28 = 0x10000000,   ///< 层级28
		Layer_29 = 0x20000000,   ///< 层级29
		Layer_30 = 0x40000000,   ///< 层级30
		Layer_31 = 0x80000000    ///< 层级31
	};

	inline constexpr unsigned int ToBit(GameLayer layer)
	{
		return static_cast<unsigned int>(layer);
	}

}
#endif // !__MY_GAME_LAYER_H__


