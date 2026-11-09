#define _USE_DECL_ANNOTATIONS_
#include <windows.h>
#include "./GameCore/Application.h"
#include "./GameCore/GameLoop.h"
#include "./GameCore/SceneManager.h"
#include "./GameCore/InputMgr.h"
#include "./GameCore/AudioLib.h"
#include "./D2DCore/Sprite.h"
#include "./D2DCore/D2DRender.h"
#include "./D2DCore/FontRenderer.h"
#include "./D2DCore/TextureManager.h"
#include "./HelpTool/MyConsole.h"
#include "./HelpTool/INIConfig.h"
#include "./GamePlay/Debug.h"

#include "./GameUI/FloatingNumberPool.h"
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	(void)hPrevInstance;
	(void)lpCmdLine;
	
	HelpTool::MYConsole.ShowConsole();
	SetConsoleOutputCP(CP_UTF8);  // 控制台切到 UTF-8 模式

	do 
	{
		if (!App.Initialize(hInstance, nCmdShow, 768, 576, "AAAAGame"))//"D2D 简单仿Unity组件对象模式2D游戏框架"
		{
			MessageBox(nullptr, TEXT("应用程序初始化失败"), TEXT("初始化信息"), MB_OK | MB_ICONERROR);
			break;
		}

		D2DRd.Initialize();
		D2DSp.Initialize();
		FontMgr.Initialize();
		TexMgr.Initialize();
		Debug.Initialize();
		AudioLib.Initialize();
		GameInput.Initialize();

		{
			D2DCore::Texture* texs[10] = {};
			texs[0] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\07.bmp");
			texs[1] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\08.bmp");
			texs[2] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\09.bmp");
			texs[3] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\10.bmp");
			texs[4] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\11.bmp");
			texs[5] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\12.bmp");
			texs[6] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\13.bmp");
			texs[7] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\14.bmp");
			texs[8] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\15.bmp");
			texs[9] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\16.bmp");
			FLOATINGNUMS.InitWithTextures(texs);
		}
		
		{
			D2DCore::Texture* texs[10] = {};
			texs[0] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\17.bmp");
			texs[1] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\18.bmp");
			texs[2] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\19.bmp");
			texs[3] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\20.bmp");
			texs[4] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\21.bmp");
			texs[5] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\22.bmp");
			texs[6] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\23.bmp");
			texs[7] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\24.bmp");
			texs[8] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\25.bmp");
			texs[9] = TexMgr.LoadTextureWithoutColor(L"resource\\ui\\hud\\26.bmp");
			FLOATINGNUMS_HIT.InitWithTextures(texs);
		}
		
		GameLooper.SetFrameRate(60);
		GameLooper.Run();
	} while (0);

	HelpTool::MYConsole.FreeConsole();
	return 0;
}
