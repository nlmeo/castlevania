#include "AfterImageGhost.h"
#include "SpriteRender.h"
#include "../D2DCore/Texture.h"
#include "../D2DCore/TextureManager.h"
#include "../HelpTool/EnCoding.h"
#include <string>
#include "../../ThirdParty/FreeImage/include/FreeImage.h"

#ifndef _WIN64
#pragma comment(lib,"../ThirdParty/FreeImage/bin/x86/FreeImage.lib")
#else
#pragma comment(lib,"../ThirdParty/FreeImage/bin/x64/FreeImage.lib")
#endif

namespace GamePlay
{
	int AfterImageGhost::s_tintId = 0;

	AfterImageGhost::AfterImageGhost()
	{
		SetDestroyOnSceneExit();
		mLayer = GamePlay::GameLayer::Layer_3;
		auto a = AddComponent<GamePlay::SpriteRender>();
		a->SetOrderInLayer(GameObjectMgr.GameObjectCount());
		
		
	}

	void AfterImageGhost::Awake() {}

	void AfterImageGhost::Update(float deltaTime)
	{
		if (!Active()) return;

		m_elapsed += deltaTime;
		if (m_elapsed >= m_lifeTime)
		{
			Destroy();
			return;
		}

		float alpha = 1.0f - (m_elapsed / m_lifeTime);
		GetComponent<GamePlay::SpriteRender>()->SetOpcity(alpha * 0.5f);
	}

	void AfterImageGhost::Init(D2DCore::Texture* tex, const MathType::vec2f& anchor,
		const MathType::vec2f& position, float scaleX, float scaleY,
		float lifeTime)
	{
		m_lifeTime = lifeTime;


		
		D2DCore::Texture* tintedTex = CreateTintedTexture(tex);
		auto* sr = GetComponent<GamePlay::SpriteRender>();
		sr->SetTexture(tintedTex);
		sr->SetAnchor(anchor.x, anchor.y);
		sr->SetOpcity(0.8f);

		transform().SetPosition(position);
		transform().SetScale({ scaleX, scaleY });
	}

	D2DCore::Texture* AfterImageGhost::CreateTintedTexture(D2DCore::Texture* src)
	{
		if (!src || !src->IsValid()) return src;

		const std::wstring& filePath = src->GetFilePath();
		if (!filePath.empty())
		{
			std::wstring key = filePath + L"_ghost";
			D2DCore::Texture* result = TexMgr.GetTexture(key.c_str());
			if (!result)
			{
				result = LoadAndTintFromFile(filePath);
			}
			 
			if (result && result->IsValid()) return result;
		}

		return src;
	}

	D2DCore::Texture* AfterImageGhost::LoadAndTintFromFile(const std::wstring& filePath)
	{
		std::string imgname = HelpTool::EnCodeing::W2C(filePath.c_str());

		FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(imgname.c_str(), 0);
		if (fif == FIF_UNKNOWN)
			fif = FreeImage_GetFIFFromFilename(imgname.c_str());
		if (fif == FIF_UNKNOWN || !FreeImage_FIFSupportsReading(fif))
			return nullptr;

		FIBITMAP* dib = FreeImage_Load(fif, imgname.c_str());
		if (!dib) return nullptr;

		FreeImage_FlipVertical(dib);

		unsigned int width = FreeImage_GetWidth(dib);
		unsigned int height = FreeImage_GetHeight(dib);
		if (width == 0 || height == 0) { FreeImage_Unload(dib); return nullptr; }

		int bpp = FreeImage_GetBPP(dib);
		if (bpp != 32)
		{
			FIBITMAP* converted = FreeImage_ConvertTo32Bits(dib);
			FreeImage_Unload(dib);
			dib = converted;
			bpp = FreeImage_GetBPP(dib);
		}

		BYTE* bits = FreeImage_GetBits(dib);
		if (!bits) { FreeImage_Unload(dib); return nullptr; }

		uint32_t colorKey = 0x0000FF00; // 标准绿幕色键
		size_t pixelCount = (size_t)width * height;

		for (size_t i = 0; i < pixelCount; ++i)
		{
			BYTE* p = bits + i * 4; // BGRA: [B, G, R, A]
			uint32_t curColor = *(uint32_t*)p & 0x00FFFFFF;

			if (curColor == colorKey)
			{
				p[3] = 0; // 透明
			}

			// 预乘 Alpha
			float a = p[3] / 255.0f;
			p[0] = (BYTE)(p[0] * a); // B
			p[1] = (BYTE)(p[1] * a); // G
			p[2] = (BYTE)(p[2] * a); // R

			// 蓝色调：B*1.0, G*0.4, R*0.2
			//p[0] = (BYTE)(p[0] * 1.0f); // B → 减弱（预乘后 B 偏大，用 0.2f 补偿）
			//p[1] = (BYTE)(p[1] * 0.4f); // G
			//p[2] = (BYTE)(p[2] * 0.4f); // R
			p[0] = (p[0] == 0) ? 0 :(BYTE)(255); //255 *a  B → 减弱（预乘后 B 偏大，用 0.2f 补偿）
			p[1] = (BYTE)(p[1]* 0.8); // G
			p[2] = (BYTE)(p[2]* 0.8); // R
		}

		std::wstring key = filePath + L"_ghost";
		D2DCore::Texture* result = TexMgr.CreateTextureFromMemory(key.c_str(),
			(uint8_t*)bits, (int)width, (int)height, (int)width * 4);

		FreeImage_Unload(dib);
		return result;
	}
}
