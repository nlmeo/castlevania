#include "S_Fly.h"
#include "GameTool.h"
#include "Player.h"
#include "Soma.h"
void S_Fly::Init()
{
	mSoulId = 1;
	mInputType = 2; //触发类型： 1-开关类型 2-持续按压
	mRunType = 1;   //运行类型: 1-不需要接管角色状态（附加效果） 2-临时角色状态（类似枪兵旋转技能）
	mLayer = GamePlay::GameLayer::Layer_3;
	int order = 999;
	a1 = GT::DrawPicture(L"resource\\skills\\p_fly\\frame000.bmp", { 0,0 }, order++, { 0,0 }, 0x00FF00, nullptr, GamePlay::GameLayer::Layer_3);
	a1->SetOpcity(0);

	a3 = GT::DrawPicture(L"resource\\skills\\p_fly\\frame002.bmp", { 0,0 }, order++, { 0.68f,0.60f }, 0x00FF00, nullptr, GamePlay::GameLayer::Layer_3);
	a3->SetOpcity(0);

	a2 = GT::DrawPicture(L"resource\\skills\\p_fly\\frame001.bmp", { 0,0 }, order++, { 0,0 }, 0x00FF00, nullptr, GamePlay::GameLayer::Layer_3);
	a2->SetOpcity(0);
	

	GamePlay::AnimationClip clip;
	clip.m_isLoop = true;
	clip.AddFrames({
			{ L"resource\\skills\\p_fly\\frame003.bmp",{0,0},6, 0x00FF00 },
			{ L"resource\\skills\\p_fly\\frame004.bmp",{0,0},6, 0x00FF00 },
			{ L"resource\\skills\\p_fly\\frame005.bmp",{0,0},6, 0x00FF00 },
			{ L"resource\\skills\\p_fly\\frame006.bmp",{0,0},6, 0x00FF00 },
			{ L"resource\\skills\\p_fly\\frame007.bmp",{0,0},6, 0x00FF00 },
			{ L"resource\\skills\\p_fly\\frame005.bmp",{0,0},6, 0x00FF00 },
			{ L"resource\\skills\\p_fly\\frame004.bmp",{0,0},6, 0x00FF00 },
		});
	a4 = GT::PlayAnimation(clip, L"idle", ++order);
	a4->gameobject()->layer() = GamePlay::GameLayer::Layer_3;
	a4->SetOpcity(0);
	SetActive(false);

}

void S_Fly::Start()
{

}

void S_Fly::Update(float deltaTime)
{
	if (MainPlayer == nullptr) { return; }
	
	Soma* pSoma = dynamic_cast<Soma*>(MainPlayer);
	float curScaleX = pSoma->transform().localScale().x;
	MathType::vec2f curPos = pSoma->transform().position();
	float curPlus = deltaTime * 2;
	MathType::vec2f curScale = { 1,1 };
	if (curScaleX < 0) { curScale.x = -1; }

	a1->transform()->SetPosition({ curPos.x - 150 * curScale.x,curPos.y - 150 });
	a2->transform()->SetPosition({ curPos.x - 250 * curScale.x,curPos.y - 300 });
	a3->transform()->SetPosition({ curPos.x - 120 * curScale.x,curPos.y - 100 });
	a4->transform()->SetPosition({ curPos.x - 150 * curScale.x,curPos.y - 150 });
	a1->transform()->SetScale(curScale);
	a2->transform()->SetScale(curScale);
	a3->transform()->SetScale(curScale);
	a4->transform()->SetScale(curScale);
	

	float curAngleR;
	auto attr = pSoma->GetAttributes();
	
	if ((attr.curjp < 0 && !pSoma->OnGround()) || attr.cursp > 0)
	{
		curAngleR = mRotation + deltaTime * 50;
		if (curAngleR > 30)
			curAngleR = 30;
		
	}
	else
	{
		curAngleR = mRotation - deltaTime * 50;
		if (curAngleR < 0)
			curAngleR = 0;
	}
	mRotation = curAngleR;
	a3->transform()->SetRotation(curScaleX < 0 ? -curAngleR : curAngleR);
	


	Attributes& a = pSoma->GetAttributes();
	if (GT::KeyDown('K'))
	{
		a.g = a.dy == 2 ? 0.3f : 3.0f;
		float curOpacity = a1->Opacity() + curPlus;
		a1->SetOpcity(curOpacity > 0.5f ? 0.5f : curOpacity);
		
		curOpacity = a2->Opacity() + curPlus;
		a2->SetOpcity(curOpacity > 1 ? 1 : curOpacity);
		
		curOpacity = a3->Opacity() + curPlus;
		a3->SetOpcity(curOpacity > 1 ? 1 : curOpacity);
		
		curOpacity = a4->Opacity() + curPlus;
		a4->SetOpcity(curOpacity > 1 ? 1 : curOpacity);

	}
	else
	{
		a.g = 3.0f;
		float curOpacity = a1->Opacity() - curPlus;
		a1->SetOpcity(curOpacity <  0 ? 0 : curOpacity);

		curOpacity = a2->Opacity() - curPlus;
		a2->SetOpcity(curOpacity < 0 ? 0 : curOpacity);

		curOpacity = a3->Opacity() - curPlus;
		a3->SetOpcity(curOpacity < 0 ? 0 : curOpacity);

		curOpacity = a4->Opacity() - curPlus;
		a4->SetOpcity(curOpacity < 0 ? 0 : curOpacity);
	}

}

void S_Fly::OnDestroy()
{
	// a1/a2/a3 是 GT::DrawPicture 创建的独立 GameObject，需手动销毁
	// 直接用 Destroy() 而非 SAFE_DESTROY，因为 gameobject() 返回的是临时值
	if (a1) { a1->gameobject()->Destroy(); a1 = nullptr; }
	if (a2) { a2->gameobject()->Destroy(); a2 = nullptr; }
	if (a3) { a3->gameobject()->Destroy(); a3 = nullptr; }
	// a4 是 AddComponent 添加的组件，随 S_Fly 析构自动 delete（GameObject::~GameObject 会遍历 mComponents 逐个 delete）
	a4 = nullptr;
}
