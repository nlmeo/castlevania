#pragma once

#include <windows.h>
#include <d2d1.h>
#include <wincodec.h>
#include <string>

namespace D2DCore
{
	/**
 * @brief 纹理对象类
 *
 * 封装ID2D1Bitmap，表示单个纹理资源
 */
	class Texture
	{
	public:
		/** @brief 构造函数 */
		Texture();

		/** @brief 析构函数 */
		~Texture();

		/** @brief 判断纹理是否有效 */
		bool IsValid() const { return m_pBitmap != nullptr; }

		/** @brief 获取底层D2D Bitmap */
		ID2D1Bitmap* GetD2DBitmap() const { return m_pBitmap; }

		/** @brief 获取纹理宽度 */
		float GetWidth() const { return m_width; }

		/** @brief 获取纹理高度 */
		float GetHeight() const { return m_height; }

		/** @brief 获取纹理文件路径 */
		const std::wstring& GetFilePath() const { return m_filePath; }

		/**
		 * @brief 获取纹理像素数据
		 *
		 * 将纹理位图数据复制到提供的缓冲区
		 *
		 * @param buffer 目标缓冲区（必须足够大，至少 width * height * 4 字节）
		 * @param bufferSize 缓冲区大小（字节）
		 * @param pitch 行字节跨度（通常为 width * 4）
		 * @return 成功返回true，失败返回false
		 */
		bool GetPixelData(uint8_t* buffer, size_t bufferSize, size_t pitch);
	private:
		
	private:
		friend class TextureManager;  ///< 纹理管理器为友元类，可访问私有成员
		friend class D2DRender;       ///< 渲染器为友元类，可访问私有成员

		ID2D1Bitmap* m_pBitmap;     ///< D2D位图接口
		float m_width;              ///< 纹理宽度
		float m_height;             ///< 纹理高度
		std::wstring m_filePath;    ///< 文件路径
	};
}