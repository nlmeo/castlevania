#include "BufferBlock.h"
#include "D2DCore/TextureManager.h"

void BufferBlock::Init(GameUI::UICanvas* p)
{
	mAvailableBufferBar.resize(20);
	mActiveBufferBar.resize(20);

	GameUI::UIImage* pIMG;
	for (int i = 0; i < 20; ++i)
	{
		pIMG = new GameUI::UIImage();
		pIMG->SetTexture(TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\hpbufferbar.png"));
		pIMG->SetOpacity(0);
		pIMG->SetActive(false);
		p->AddChild(pIMG);
		mAvailableBufferBar[i] = pIMG;
	}
	mIsInit = true;
}

void BufferBlock::Update(float deltaTime)
{
	if (!mIsInit) return;

	std::vector<GameUI::UIImage*>::iterator it = mActiveBufferBar.begin();
	GameUI::UIImage* pIMG;
	while (it != mActiveBufferBar.end())
	{
		pIMG = *it;
		if (pIMG == nullptr)
		{
			++it;
			continue;
		}

		if (pIMG->GetOpacity() <= 0)
		{
			pIMG->SetActive(false);
			mAvailableBufferBar.push_back(pIMG);
			it = mActiveBufferBar.erase(it);
			continue;
		}
		else
		{
			MathType::vec2f curPos = pIMG->GetPosition();
			//MathType::vec2f dest = curPos;
			//dest.y += deltaTime * 60;

			//dest = curPos.Lerp(dest, deltaTime * 40);

			curPos.y += deltaTime * 80;
			float curOpacity = pIMG->GetOpacity();
			curOpacity -= deltaTime * 1.5f;
			curOpacity = curOpacity < 0 ? 0 : curOpacity;

			pIMG->SetPosition(curPos);
			pIMG->SetOpacity(curOpacity);



		}
		++it;
	}
	pIMG = nullptr;
}

void BufferBlock::Clear()
{
	for (auto* image : mActiveBufferBar)
	{
		if (!image) continue;
		image->SetActive(false);
		image->SetOpacity(0);
		mAvailableBufferBar.push_back(image);
	}
	mActiveBufferBar.clear();
}

void BufferBlock::Spawn(float posX, float posY, float width, float height)
{
	GameUI::UIImage* pIMG = mAvailableBufferBar.back();
	mAvailableBufferBar.pop_back();

	pIMG->SetOpacity(1);
	pIMG->SetPosition(posX, posY);
	pIMG->SetSize(width, height);
	pIMG->SetActive(true);
	
	mActiveBufferBar.push_back(pIMG);

}
