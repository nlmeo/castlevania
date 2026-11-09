#include "Sword_Attack.h"

void Sword_Attack::OnEnter()
{
	mAtkTimer = 0.0f;
	mAnimator->Play(L"attack");
}

void Sword_Attack::OnUpdate(float delta_time)
{
	if (mAnimator->GetState() == GamePlay::AS::AS_PLAYING)
	{
		return;
	}

	// 动画播放完毕后开始计时
	mAtkTimer += delta_time;

	mTransform->translate(mOwner->GetAtkDir() * delta_time * 800);
	MathType::vec2f curPos = mTransform->position();
	float camSizeX = MainCamera->SizeX();
	float camSizeY = MainCamera->SizeY();
	MathType::RectF camBound = MainCamera->CameraBound();
	float newLeft = curPos.x - camSizeX * 0.5f;
	float newRight = curPos.x + camSizeX * 0.5f;
	camBound.left = max(newLeft, camBound.left);
	camBound.right = min(newRight, camBound.right);

	// 超时未飞出边界 → 直接切 withdraw
	if (mAtkTimer >= MAX_ATK_TIME) // || !camBound.Contains(curPos.x, curPos.y))
	{
		mFSM->SetNextState(L"withdraw");
	}

}

void Sword_Attack::OnQuit()
{
	
}
