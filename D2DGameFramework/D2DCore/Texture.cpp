#include "Texture.h"

namespace D2DCore
{
	/**
 * @brief 构造函数
 *
 * 初始化成员变量为默认值
 */
	Texture::Texture()
		: m_pBitmap(nullptr)
		, m_width(0.0f)
		, m_height(0.0f)
	{
	}

	/**
	 * @brief 析构函数
	 *
	 * 释放D2D位图资源
	 */
	Texture::~Texture()
	{
		if (m_pBitmap)
		{
			m_pBitmap->Release();
			m_pBitmap = nullptr;
		}
	}

	/**
	 * @brief 获取纹理像素数据
	 *
	 * @param buffer 目标缓冲区
	 * @param bufferSize 缓冲区大小（字节）
	 * @param pitch 行字节跨度
	 * @return 成功返回true，失败返回false
	 */
	bool Texture::GetPixelData(uint8_t* buffer, size_t bufferSize, size_t pitch)
	{
		if (!m_pBitmap || !buffer || bufferSize == 0 || pitch == 0)
		{
			return false;
		}

		D2D1_SIZE_F size = m_pBitmap->GetSize();
		size_t requiredSize = pitch * static_cast<size_t>(size.height);
		if (bufferSize < requiredSize)
		{
			return false;
		}

		IWICImagingFactory* pWICFactory = nullptr;
		HRESULT hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&pWICFactory)
		);
		if (FAILED(hr))
		{
			return false;
		}

		IWICBitmap* pWICBitmap = nullptr;
		hr = pWICFactory->CreateBitmap(
			static_cast<UINT>(size.width),
			static_cast<UINT>(size.height),
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapCacheOnDemand,
			&pWICBitmap
		);
		if (FAILED(hr))
		{
			pWICFactory->Release();
			return false;
		}

		ID2D1Factory* pD2DFactory = nullptr;
		m_pBitmap->GetFactory(&pD2DFactory);
		if (!pD2DFactory)
		{
			pWICBitmap->Release();
			pWICFactory->Release();
			return false;
		}

		ID2D1RenderTarget* pWicRT = nullptr;
		D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties();
		hr = pD2DFactory->CreateWicBitmapRenderTarget(pWICBitmap, rtProps, &pWicRT);
		pD2DFactory->Release();
		if (FAILED(hr))
		{
			pWICBitmap->Release();
			pWICFactory->Release();
			return false;
		}

		pWicRT->BeginDraw();
		pWicRT->DrawBitmap(m_pBitmap);
		hr = pWicRT->EndDraw();
		pWicRT->Release();
		if (FAILED(hr))
		{
			pWICBitmap->Release();
			pWICFactory->Release();
			return false;
		}

		IWICBitmapLock* pLock = nullptr;
		WICRect rect = { 0, 0, static_cast<int>(size.width), static_cast<int>(size.height) };
		hr = pWICBitmap->Lock(&rect, WICBitmapLockRead, &pLock);
		if (FAILED(hr))
		{
			pWICBitmap->Release();
			pWICFactory->Release();
			return false;
		}

		UINT wicPitch = 0;
		UINT wicBufferSize = 0;
		BYTE* pWicBuffer = nullptr;

		hr = pLock->GetStride(&wicPitch);
		if (SUCCEEDED(hr))
		{
			hr = pLock->GetDataPointer(&wicBufferSize, &pWicBuffer);
		}

		if (FAILED(hr) || !pWicBuffer)
		{
			pLock->Release();
			pWICBitmap->Release();
			pWICFactory->Release();
			return false;
		}

		if (pitch == wicPitch)
		{
			memcpy(buffer, pWicBuffer, requiredSize);
		}
		else
		{
			for (UINT y = 0; y < static_cast<UINT>(size.height); ++y)
			{
				memcpy(buffer + y * pitch, pWicBuffer + y * wicPitch, pitch);
			}
		}

		pLock->Release();
		pWICBitmap->Release();
		pWICFactory->Release();

		return true;
	}
}
