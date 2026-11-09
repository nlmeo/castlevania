#pragma once
#ifndef __SWORD_ATTACK3_H__
#define __SWORD_ATTACK3_H__
#include "SwordStates.h"


class Sword_Attack3 : public SwordStates
{
public:
	virtual void OnEnter();
	virtual void OnUpdate(float delta_time = 0);
	virtual void OnQuit();
private:
	enum Phase { PHASE_AIM, PHASE_FLY, PHASE_WITHDRAW };
	Phase mPhase = PHASE_AIM;

	
	float mAimRotationAngle = 0.0f;         // ��Ҫ��ת��Ŀ��ǶȲ�
	int mAimRotationType = 0;               // 0:˳ʱ�� 1:��ʱ��
	float mAimTimer = 0.0f;                 // ��׼��ʱ��
	static constexpr float MAX_AIM_TIME = 2.0f;     // �����׼ʱ�䣨�룩
	static constexpr float AIM_ROT_SPEED = 300.0f;  // ��׼��ת�ٶȣ���/�룩

	// ===== �ڶ��׶�(FLY)��Ա =====
	float mFlyTimer = 0.0f;                 // ���м�ʱ��
	static constexpr float MAX_FLY_TIME = 2.0f;     // ������ʱ�䣨�룩
	static constexpr float FLY_SPEED = 800.0f;      // �����ٶȣ�����/�룩

	// ===== �����׶�(WITHDRAW)��Ա =====
	bool mWithdrawAboveDone = false;        // �Ƿ������˲�Ƶ��Ϸ�
	float mWithdrawRotRemain = 0.0f;        // ʣ����ת�Ƕ�
	int mWithdrawRotDir = 0;                // 0:˳ʱ��(�ҽ�) 1:��ʱ��(��)
	static constexpr float ABOVE_OFFSET = 100.0f;   // ���ҵ��Ϸ�ƫ��
	static constexpr float WITHDRAW_MOVE_SPEED = 200.0f;  // �յ��ƶ��ٶ�
	static constexpr float WITHDRAW_ROT_SPEED = 360.0f;  // �յ���ת�ٶȣ���/�룩
};
#endif
