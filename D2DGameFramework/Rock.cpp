#include "Rock.h"
#include "./D2DCore/TextureManager.h"
#include "./HelpTool/RandomMaker.h"


void Rock::Awake()
{
	auto* psr = AddComponent<GamePlay::SpriteRender>();
	psr->SetTexture(TexMgr.LoadTextureWithoutColor(L"./res/texture/csm.bmp",RGB(0,255,0)));
}


void Rock::OnEnable()
{
	//std::cout << name() << "  " << __FUNCTION__ << std::endl;
}

void Rock::Start()
{
	//std::cout << name() << "  " << __FUNCTION__ << std::endl;
	speed = HelpTool::RandomFloat(-180, 180);
	lifetime = HelpTool::RandomFloat(1, 5);
}

void Rock::Update(float deltaTime)
{
	if (!mIsDestroy)
	{
		//livetime += deltaTime;
		//if (livetime < lifetime)
		//{
			//std::cout << name() << "  " << __FUNCTION__ << std::endl;
			transform().rotate(deltaTime * speed);

		//}
		//else
		//{
		//	Destroy();
		//}
	}
}

void Rock::OnDisable()
{
	//std::cout << name() << "  " << __FUNCTION__ << std::endl;
}

void Rock::OnDestroy()
{
	//std::cout << name() << "  " << __FUNCTION__ << std::endl;
}
