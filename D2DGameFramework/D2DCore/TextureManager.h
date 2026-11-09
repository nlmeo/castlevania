#pragma once

#include <windows.h>
#include <d2d1.h>
#include <wincodec.h>
#include <unordered_map>
#include <string>
#include "Texture.h"
#include "../HelpTool/SingletonTemplate.h"


namespace D2DCore
{
	/**
 * @brief 纹理管理器类
 *
 * 通过WIC (Windows Imaging Component) 加载PNG/BMP/JPG图片，管理纹理生命周期
 */
	class TextureManager :public Singleton<TextureManager>
	{
		PRIVATE_COPY(TextureManager)
	public:
		/** @brief 构造函数 */
		TextureManager();

		/** @brief 析构函数 */
		~TextureManager();

		/**
		 * @brief 初始化纹理管理器
		 *
		 * @param pRenderTarget D2D渲染目标
		 * @return 初始化成功返回true，失败返回false
		 */
		bool Initialize();
		/**
		 * @brief 加载纹理（自动缓存，避免重复加载）
		 *
		 * @param filePath 纹理文件路径
		 * @return 成功返回Texture指针，失败返回nullptr
		 */
		Texture* LoadTexture(const wchar_t* filePath);
		/**
		  * @brief 加载纹理（自动缓存，避免重复加载）
		  *
		  * @param textureKey 纹理缓存键
		  * @param filePath 纹理文件路径
		  * @return 成功返回Texture指针，失败返回nullptr
		  */
		Texture* LoadTexture(const wchar_t* textureKey, const wchar_t* filePath);
		/**
		 * @brief 加载纹理（自动缓存，避免重复加载）
		 *
		 * @param filePath 纹理文件路径
		 * @param colorKey   透明色键（RGB格式，例如0xFF00FF表示纯品红）
		 * @return 成功返回Texture指针，失败返回nullptr
		 */
		Texture* LoadTextureWithoutColor(const wchar_t* filePath, int colorKey = 0x00FF00);
		/**
		 * @brief 加载纹理（自动缓存，避免重复加载）
		 *
		 * @param textureKey 纹理缓存键
		 * @param filePath 纹理文件路径
		 * @param colorKey   透明色键（RGB格式，例如0xFF00FF表示纯品红）
		 * @return 成功返回Texture指针，失败返回nullptr
		 */
		Texture* LoadTextureWithoutColor(const wchar_t* textureKey, const wchar_t* filePath, int colorKey = 0x00FF00);

		bool LoadTextureFromFolder(const wchar_t* folderPath, int colorKey = 0x00FF00);

		bool LoadTextureFromFolderS(const std::vector<std::pair<const wchar_t*, int>>& loadTasks);

		/**
		 * @brief 获取已加载的纹理
		 *
		 * @param textureKey 纹理缓存键
		 * @return 成功返回Texture指针，不存在返回nullptr
		 */
		Texture* GetTexture(const wchar_t* textureKey);
		/**
		 * @brief 卸载指定纹理
		 *
		 * @param textureKey 纹理缓存键
		 */
		void UnloadTexture(const wchar_t* textureKey);

		/** @brief 卸载所有纹理 */
		void UnloadAll();

		/**
		 * @brief 从位图数据创建纹理
		 *
		 * @param key 纹理缓存键（用于后续GetTexture查找）
		 * @param pixelData 像素数据缓冲区
		 * @param width 纹理宽度（像素）
		 * @param height 纹理高度（像素）
		 * @param pitch 行字节跨度
		 * @return 成功返回Texture指针，失败返回nullptr
		 */
		Texture* CreateTextureFromMemory(const wchar_t* key, const uint8_t* pixelData, int width, int height, int pitch);

	private:
		ID2D1RenderTarget* m_pRenderTarget;           ///< D2D渲染目标
		IWICImagingFactory* m_pWICFactory;            ///< WIC工厂接口
		std::unordered_map<std::wstring, Texture*> m_textureCache;  ///< 纹理缓存
	};
#define TexMgr D2DCore::TextureManager::Instance()
#define LoadTexWithOutColor(a,b) D2DCore::TextureManager::Instance().LoadTextureWithoutColor(a,b)
}
