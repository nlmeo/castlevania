#include <filesystem>
#include "TextureManager.h"
#include "D2DRender.h"
#include "../TypeDefine/Color.h"
#include "../HelpTool/EnCoding.h"
#include "../../ThirdParty/FreeImage/include/FreeImage.h"
#ifndef _WIN64
#pragma comment(lib,"../ThirdParty/FreeImage/bin/x86/FreeImage.lib")
#else
#pragma comment(lib,"../ThirdParty/FreeImage/bin/x64/FreeImage.lib")
#endif


namespace D2DCore
{
	/**
 * @brief 构造函数
 *
 * 初始化成员变量为默认值
 */
	TextureManager::TextureManager()
		: m_pRenderTarget(nullptr)
		, m_pWICFactory(nullptr)
	{
#ifdef FREEIMAGE_LIB
		FreeImage_Initialise();//只有使用静态库的时候要进行这样的初始化
#endif
	}

	/**
	 * @brief 析构函数
	 *
	 * 卸载所有纹理并释放WIC工厂
	 */
	TextureManager::~TextureManager()
	{
		UnloadAll();
		//SAFE_RELEASE(m_pWICFactory);
	}

	/**
	 * @brief 初始化纹理管理器
	 *
	 * 创建WIC工厂实例
	 *
	 * @param pRenderTarget D2D渲染目标
	 * @return 初始化成功返回true，失败返回false
	 */
	bool TextureManager::Initialize()
	{

		if (!D2DRd.IsInit())
		{
			MessageBox(nullptr,
				TEXT("请保证 D2DRender::Initialize 函数优先初始化成功后再调用 TextureManager::Initialize 函数!"),
				TEXT("D2DFontRender初始化提示!"), MB_OK);
			return false;
		}
		m_pRenderTarget = D2DRd.GetD2DRenderTarget();

		// 创建WIC工厂
		HRESULT hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&m_pWICFactory)
		);

		return SUCCEEDED(hr);
	}
	/**
	 * @brief 加载纹理（自动缓存，避免重复加载）
	 *
	 * 通过WIC解码图片文件，转换为D2D可用的格式后创建Bitmap
	 *
	 * @param filePath 纹理文件路径
	 * @return 成功返回Texture指针，失败返回nullptr
	 */
	Texture* TextureManager::LoadTexture(const wchar_t* filePath)
	{
		std::wstring path(filePath);

		// 检查缓存中是否已存在
		auto it = m_textureCache.find(path);
		if (it != m_textureCache.end())
		{
			return it->second;
		}

		// 检查必要的资源是否可用
		if (!m_pWICFactory || !m_pRenderTarget)
		{
			return nullptr;
		}

		// 创建WIC解码器
		IWICBitmapDecoder* pDecoder = nullptr;
		HRESULT hr = m_pWICFactory->CreateDecoderFromFilename(
			filePath,
			nullptr,
			GENERIC_READ,
			WICDecodeMetadataCacheOnLoad,
			&pDecoder
		);

		if (FAILED(hr))
		{
			return nullptr;
		}

		// 获取解码后的第一帧
		IWICBitmapFrameDecode* pFrame = nullptr;
		hr = pDecoder->GetFrame(0, &pFrame);
		if (FAILED(hr))
		{
			SAFE_RELEASE(pDecoder);
			return nullptr;
		}

		// 创建格式转换器
		IWICFormatConverter* pConverter = nullptr;
		hr = m_pWICFactory->CreateFormatConverter(&pConverter);
		if (FAILED(hr))
		{
			SAFE_RELEASE(pFrame);
			SAFE_RELEASE(pDecoder);
			return nullptr;
		}

		// 将图片转换为D2D原生格式 (32bppPBGRA)
		hr = pConverter->Initialize(
			pFrame,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			nullptr,
			0.0f,
			WICBitmapPaletteTypeMedianCut
		);

		SAFE_RELEASE(pDecoder);

		if (FAILED(hr))
		{
			SAFE_RELEASE(pFrame);
			SAFE_RELEASE(pConverter);
			return nullptr;
		}

		// 从WIC位图创建D2D位图
		ID2D1Bitmap* pBitmap = nullptr;
		hr = m_pRenderTarget->CreateBitmapFromWicBitmap(pConverter, nullptr, &pBitmap);

		SAFE_RELEASE(pFrame);
		SAFE_RELEASE(pConverter);

		if (FAILED(hr))
		{
			return nullptr;
		}

		// 创建Texture对象并填充信息
		Texture* texture = new Texture();
		texture->m_pBitmap = pBitmap;
		texture->m_filePath = path;
		D2D1_SIZE_F size = pBitmap->GetSize();
		texture->m_width = size.width;
		texture->m_height = size.height;

		// 添加到缓存
		m_textureCache[path] = texture;
		return texture;
	}

	Texture* TextureManager::LoadTexture(const wchar_t* textureKey, const wchar_t* filePath)
	{
		std::wstring path(filePath);

		// 检查缓存中是否已存在
		auto it = m_textureCache.find(path);
		if (it != m_textureCache.end())
		{
			return it->second;
		}

		// 检查必要的资源是否可用
		if (!m_pWICFactory || !m_pRenderTarget)
		{
			return nullptr;
		}

		// 创建WIC解码器
		IWICBitmapDecoder* pDecoder = nullptr;
		HRESULT hr = m_pWICFactory->CreateDecoderFromFilename(
			filePath,
			nullptr,
			GENERIC_READ,
			WICDecodeMetadataCacheOnLoad,
			&pDecoder
		);

		if (FAILED(hr))
		{
			return nullptr;
		}

		// 获取解码后的第一帧
		IWICBitmapFrameDecode* pFrame = nullptr;
		hr = pDecoder->GetFrame(0, &pFrame);
		if (FAILED(hr))
		{
			SAFE_RELEASE(pDecoder);
			return nullptr;
		}

		// 创建格式转换器
		IWICFormatConverter* pConverter = nullptr;
		hr = m_pWICFactory->CreateFormatConverter(&pConverter);
		if (FAILED(hr))
		{
			SAFE_RELEASE(pFrame);
			SAFE_RELEASE(pDecoder);
			return nullptr;
		}

		// 将图片转换为D2D原生格式 (32bppPBGRA)
		hr = pConverter->Initialize(
			pFrame,
			GUID_WICPixelFormat32bppPBGRA,
			WICBitmapDitherTypeNone,
			nullptr,
			0.0f,
			WICBitmapPaletteTypeMedianCut
		);

		SAFE_RELEASE(pDecoder);

		if (FAILED(hr))
		{
			SAFE_RELEASE(pFrame);
			SAFE_RELEASE(pConverter);
			return nullptr;
		}

		// 从WIC位图创建D2D位图
		ID2D1Bitmap* pBitmap = nullptr;
		hr = m_pRenderTarget->CreateBitmapFromWicBitmap(pConverter, nullptr, &pBitmap);

		SAFE_RELEASE(pFrame);
		SAFE_RELEASE(pConverter);

		if (FAILED(hr))
		{
			return nullptr;
		}

		// 创建Texture对象并填充信息
		Texture* texture = new Texture();
		texture->m_pBitmap = pBitmap;
		texture->m_filePath = path;
		D2D1_SIZE_F size = pBitmap->GetSize();
		texture->m_width = size.width;
		texture->m_height = size.height;

		// 添加到缓存
		m_textureCache[path] = texture;
		return texture;
	}

	Texture* TextureManager::LoadTextureWithoutColor(const wchar_t* filePath, int colorKey)
	{
		if (filePath == nullptr) { return nullptr; }
		std::wstring path(filePath);

		// 检查缓存中是否已存在
		auto it = m_textureCache.find(path);
		if (it != m_textureCache.end())
		{
			return it->second;
		}

		//定义了一个图片格式（FREE_IMAGE_FORMAT） 的变量 fif;
		FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
		//如果图片成功加载，那么dib就是一个指向图片的指针变量
		FIBITMAP* dib = nullptr;
		//指向图片数据的指针变量
		BYTE* bits = nullptr;
		//定义图片的宽高
		unsigned int width = 0, height = 0;

		std::string imgname = HelpTool::EnCodeing::W2C(filePath);

		//尝试从文件头签名获取图片的格式
		fif = FreeImage_GetFileType(imgname.c_str(), 0);
		//如果上述调用失败，那么尝试从文件扩展名猜测图片的格式
		if (fif == FIF_UNKNOWN)
			fif = FreeImage_GetFIFFromFilename(imgname.c_str());
		//如果还是不能得到受支持的格式，返回假
		if (fif == FIF_UNKNOWN)
			return nullptr;

		//检查一下读到的格式是否被支持读取，成功就加载
		if (FreeImage_FIFSupportsReading(fif))
			dib = FreeImage_Load(fif, imgname.c_str());
		//如果加载失败，返回假
		if (!dib)
			return nullptr;

		//根据传入的参数判断是否需要将图片的数据进行颠倒
		//if (FlipVertical)
		//{
		//    //FreeImage_FlipHorizontal(dib);//水平翻转
		FreeImage_FlipVertical(dib);//垂直翻转
		//}

		//获取图片的数据
		bits = FreeImage_GetBits(dib);
		//获取图片的宽高信息
		width = FreeImage_GetWidth(dib);
		height = FreeImage_GetHeight(dib);

		//上述三个信息任一一个为0就都返回假
		if ((bits == 0) || (width == 0) || (height == 0))
		{
			FreeImage_Unload(dib);//因为Load操作是成功的，所以这里要释放
			return nullptr;
		}

		//获取图片的位深度
		int bpp = FreeImage_GetBPP(dib);
		if (bpp != 32)
		{
			//将图片转换为32位格式
			dib = FreeImage_ConvertTo32Bits(dib);
			bpp = FreeImage_GetBPP(dib);
			bits = FreeImage_GetBits(dib);
		}

		int curColor;

		ColorType::LPCOLOR32  pLine = nullptr;
		ColorType::LPCOLOR32 pColor = nullptr;
		for (int y = 0; y < height; ++y)
		{
			pLine = (ColorType::LPCOLOR32)(bits + y * width * 4);
			for (int x = 0; x < width; ++x)
			{
				pColor = pLine + x;
				curColor = *pColor;
				if (curColor == colorKey)
				{
					pColor->a = 0;
				}
				pColor->r = pColor->r * pColor->a / 255.0f;
				pColor->g = pColor->g * pColor->a / 255.0f;
				pColor->b = pColor->b * pColor->a / 255.0f;
			}
		}


		Texture* texture = CreateTextureFromMemory(filePath, (uint8_t*)bits, width, height, width * 4);

		FreeImage_Unload(dib);
		dib = nullptr;
		bits = nullptr;

		return texture;
	}

	Texture* TextureManager::LoadTextureWithoutColor(const wchar_t* textureKey, const wchar_t* filePath, int colorKey)
	{
		if (filePath == nullptr) { return nullptr; }
		std::wstring path(filePath);

		// 检查缓存中是否已存在
		auto it = m_textureCache.find(path);
		if (it != m_textureCache.end())
		{
			return it->second;
		}

		//定义了一个图片格式（FREE_IMAGE_FORMAT） 的变量 fif;
		FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
		//如果图片成功加载，那么dib就是一个指向图片的指针变量
		FIBITMAP* dib = nullptr;
		//指向图片数据的指针变量
		BYTE* bits = nullptr;
		//定义图片的宽高
		unsigned int width = 0, height = 0;

		std::string imgname = HelpTool::EnCodeing::W2C(filePath);

		//尝试从文件头签名获取图片的格式
		fif = FreeImage_GetFileType(imgname.c_str(), 0);
		//如果上述调用失败，那么尝试从文件扩展名猜测图片的格式
		if (fif == FIF_UNKNOWN)
			fif = FreeImage_GetFIFFromFilename(imgname.c_str());
		//如果还是不能得到受支持的格式，返回假
		if (fif == FIF_UNKNOWN)
			return nullptr;

		//检查一下读到的格式是否被支持读取，成功就加载
		if (FreeImage_FIFSupportsReading(fif))
			dib = FreeImage_Load(fif, imgname.c_str());
		//如果加载失败，返回假
		if (!dib)
			return nullptr;

		//根据传入的参数判断是否需要将图片的数据进行颠倒
		//if (FlipVertical)
		//{
		//    //FreeImage_FlipHorizontal(dib);//水平翻转
		FreeImage_FlipVertical(dib);//垂直翻转
		//}

		//获取图片的数据
		bits = FreeImage_GetBits(dib);
		//获取图片的宽高信息
		width = FreeImage_GetWidth(dib);
		height = FreeImage_GetHeight(dib);

		//上述三个信息任一一个为0就都返回假
		if ((bits == 0) || (width == 0) || (height == 0))
		{
			FreeImage_Unload(dib);//因为Load操作是成功的，所以这里要释放
			return nullptr;
		}

		//获取图片的位深度
		int bpp = FreeImage_GetBPP(dib);
		if (bpp != 32)
		{
			//将图片转换为32位格式
			dib = FreeImage_ConvertTo32Bits(dib);
			bpp = FreeImage_GetBPP(dib);
			bits = FreeImage_GetBits(dib);
		}

		int curColor;

		ColorType::LPCOLOR32  pLine = nullptr;
		ColorType::LPCOLOR32 pColor = nullptr;
		for (int y = 0; y < height; ++y)
		{
			pLine = (ColorType::LPCOLOR32)(bits + y * width * 4);
			for (int x = 0; x < width; ++x)
			{
				pColor = pLine + x;
				curColor = *pColor;
				if (curColor == colorKey)
				{
					pColor->a = 0;
				}
				pColor->r = pColor->r * pColor->a / 255.0f;
				pColor->g = pColor->g * pColor->a / 255.0f;
				pColor->b = pColor->b * pColor->a / 255.0f;
			}
		}


		Texture* texture = CreateTextureFromMemory(textureKey, (uint8_t*)bits, width, height, width * 4);

		FreeImage_Unload(dib);
		dib = nullptr;
		bits = nullptr;

		return texture;
	}

	bool TextureManager::LoadTextureFromFolder(const wchar_t* folderPath, int colorKey)
	{
		//创建文件夹迭代器
		std::filesystem::directory_iterator it(folderPath); //todo:如果路径未找到不使用try catch捕获程序会崩溃
		std::filesystem::directory_iterator end_it; //流式迭代器，默认构造即为结束符

		while (it != end_it)
		{
			const std::filesystem::directory_entry entry = *it;
			// 只处理普通文件
			if (entry.is_regular_file())
			{
				const std::filesystem::path p = entry.path();
				std::wstring file_stem = p.stem().wstring(); //不带后缀文件名字
				std::wstring file_name = p.filename().wstring(); //带后缀文件名
				std::wstring file_ext = p.extension().wstring(); //后缀

				// 只加载bmp文件
				if (file_ext == L".bmp" || file_ext == L".png")
				{
					std::wstring textureKey = folderPath + file_name;

					if (LoadTextureWithoutColor(textureKey.c_str(), colorKey) == nullptr)
					{
						int a = 2;
						return false;
					}
				}
			}

			++it;
			
		}
		return true;
	}

	bool TextureManager::LoadTextureFromFolderS(const std::vector<std::pair<const wchar_t*, int>>& loadTasks)
	{

		for (const auto task : loadTasks)
		{

			// task.first  = 文件夹路径
			// task.second = 颜色键
			if (!LoadTextureFromFolder(task.first, task.second))
			{
				int a = 1;
				return false;
			}
		}

		return true;
	}


	Texture* TextureManager::GetTexture(const wchar_t* textureKey)
	{
		std::wstring key(textureKey);
		auto it = m_textureCache.find(key);
		if (it != m_textureCache.end())
		{
			return it->second;
		}
		return nullptr;
	}


	void TextureManager::UnloadTexture(const wchar_t* textureKey)
	{
		std::wstring key(textureKey);
		auto it = m_textureCache.find(key);
		if (it != m_textureCache.end())
		{
			delete it->second;
			m_textureCache.erase(it);
		}
	}

	/**
	 * @brief 卸载所有纹理
	 */
	void TextureManager::UnloadAll()
	{
		if (!m_textureCache.empty())
		{
			for (auto& pair : m_textureCache)
			{
				delete pair.second;
			}
			m_textureCache.clear();
		}
	}

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
	Texture* TextureManager::CreateTextureFromMemory(const wchar_t* key, const uint8_t* pixelData, int width, int height, int pitch)
	{
		if (!key || !pixelData || width <= 0 || height <= 0)
		{
			return nullptr;
		}

		std::wstring cacheKey(key);

		auto it = m_textureCache.find(cacheKey);
		if (it != m_textureCache.end())
		{
			return it->second;
		}

		if (!m_pRenderTarget)
		{
			return nullptr;
		}

		D2D1_BITMAP_PROPERTIES bitmapProperties = D2D1::BitmapProperties(
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
		);

		ID2D1Bitmap* pBitmap = nullptr;
		HRESULT hr = m_pRenderTarget->CreateBitmap(
			D2D1::SizeU(static_cast<UINT>(width), static_cast<UINT>(height)),
			pixelData,
			static_cast<UINT>(pitch),
			bitmapProperties,
			&pBitmap
		);

		if (FAILED(hr))
		{
			return nullptr;
		}

		Texture* texture = new Texture();
		texture->m_pBitmap = pBitmap;
		texture->m_filePath = cacheKey;
		texture->m_width = static_cast<float>(width);
		texture->m_height = static_cast<float>(height);

		m_textureCache[cacheKey] = texture;
		return texture;
	}
}
