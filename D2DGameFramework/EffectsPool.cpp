#include "EffectsPool.h"
#include "./GamePlay/Animator.h"
#include "./GamePlay/GameObject.h"
#include "./D2DCore/TextureManager.h"

#define COMMON(hex)  L"resource\\effects\\common\\frame" hex L".bmp"
#define BFIRE(hex)   L"resource\\effects\\blue_fire\\frame" hex L".bmp"
Effect::Effect()
{
	SetDestroyOnSceneExit();
	mLayer = GamePlay::GameLayer::Layer_4;
}

void Effect::PreloadTextures()
{
	static const std::vector<std::wstring> allTextures = {
		// Dust
		COMMON("090"), COMMON("091"), COMMON("092"), COMMON("093"), COMMON("094"),
		COMMON("095"), COMMON("096"), COMMON("097"), COMMON("098"), COMMON("099"),
		COMMON("09A"), COMMON("09B"), COMMON("09C"), COMMON("09E"),
		// Blue Fire (all frames from 00C to 033)
		BFIRE("00C"), BFIRE("00D"), BFIRE("00E"), BFIRE("00F"),
		BFIRE("010"), BFIRE("011"), BFIRE("012"), BFIRE("013"),
		BFIRE("014"), BFIRE("015"), BFIRE("016"), BFIRE("017"),
		BFIRE("018"), BFIRE("019"), BFIRE("01A"), BFIRE("01B"),
		BFIRE("01C"), BFIRE("01D"), BFIRE("01E"), BFIRE("01F"),
		BFIRE("020"), BFIRE("021"), BFIRE("022"), BFIRE("023"),
		BFIRE("024"), BFIRE("025"), BFIRE("026"), BFIRE("027"),
		BFIRE("028"), BFIRE("029"), BFIRE("02A"), BFIRE("02B"),
		BFIRE("02C"), BFIRE("02D"), BFIRE("02E"), BFIRE("02F"),
		BFIRE("030"), BFIRE("031"), BFIRE("032"), BFIRE("033"),
		//Spark
		COMMON("056"), COMMON("057"), COMMON("058"), COMMON("059"),COMMON("05A"),
		//Claimh Solais
		COMMON("079"), COMMON("07A"), COMMON("07B"), COMMON("07C"),COMMON("07D"),
		COMMON("07E"), COMMON("07F"), COMMON("080"), COMMON("081"),COMMON("082"),
	};

	for (const auto& path : allTextures)
	{
		TexMgr.LoadTextureWithoutColor(path.c_str(), 0x00FF00);
	}
}


Effect* Effect::Spawn(const std::wstring& effectKey, MathType::vec2f position, bool flipX,float opacity)
{
	Effect* effect = GamePlay::GameObject::CreateGameObject<Effect>();
	effect->transform().SetPosition(position);
	effect->transform().SetScale({ flipX ? -1.0f : 1.0f, 1.0f });
	effect->setupEffect(effectKey,opacity);
	return effect;
}

void Effect::setupEffect(const std::wstring& effectKey,float opacity)
{
	mAnimator = AddComponent<GamePlay::Animator>();
	GamePlay::AnimationClip clip;

	if (effectKey == EFFECT_DUST_01)
	{
		clip.AddFrames({
			{ COMMON("090"), {0.5f, 0.5f}, 2, 0x00FF00 },
			{ COMMON("091"), {0.5f, 0.5f}, 4, 0x00FF00 },
			{ COMMON("09A"), {0.5f, 0.5f}, 4, 0x00FF00 },
			{ COMMON("09B"), {0.5f, 0.5f}, 2, 0x00FF00 },
			{ COMMON("09C"), {0.5f, 0.5f}, 2, 0x00FF00 },
			{ COMMON("09E"), {0.5f, 0.5f}, 2, 0x00FF00 },
		});
	}
	else if (effectKey == EFFECT_DUST_02)
	{
		clip.AddFrames({
			{ COMMON("090"), {0.5f, 0.5f}, 2, 0x00FF00 },
			{ COMMON("091"), {0.5f, 0.5f}, 2, 0x00FF00 },
			{ COMMON("092"), {0.5f, 0.5f}, 4, 0x00FF00 },
			{ COMMON("093"), {0.5f, 0.5f}, 4, 0x00FF00 },
			{ COMMON("094"), {0.5f, 0.5f}, 4, 0x00FF00 },
			{ COMMON("095"), {0.5f, 0.5f}, 2, 0x00FF00 },
			{ COMMON("096"), {0.5f, 0.5f}, 2, 0x00FF00 },
			{ COMMON("097"), {0.5f, 0.5f}, 2, 0x00FF00 },
			{ COMMON("098"), {0.5f, 0.5f}, 2, 0x00FF00 },
			{ COMMON("099"), {0.5f, 0.5f}, 2, 0x00FF00 },
		});
	}
	else if (effectKey == EFFECT_BLUE_FIRE_01)
	{
		clip.AddFrames({
			{ BFIRE("01C"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("01D"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("01E"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("01F"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("020"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("02E"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("02F"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("030"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("031"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("032"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("033"), {0.5f, 0.53f}, 2, 0x00FF00 },
		});
	}
	else if (effectKey == EFFECT_BLUE_FIRE_02)
	{
		clip.AddFrames({
			{ BFIRE("00C"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("00D"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("00E"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("00F"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("010"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("011"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("012"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("013"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("014"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("015"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("016"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("017"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("018"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("019"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("01A"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("01B"), {0.5f, 0.53f}, 2, 0x00FF00 },
		});
	}
	else if (effectKey == EFFECT_BLUE_FIRE_03)
	{
		clip.AddFrames({
			{ BFIRE("01C"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("01D"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("01E"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("01F"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("020"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("021"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("022"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("023"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("024"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("025"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("026"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("027"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("028"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("029"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("02A"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("02B"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("02C"), {0.5f, 0.53f}, 2, 0x00FF00 },
			{ BFIRE("02D"), {0.5f, 0.53f}, 2, 0x00FF00 },
		});
	}
	else if (effectKey == EFFECT_BLUE_HIT)
	{
		clip.AddFrames({
			{ COMMON("061"), {0.5f, 0.5f}, 4, 0x00FF00 },
			{ COMMON("062"), {0.5f, 0.5f}, 4, 0x00FF00 },
			{ COMMON("063"), {0.5f, 0.5f}, 4, 0x00FF00 },
			{ COMMON("064"), {0.5f, 0.5f}, 4, 0x00FF00 },
			{ COMMON("065"), {0.5f, 0.5f}, 4, 0x00FF00 },
		});
	}
	else if (effectKey == EFFECT_WHITE_CIRCLE)
	{
		clip.AddFrames({
			{ COMMON("0C0"), {0.5f, 0.5f}, 4, 0x00FF00 },
			{ COMMON("0C1"), {0.5f, 0.5f}, 4, 0x00FF00 },
			{ COMMON("0C2"), {0.5f, 0.5f}, 4, 0x00FF00 },
			{ COMMON("0C3"), {0.5f, 0.5f}, 4, 0x00FF00 },
			{ COMMON("0C4"), {0.5f, 0.5f}, 4, 0x00FF00 },
		});
	}
	else if (effectKey == EFFECT_RED_HIT)
	{
		clip.AddFrames({
			{ COMMON("056"), {0.5f, 0.5f}, 2, 0x00FF00 },
			{ COMMON("057"), {0.5f, 0.5f}, 2, 0x00FF00 },
			{ COMMON("058"), {0.5f, 0.5f}, 2, 0x00FF00 },
			{ COMMON("059"), {0.5f, 0.5f}, 4, 0x00FF00 },
			{ COMMON("05A"), {0.5f, 0.5f}, 2, 0x00FF00 },
			});
	}
	else if (effectKey == EFFECT_CLAIMH_SOLAIS)
	{
		clip.AddFrames({
			{ COMMON("079"), {0.5f, 0.5f}, 1, 0x00FF00 },
			{ COMMON("07A"), {0.5f, 0.5f}, 1, 0x00FF00 },
			{ COMMON("07B"), {0.5f, 0.5f}, 1, 0x00FF00 },
			{ COMMON("07C"), {0.5f, 0.5f}, 1, 0x00FF00 },
			{ COMMON("07D"), {0.5f, 0.5f}, 1, 0x00FF00 },
			{ COMMON("07E"), {0.5f, 0.5f}, 3, 0x00FF00 },
			{ COMMON("07F"), {0.5f, 0.5f}, 2, 0x00FF00 },
			{ COMMON("080"), {0.5f, 0.5f}, 2, 0x00FF00 },
			{ COMMON("081"), {0.5f, 0.5f}, 2, 0x00FF00 },
			{ COMMON("082"), {0.5f, 0.5f}, 2, 0x00FF00 },
			{ COMMON("079"), {0.5f, 0.5f}, 2, 0x00FF00 },
			});
	}
	mAnimator->AddClip(L"effect", clip);
	mAnimator->SetOpcity(opacity);
	mAnimator->SetOrderInLayer(9999);
	mAnimator->Play(L"effect");
}

void Effect::Update(float deltaTime)
{
	
	if (mAnimator && mAnimator->GetState() == GamePlay::AS::AS_STOPPED)
	{
		Destroy();
	}
}
