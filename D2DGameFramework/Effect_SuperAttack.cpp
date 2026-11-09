#include "Effect_SuperAttack.h"
#include "EffectsPool.h"
#include "GameTool.h"

#define COMMON(hex)  L"resource\\effects\\common\\frame" hex L".bmp"

Effect_SuperAttack::Effect_SuperAttack()
{
	SetDestroyOnSceneExit();
    mLayer = GamePlay::GameLayer::Layer_4;
}

void Effect_SuperAttack::Awake()
{
	int order = GameObjectMgr.GameObjectCount();

    mAnimator = AddComponent<GamePlay::Animator>();
    GamePlay::AnimationClip clip;
	clip.AddFrames({
			{ COMMON("0C0"), {0.5f, 0.5f}, 3, 0x00FF00 },
			{ COMMON("0C1"), {0.5f, 0.5f}, 3, 0x00FF00 },
			{ COMMON("0C2"), {0.5f, 0.5f}, 3, 0x00FF00 },
			{ COMMON("0C3"), {0.5f, 0.5f}, 3, 0x00FF00 },
			{ COMMON("0C4"), {0.5f, 0.5f}, 3, 0x00FF00 },
		});
	clip.m_isLoop = false;
	mAnimator->AddClip(L"superatk", clip);
	mAnimator->SetOpcity(0.8f);
	mAnimator->Play(L"superatk");
	mAnimator->SetOrderInLayer(++order);
	
	mEffectObject1 = GT::CreateGameObject<GamePlay::GameObject>();
	mEffectObject1->transform().SetScale({ 2.0f,2.0f });
	auto* sp1 = GT::DrawPicture(COMMON("0C5"), transform().position(), order++, {0.5f,0.5f}, 0x00FF00, mEffectObject1, GamePlay::GameLayer::Layer_4);
	sp1->SetOpcity(0.8f);


	mEffectObject2 = GT::CreateGameObject<GamePlay::GameObject>();
	mEffectObject2->transform().SetScale({ 2.0f,2.0f });
	auto* sp2 = GT::DrawPicture(COMMON("0C5"), transform().position(), order++, { 0.5f,0.5f }, 0x00FF00, mEffectObject2, GamePlay::GameLayer::Layer_4);
	sp2->SetOpcity(0.8f);

	auto a = mEffectObject1->Active();
	//GamePlay::SpriteRender* sp1 = GT::DrawPicture("",,++order, GamePlay::GameLayer::Layer_4;)
	//mEffectObject1 = 
}

void Effect_SuperAttack::OnDestroy()
{
    SAFE_DESTROY(mEffectObject1);
    SAFE_DESTROY(mEffectObject2);
}

Effect_SuperAttack* Effect_SuperAttack::Spawn(MathType::vec2f position, bool flipX)
{
	Effect_SuperAttack* effect = GamePlay::GameObject::CreateGameObject<Effect_SuperAttack>();
	effect->transform().SetPosition(position);
	
	
	return effect;
}

void Effect_SuperAttack::Update(float deltaTime)
{

	float curScale = transform().localScale().y;
	curScale += deltaTime * 3;
	curScale = curScale > 2 ? 2 : curScale;
	transform().SetScale({ curScale, curScale });

	float curOpacity = mAnimator->Opacity();
	curOpacity -= deltaTime * 3;
	curOpacity = curOpacity < 0 ? 0 : curOpacity;
	mAnimator->SetOpcity(curOpacity);



	curScale = mEffectObject1->transform().localScale().y;
	curScale -= deltaTime * 3;
	curScale = curScale < 1 ? 1 : curScale;
	mEffectObject1->transform().SetScale({ curScale, curScale });
	auto* sp1 = mEffectObject1->GetComponent<GamePlay::SpriteRender>();
	sp1->SetOpcity(curOpacity);


	curScale = mEffectObject2->transform().localScale().y;
	curScale += deltaTime * 3;
	curScale = curScale > 3 ? 3 : curScale;
	mEffectObject2->transform().SetScale({ curScale, curScale });
	auto* sp2 = mEffectObject2->GetComponent<GamePlay::SpriteRender>();
	sp2->SetOpcity(curOpacity);


	if (mAnimator && mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{

		SAFE_DESTROY(mEffectObject1);
		SAFE_DESTROY(mEffectObject2);
		Destroy();
	}
}
