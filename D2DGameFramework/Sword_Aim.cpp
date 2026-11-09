#include "Sword_Aim.h"

void Sword_Aim::OnEnter()
{
	const MathType::vec2f atkDir = mOwner->GetAtkDir();

	rotationAngle = MathType::vec2f::down().SignedAngleDegree(atkDir);
	rotationType = (rotationAngle > 0) ? 0 : 1;  // 0顺时针 1逆时针 
	mOwner->transform().SetRotation(180.0f);
	isAimEnd = false;
	curAimTime = 0.0f;
}

void Sword_Aim::OnUpdate(float delta_time)
{
	

	float rAngle = delta_time * 300; 
	
	if (rotationType == 0)
	{
		
		if(rotationAngle - rAngle <=0)
		{
			rAngle = rotationAngle;
			isAimEnd = true;
		}
		rotationAngle -= rAngle;
		mOwner->transform().rotate(rAngle);
	}
	else
	{
		if (rotationAngle + rAngle >= 0)
		{
			rAngle = -rotationAngle;
			isAimEnd = true;
		}
		rotationAngle += rAngle;
		mOwner->transform().rotate(-rAngle);
	}
	
	curAimTime += delta_time;
	if (curAimTime >= MAX_AIM_TIME)
	{
		isAimEnd = true;
	}

	
	if (isAimEnd)
	{
		mFSM->SetNextState(L"attack");
	}

}

void Sword_Aim::OnQuit()
{
}
