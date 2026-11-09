#include "Arrow.h"
#include "GamePlay/Animator.h"
void Arrow::Awake()
{
	GamePlay::Animator* pAni = AddComponent<GamePlay::Animator>();
	
	GamePlay::AnimationClip clip;
	clip.AddFrames({
		{L"./res/texture/arrow/arrow1.bmp",{0,0},0,0},
		{L"./res/texture/arrow/arrow2.bmp",{0,0},0,0},
		{L"./res/texture/arrow/arrow3.bmp",{0,0},0,0},
		{L"./res/texture/arrow/arrow4.bmp",{0,0},0,0},
		});
}
