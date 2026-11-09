#pragma once

struct Attributes
{
	int atk;							//攻击力
	int atk_cd;
	int cur_atk_cd;
	int atk_buff;

	int def;							//防御力
	int def_buff;

	int lck;							//幸运
	int lck_buff;

	float hp;
	float cur_hp;

	int mp = 0;                         // 体力上限（沿用 MP 蓝条）
	float cur_mp = 0;                   // 当前体力，保留持续消耗的小数精度
	int mp_recover_amount = 6;          // 每秒恢复体力

	int str;							//力量	
	int str_buff;

	int con;							//体质
	int con_buff;

	int intel;							//智力
	int intel_buff;

	int level;							//等级
	int exp;							//经验
	int next_level_exp;					//升级所需经验
	int gold;							//金币


	// ============ 物理相关 ====================
	 
	float sp;							//最大速度
	float cursp;						//当前速度
	float jp;							//跳跃力
	float curjp;						//当前跳跃力
	bool use_g = true;					//是否使用重力						
	float g;							//重力
	float cur_g;						//当前重力
	// ==========================================

	float minjp;						//最小跳跃力
	float max_jp_height;				//最大跳跃高度
	float cur_jp_height;				//当前跳跃高度
	int air_jump_remain;				//剩余空中跳跃次数（二段跳）
	float fall_height;					//下坠的距离
	float landing_fall_height;			//落地瞬间记录的落地高度（供 land 状态读取）

	int dx;								// 4 5 6
	int dy;								// 2 5 8

	bool is_hit;						//是否受伤
	bool is_hit_state_change = true;	//是否霸体
	bool is_hit_back;					//是否背部受击
	bool is_invulnurable;				//是否无敌
	float invulnurable_time;			//无敌时间
	float cur_invulnurable_time;		//当前无敌时间

	bool is_dead;						//是否死亡
	bool is_corpse;						//死亡动画播放完毕

	bool m_is_turn;						//是否转向	
	int	 m_last_scale_x;
	float last_bottom;
};
