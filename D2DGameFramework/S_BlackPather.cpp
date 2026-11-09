#include "S_BlackPather.h"
#include "Player.h"
#include "Soma.h"
#include "GameTool.h"
void S_BlackPather::Init()
{
	mSoulId = 2;
	mInputType = 2; //触发类型： 1-开关类型 2-持续按压
	mRunType = 2;   //运行类型: 1-不需要接管角色状态（附加效果） 2-临时角色状态（类似枪兵旋转技能）
	mFsmStateKey = L"black_panther";
	mLayer = GamePlay::GameLayer::Layer_5;
	int order = 999;
	

	GamePlay::AnimationClip clip;
	clip.m_isLoop = true;
	clip.AddFrames({
			{ L"resource\\skills\\p_blackpanther\\frame000.png",{0.5f,0.5f},2, 0xFFFFFF },
			{ L"resource\\skills\\p_blackpanther\\frame001.png",{0.5f,0.5f},2, 0xFFFFFF },
			{ L"resource\\skills\\p_blackpanther\\frame002.png",{0.5f,0.5f},2, 0xFFFFFF },
			{ L"resource\\skills\\p_blackpanther\\frame003.png",{0.5f,0.5f},2, 0xFFFFFF },
			{ L"resource\\skills\\p_blackpanther\\frame004.png",{0.5f,0.5f},2, 0xFFFFFF },
			{ L"resource\\skills\\p_blackpanther\\frame005.png",{0.5f,0.5f},2, 0xFFFFFF },
			{ L"resource\\skills\\p_blackpanther\\frame006.png",{0.5f,0.5f},2, 0xFFFFFF },
			{ L"resource\\skills\\p_blackpanther\\frame007.png",{0.5f,0.5f},2, 0xFFFFFF },
			{ L"resource\\skills\\p_blackpanther\\frame008.png",{0.5f,0.5f},2, 0xFFFFFF },
			{ L"resource\\skills\\p_blackpanther\\frame009.png",{0.5f,0.5f},2, 0xFFFFFF },
			{ L"resource\\skills\\p_blackpanther\\frame00A.png",{0.5f,0.5f},2, 0xFFFFFF },
			{ L"resource\\skills\\p_blackpanther\\frame00B.png",{0.5f,0.5f},2, 0xFFFFFF },
		});
	a1 = GT::PlayAnimation(clip, L"idle", ++order);
	a1->gameobject()->layer() = GamePlay::GameLayer::Layer_5;
	a1->SetOpcity(0);
	SetActive(false);
}

void S_BlackPather::Start()
{
}

void S_BlackPather::Update(float deltaTime)
{
	if (MainPlayer == nullptr) { return; }

	Soma* pSoma = dynamic_cast<Soma*>(MainPlayer);
	float curScaleX = pSoma->transform().localScale().x;
	MathType::vec2f curPos = pSoma->transform().position();
	MathType::vec2f curScale = { 1,1 };
	if (curScaleX < 0) { curScale.x = -1; }

	curPos.x += curScaleX < 0 ? +20.0f : -20.0f;
	curPos.y -= 25.0f;

	a1->transform()->SetPosition(curPos);
	a1->transform()->SetScale(curScale);
	// 持续按压型技能：按住 K 键时接管状态
	const bool held = GT::KeyDown('K');
	if (!held) ResetStaminaUsage();
	bool canUse = false;
	if (auto* fsm = MainPlayer->GetComponent<GamePlay::FSM>())
	{
		auto* cur = fsm->GetCurrentState();
		canUse = IsFsmActive() || cur == fsm->GetState(L"idle") || cur == fsm->GetState(L"walk") ||
			cur == fsm->GetState(L"dash_back") || cur == fsm->GetState(L"jump") || cur == fsm->GetState(L"fall");
	}
	if (held && canUse && TrySustain(deltaTime))
	{
		if (!IsFsmActive())
		{
			EnterFsmState();
		}
		a1->SetOpcity(1);
			
	}
	else
	{
		a1->SetOpcity(0);
		if (IsFsmActive()) ExitFsmState();
			
	}
}

void S_BlackPather::OnDestroy()
{
	if (a1) { a1->gameobject()->Destroy(); a1 = nullptr; }
}
