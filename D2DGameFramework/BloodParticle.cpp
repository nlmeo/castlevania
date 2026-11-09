#include "BloodParticle.h"
#include "./HelpTool/RandomMaker.h"
#include "./GamePlay/SpriteRender.h"
#include "./D2DCore/TextureManager.h"
#include <cmath>

#define BLOOD_TEX_PATH    L"resource\\effects\\blood\\blood_drop.bmp"
#define FALLBACK_TEX_PATH L"resource\\effects\\common\\frame061.bmp"
#define DEG2RAD (3.1415926535f / 180.0f)

BloodParticle::BloodParticle()
	: mVelocity{ 0.0f, 0.0f }
{
	mpSprite = AddComponent<GamePlay::SpriteRender>();
	mLayer = GamePlay::GameLayer::Layer_6;
}

void BloodParticle::Init(const MathType::vec2f& spawnPos,
	const MathType::vec2f& vel,
	float dissolveTime)
{
	mLifeTime = dissolveTime;
	mElapsed = 0.0f;
	mVelocity = vel;

	// 对标 ZombiPart：重力 800 上下浮动 ±20%（640 ~ 960）
	mGravity = GRAVITY_BASE * (1.0f - GRAVITY_VAR + 2.0f * GRAVITY_VAR * HelpTool::RandomFloat(0.0f, 1.0f));

	// --- 纹理加载（优先血滴图，无则回退色块）---
	D2DCore::Texture* tex = TexMgr.LoadTextureWithoutColor(BLOOD_TEX_PATH, 0x00FF00);
	bool useFallback = (!tex || !tex->IsValid());
	if (useFallback)
		tex = TexMgr.LoadTextureWithoutColor(FALLBACK_TEX_PATH, 0x00FF00);
	if (tex && tex->IsValid())
		mpSprite->SetTexture(tex);

	// --- 随机初始缩放 ---
	float randScale = 0.6f + 0.4f * HelpTool::RandomFloat(0.0f, 1.0f);
	mInitialScale = useFallback ? randScale * 0.25f : randScale;
	mFinalScale = mInitialScale * 0.15f;

	transform().SetPosition(spawnPos);
	transform().SetScale({ mInitialScale, mInitialScale });
	mpSprite->SetOpcity(0.85f);
	mpSprite->SetAnchor(0.5f, 0.5f);
	_ComponentUpdate(0.0f);
}

void BloodParticle::Update(float deltaTime)
{
	mElapsed += deltaTime;
	if (mElapsed >= mLifeTime)
	{
		DestroyThis();
		return;
	}

	// 重力加速度（与 ZombiPart 一致的坐标系：y 正=下）
	mVelocity.y += mGravity * deltaTime;

	MathType::vec2f pos = transform().position();
	pos += mVelocity * deltaTime;
	transform().SetPosition(pos);

	float t = mElapsed / mLifeTime;
	//float curScale = mInitialScale + (mFinalScale - mInitialScale) * t;
	
	float curScale = transform().localScale().x - deltaTime;
	curScale = curScale < mFinalScale ? mFinalScale : curScale;

	transform().SetScale({ curScale, curScale });

	// 二次缓出透明衰减
	float alpha = 1.0f - t * t;
	mpSprite->SetOpcity(alpha * 0.85f);
	_ComponentUpdate(0.0f);
}

void BloodParticle::Emit(const MathType::vec2f& spawnPos,
	const MathType::vec2f& baseDir,
	int count)
{
	if (count <= 0) return;

	MathType::vec2f dir = baseDir.normalized();

	float curAngle = MathType::vec2f::right().SignedAngleDegree(dir);

	


	for (int i = 0; i < count; ++i)
	{
		float vx = HelpTool::RandomFloat(VX_MIN, VX_MAX);
		float vy = HelpTool::RandomFloat(VY_MIN, VY_MAX);
		float atkAngle = curAngle;
		//将夹角随机正负半角后计算新的方向向量
		float randomAnglePlus = HelpTool::RandomFloat(0.0f, 2 * SCATTER_ANGLE) - SCATTER_ANGLE;
		atkAngle += randomAnglePlus;
		float dx = cos(atkAngle * DEG2RAD);
		float dy = -sinf(atkAngle * DEG2RAD);//非笛卡尔坐标系
		//将速度乘以方向向量后得到自身速度
		MathType::vec2f vel;
		vel.x = vx * dx;
		vel.y = vy * dy;
		
		// 消散时间 0.8 ~ 1.2s
		float life = HelpTool::RandomFloat(LIFE_MIN, LIFE_MAX);

		auto particle = GamePlay::GameObject::CreateGameObject<BloodParticle>();
		particle->Init(spawnPos, vel, life);
	}
}

void BloodParticle::DestroyThis()
{
	SetActive(false);
	Destroy();
}
