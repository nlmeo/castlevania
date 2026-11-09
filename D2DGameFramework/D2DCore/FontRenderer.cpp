#include "FontRenderer.h"
#include "D2DRender.h"
#include <string>

namespace D2DCore
{
	/**
 * @brief 单文件字体集合加载器（内部类）
 *
 * 实现IDWriteFontCollectionLoader接口，用于从单个字体文件创建字体集合
 */
	class SingleFileFontFileEnumerator;

	class SingleFileFontCollectionLoader : public IDWriteFontCollectionLoader
	{
	public:
		/**
		 * @brief 构造函数
		 *
		 * @param pFactory DirectWrite工厂
		 */
		SingleFileFontCollectionLoader(IDWriteFactory* pFactory) : m_pFactory(pFactory), m_refCount(1) {}

		/**
		 * @brief 查询接口
		 *
		 * @param riid 接口ID
		 * @param ppvObject 输出接口指针
		 * @return S_OK表示成功
		 */
		IFACEMETHOD(QueryInterface)(REFIID riid, void** ppvObject)
		{
			if (riid == __uuidof(IUnknown) || riid == __uuidof(IDWriteFontCollectionLoader))
			{
				*ppvObject = this;
				AddRef();
				return S_OK;
			}
			*ppvObject = nullptr;
			return E_NOINTERFACE;
		}

		/** @brief 增加引用计数 */
		IFACEMETHOD_(ULONG, AddRef)() { return InterlockedIncrement(&m_refCount); }

		/** @brief 减少引用计数，计数为0时销毁对象 */
		IFACEMETHOD_(ULONG, Release)()
		{
			ULONG count = InterlockedDecrement(&m_refCount);
			if (count == 0)
				delete this;
			return count;
		}

		/**
		 * @brief 创建字体文件枚举器
		 *
		 * @param pFactory DirectWrite工厂
		 * @param collectionKey 集合键（字体文件路径）
		 * @param collectionKeySize 键大小
		 * @param ppEnumerator 输出枚举器
		 */
		IFACEMETHOD(CreateEnumeratorFromKey)(IDWriteFactory* pFactory, const void* collectionKey, UINT32 collectionKeySize, IDWriteFontFileEnumerator** ppEnumerator);

	private:
		IDWriteFactory* m_pFactory;  ///< DirectWrite工厂
		LONG m_refCount;              ///< 引用计数
	};

	/**
	 * @brief 单文件字体文件枚举器（内部类）
	 *
	 * 实现IDWriteFontFileEnumerator接口，枚举单个字体文件
	 */
	class SingleFileFontFileEnumerator : public IDWriteFontFileEnumerator
	{
	public:
		/**
		 * @brief 构造函数
		 *
		 * @param pFactory DirectWrite工厂
		 * @param filePath 字体文件路径
		 */
		SingleFileFontFileEnumerator(IDWriteFactory* pFactory, const wchar_t* filePath) : m_pFactory(pFactory), m_currentIndex(-1), m_refCount(1)
		{
			m_filePath = filePath;
		}

		/**
		 * @brief 查询接口
		 *
		 * @param riid 接口ID
		 * @param ppvObject 输出接口指针
		 * @return S_OK表示成功
		 */
		IFACEMETHOD(QueryInterface)(REFIID riid, void** ppvObject)
		{
			if (riid == __uuidof(IUnknown) || riid == __uuidof(IDWriteFontFileEnumerator))
			{
				*ppvObject = this;
				AddRef();
				return S_OK;
			}
			*ppvObject = nullptr;
			return E_NOINTERFACE;
		}

		/** @brief 增加引用计数 */
		IFACEMETHOD_(ULONG, AddRef)() { return InterlockedIncrement(&m_refCount); }

		/** @brief 减少引用计数，计数为0时销毁对象 */
		IFACEMETHOD_(ULONG, Release)()
		{
			ULONG count = InterlockedDecrement(&m_refCount);
			if (count == 0)
				delete this;
			return count;
		}

		/**
		 * @brief 移动到下一个字体文件
		 *
		 * @param hasCurrentFile 是否还有当前文件
		 * @return S_OK表示成功
		 */
		IFACEMETHOD(MoveNext)(BOOL* hasCurrentFile)
		{
			m_currentIndex++;
			*hasCurrentFile = (m_currentIndex == 0) ? TRUE : FALSE;
			return S_OK;
		}

		/**
		 * @brief 获取当前字体文件
		 *
		 * @param ppFontFile 输出字体文件接口
		 * @return S_OK表示成功
		 */
		IFACEMETHOD(GetCurrentFontFile)(IDWriteFontFile** ppFontFile)
		{
			if (m_currentIndex == 0)
			{
				return m_pFactory->CreateFontFileReference(m_filePath.c_str(), nullptr, ppFontFile);
			}
			*ppFontFile = nullptr;
			return E_FAIL;
		}

	private:
		IDWriteFactory* m_pFactory;  ///< DirectWrite工厂
		std::wstring m_filePath;     ///< 字体文件路径
		int m_currentIndex;          ///< 当前索引
		LONG m_refCount;             ///< 引用计数
	};

	/**
	 * @brief 创建字体文件枚举器实现
	 */
	IFACEMETHODIMP SingleFileFontCollectionLoader::CreateEnumeratorFromKey(IDWriteFactory* pFactory, const void* collectionKey, UINT32 collectionKeySize, IDWriteFontFileEnumerator** ppEnumerator)
	{
		*ppEnumerator = new SingleFileFontFileEnumerator(pFactory, static_cast<const wchar_t*>(collectionKey));
		return S_OK;
	}

	// ==================== FontRenderer 类实现 ====================

	/**
	 * @brief 构造函数
	 *
	 * 初始化成员变量为默认值
	 */
	FontRenderer::FontRenderer()
		: m_pRenderTarget(nullptr)
		, m_pDWriteFactory(nullptr)
		, m_pBrush(nullptr)
	{
	}

	/**
	 * @brief 析构函数
	 *
	 * 释放所有字体格式和DirectWrite资源
	 */
	FontRenderer::~FontRenderer()
	{
		for (auto* fmt : m_fontFormats)
		{
			SAFE_RELEASE(fmt);
		}
		m_fontFormats.clear();
		SAFE_RELEASE(m_pBrush);
		SAFE_RELEASE(m_pDWriteFactory);
	}

	/**
	 * @brief 初始化字体渲染器
	 *
	 * 创建DirectWrite工厂和画刷
	 *
	 * @param pRenderTarget D2D渲染目标
	 * @return 初始化成功返回true，失败返回false
	 */
	bool FontRenderer::Initialize()
	{
		if (!D2DRd.IsInit())
		{
			MessageBox(nullptr,
				TEXT("请保证 D2DRender::Initialize 函数优先初始化成功后再调用 FontRenderer::Initialize 函数!"),
				TEXT("D2DFontRender初始化提示!"), MB_OK);
			return false;
		}
		m_pRenderTarget = D2DRd.GetD2DRenderTarget();

		// 创建DirectWrite工厂
		HRESULT hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(&m_pDWriteFactory)
		);

		if (FAILED(hr))
			return false;

		// 创建纯色画刷（默认白色）
		hr = m_pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::White),
			&m_pBrush
		);

		return SUCCEEDED(hr);
	}

	/**
	 * @brief 从系统字体创建字体
	 *
	 * @param fontName 字体名称
	 * @param fontSize 字体大小
	 * @param bold 是否粗体
	 * @param italic 是否斜体
	 * @return 成功返回字体ID，失败返回-1
	 */
	int FontRenderer::CreateFont(const wchar_t* fontName, float fontSize, bool bold, bool italic)
	{
		if (!m_pDWriteFactory)
			return -1;

		IDWriteTextFormat* pTextFormat = nullptr;

		// 创建文本格式
		HRESULT hr = m_pDWriteFactory->CreateTextFormat(
			fontName,
			nullptr,
			bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
			italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			fontSize,
			L"en-us",
			&pTextFormat
		);

		if (FAILED(hr))
			return -1;

		// 设置不换行
		pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

		// 添加到字体列表
		int fontId = static_cast<int>(m_fontFormats.size());
		m_fontFormats.push_back(pTextFormat);
		return fontId;
	}

	/**
	 * @brief 从字体文件创建字体
	 *
	 * 通过自定义字体集合加载器从TTF字体文件加载字体
	 *
	 * @param filePath 字体文件路径
	 * @param fontSize 字体大小
	 * @param bold 是否粗体
	 * @param italic 是否斜体
	 * @return 成功返回字体ID，失败返回-1
	 */
	int FontRenderer::CreateFontFromFile(const wchar_t* filePath, float fontSize, bool bold, bool italic)
	{
		if (!m_pDWriteFactory)
			return -1;

		// 创建自定义字体集合加载器
		SingleFileFontCollectionLoader* pLoader = new SingleFileFontCollectionLoader(m_pDWriteFactory);
		HRESULT hr = m_pDWriteFactory->RegisterFontCollectionLoader(pLoader);

		if (FAILED(hr))
		{
			SAFE_RELEASE(pLoader);
			return -1;
		}

		// 创建自定义字体集合
		IDWriteFontCollection* pFontCollection = nullptr;
		UINT32 keySize = static_cast<UINT32>((wcslen(filePath) + 1) * sizeof(wchar_t));
		hr = m_pDWriteFactory->CreateCustomFontCollection(
			pLoader,
			filePath,
			keySize,
			&pFontCollection
		);

		// 卸载加载器
		m_pDWriteFactory->UnregisterFontCollectionLoader(pLoader);
		SAFE_RELEASE(pLoader);

		if (FAILED(hr))
		{
			SAFE_RELEASE(pFontCollection);
			return -1;
		}

		// 获取字体系列
		IDWriteFontFamily* pFontFamily = nullptr;
		hr = pFontCollection->GetFontFamily(0, &pFontFamily);
		if (FAILED(hr))
		{
			SAFE_RELEASE(pFontCollection);
			SAFE_RELEASE(pFontFamily);
			return -1;
		}

		// 获取字体系列名称
		IDWriteLocalizedStrings* pFamilyNames = nullptr;
		hr = pFontFamily->GetFamilyNames(&pFamilyNames);
		SAFE_RELEASE(pFontFamily);

		if (FAILED(hr))
		{
			SAFE_RELEASE(pFontCollection);
			SAFE_RELEASE(pFamilyNames);
			return -1;
		}

		// 查找中文或英文名称
		UINT32 index = 0;
		BOOL exists = FALSE;
		hr = pFamilyNames->FindLocaleName(L"zh-CN", &index, &exists);
		if (FAILED(hr) || !exists)
		{
			hr = pFamilyNames->FindLocaleName(L"en-us", &index, &exists);
			if (FAILED(hr) || !exists)
			{
				index = 0;
			}
		}

		// 获取字体名称
		UINT32 length = 0;
		hr = pFamilyNames->GetStringLength(index, &length);
		if (FAILED(hr))
		{
			SAFE_RELEASE(pFontCollection);
			SAFE_RELEASE(pFamilyNames);
			return -1;
		}

		wchar_t* pFamilyName = new wchar_t[length + 1];
		hr = pFamilyNames->GetString(index, pFamilyName, length + 1);
		SAFE_RELEASE(pFamilyNames);

		if (FAILED(hr))
		{
			delete[] pFamilyName;
			SAFE_RELEASE(pFontCollection);
			return -1;
		}

		// 创建文本格式
		IDWriteTextFormat* pTextFormat = nullptr;
		DWRITE_FONT_WEIGHT fontWeight = bold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL;
		DWRITE_FONT_STYLE fontStyle = italic ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL;

		hr = m_pDWriteFactory->CreateTextFormat(
			pFamilyName,
			pFontCollection,
			fontWeight,
			fontStyle,
			DWRITE_FONT_STRETCH_NORMAL,
			fontSize,
			L"zh-CN",
			&pTextFormat
		);

		delete[] pFamilyName;
		SAFE_RELEASE(pFontCollection);

		if (FAILED(hr))
		{
			SAFE_RELEASE(pTextFormat);
			return -1;
		}

		// 设置不换行
		pTextFormat->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

		// 添加到字体列表
		int fontId = static_cast<int>(m_fontFormats.size());
		m_fontFormats.push_back(pTextFormat);

		return fontId;
	}

	/**
	 * @brief 绘制文字（简单版本）
	 *
	 * @param text 要绘制的文字
	 * @param fontId 字体ID
	 * @param x X坐标
	 * @param y Y坐标
	 * @param color 文字颜色
	 */
	void FontRenderer::DrawText(const wchar_t* text, int fontId, float x, float y, const ColorType::COLORF& color)
	{
		if (!m_pRenderTarget || !m_pBrush || fontId < 0 || fontId >= static_cast<int>(m_fontFormats.size()))
			return;

		// 设置画刷颜色
		m_pBrush->SetColor(D2D1::ColorF(color.r, color.g, color.b, color.a));

		// 计算文字长度并创建布局矩形
		size_t len = wcslen(text);
		D2D1_RECT_F layoutRect = D2D1::RectF(x, y, x + 10000.0f, y + 10000.0f);

		// 绘制文字
		m_pRenderTarget->DrawText(
			text,
			static_cast<UINT32>(len),
			m_fontFormats[fontId],
			layoutRect,
			m_pBrush,
			D2D1_DRAW_TEXT_OPTIONS_NONE,
			DWRITE_MEASURING_MODE_NATURAL
		);
	}

	/**
	 * @brief 绘制文字（带对齐版本）
	 *
	 * @param text 要绘制的文字
	 * @param fontId 字体ID
	 * @param layoutRect 布局矩形
	 * @param color 文字颜色
	 * @param hAlign 水平对齐方式
	 * @param vAlign 垂直对齐方式
	 */
	void FontRenderer::DrawText(const wchar_t* text, int fontId, const D2D1_RECT_F& layoutRect, const ColorType::COLORF& color,
		DWRITE_TEXT_ALIGNMENT hAlign, DWRITE_PARAGRAPH_ALIGNMENT vAlign, bool wordWrap)
	{
		if (!m_pRenderTarget || !m_pBrush || fontId < 0 || fontId >= static_cast<int>(m_fontFormats.size()))
			return;

		// 设置画刷颜色
		m_pBrush->SetColor(D2D1::ColorF(color.r, color.g, color.b, color.a));

		// 获取字体格式并设置对齐方式和换行
		IDWriteTextFormat* fmt = m_fontFormats[fontId];
		fmt->SetTextAlignment(hAlign);
		fmt->SetParagraphAlignment(vAlign);
		fmt->SetWordWrapping(wordWrap ? DWRITE_WORD_WRAPPING_WRAP : DWRITE_WORD_WRAPPING_NO_WRAP);

		// 计算文字长度并绘制
		size_t len = wcslen(text);
		m_pRenderTarget->DrawText(
			text,
			static_cast<UINT32>(len),
			fmt,
			layoutRect,
			m_pBrush,
			D2D1_DRAW_TEXT_OPTIONS_NONE,
			DWRITE_MEASURING_MODE_NATURAL
		);
	}

	/**
	 * @brief 测量文字尺寸
	 *
	 * @param text 要测量的文字
	 * @param fontId 字体ID
	 * @param maxWidth 最大宽度
	 * @param maxHeight 最大高度
	 * @return 文字尺寸
	 */
	D2D1_SIZE_F FontRenderer::MeasureText(const wchar_t* text, int fontId, float maxWidth, float maxHeight)
	{
		D2D1_SIZE_F size = { 0.0f, 0.0f };

		if (!m_pDWriteFactory || fontId < 0 || fontId >= static_cast<int>(m_fontFormats.size()))
			return size;

		// 创建文本布局
		IDWriteTextLayout* pLayout = nullptr;
		size_t len = wcslen(text);

		HRESULT hr = m_pDWriteFactory->CreateTextLayout(
			text,
			static_cast<UINT32>(len),
			m_fontFormats[fontId],
			maxWidth,
			maxHeight,
			&pLayout
		);

		// 获取文字度量信息
		if (SUCCEEDED(hr))
		{
			DWRITE_TEXT_METRICS metrics;
			pLayout->GetMetrics(&metrics);
			size.width = metrics.width;
			size.height = metrics.height;
			SAFE_RELEASE(pLayout);
		}

		return size;
	}
}
