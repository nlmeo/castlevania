#include "D2DRender.h"
#include "../GameCore/Application.h"
#include "Texture.h"

#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"dwrite.lib")

namespace D2DCore
{
	D2DRender::D2DRender() 
		: m_hwnd(nullptr)
		, m_width(0)
		, m_height(0)
		, m_pD2DFactory(nullptr)
		, m_pRenderTarget(nullptr)
		, m_pBrush(nullptr)
		, m_isInit(false)
	{
	}

	D2DRender::~D2DRender()
	{
		SAFE_RELEASE(m_pBrush);
		SAFE_RELEASE(m_pRenderTarget);
		SAFE_RELEASE(m_pD2DFactory);
	}
	
	bool D2DRender::Initialize()
	{
		if (!App.IsInit())
		{
			MessageBox(nullptr,
				TEXT("请保证 Application::Initialize 函数优先初始化成功后再调用 D2DRender::Initialize 函数!"),
				TEXT("D2DRender初始化提示!"),MB_OK);
			return false;
		}

		m_hwnd = App.GetHwnd();
		m_width = App.GetWidth();
		m_height = App.GetHeight();

		// 创建D2D工厂
		HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &m_pD2DFactory);
		if (FAILED(hr))
		{
			MessageBox(m_hwnd,TEXT("创建D2DFactory失败!"),TEXT("D2DRender初始化提示!"), MB_OK);
			return false;
		}
			

		// 创建设备资源
		return _CreateDeviceResources();
	}

	bool D2DRender::_CreateDeviceResources()
	{
		// 先释放旧资源
		SAFE_RELEASE(m_pRenderTarget);
		SAFE_RELEASE(m_pBrush);

		// 获取窗口客户区尺寸
		RECT rc;
		GetClientRect(m_hwnd, &rc);
		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

		// 创建HwndRenderTarget
		HRESULT hr = m_pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hwnd, size, D2D1_PRESENT_OPTIONS_IMMEDIATELY),
			&m_pRenderTarget
		);
		if (FAILED(hr))
		{
			MessageBox(m_hwnd, TEXT("创建RenderTarget失败!"), TEXT("D2DRender初始化提示!"), MB_OK);
			return false;
		}

		// 创建纯色画刷（默认白色）
		hr = m_pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::White),
			&m_pBrush
		);
		return m_isInit = SUCCEEDED(hr);
	}

	void D2DRender::Resize(int width, int height)
	{
		m_width = width;
		m_height = height;
		if (m_pRenderTarget!=nullptr)
		{
			D2D1_SIZE_U size = D2D1::SizeU(width, height);
			m_pRenderTarget->Resize(size);
		}
	}

	void D2DRender::BeginDraw()
	{
		if (m_pRenderTarget)
		{
			m_pRenderTarget->BeginDraw();
			m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));
		}
	}

	void D2DRender::EndDraw()
	{
		if (m_pRenderTarget)
		{
			HRESULT hr = m_pRenderTarget->EndDraw();
			if (hr == D2DERR_RECREATE_TARGET)
			{
				// 设备丢失，重建资源
				_CreateDeviceResources();
			}
		}
	}

	void D2DRender::Clear(float r, float g, float b, float a)
	{
		if (m_pRenderTarget)
		{
			m_pRenderTarget->Clear(D2D1::ColorF(r, g, b, a));
		}
	}

	void D2DRender::Clear(const ColorType::COLORF& color)
	{
		Clear(color.r, color.g, color.b, color.a);
	}

	void D2DRender::DrawLine(float x1, float y1, float x2, float y2, const ColorType::COLORF& color, float strokeWidth)
	{
		if (!m_pRenderTarget || !m_pBrush) return;
		SetBrushColor(color);
		m_pRenderTarget->DrawLine(
			D2D1::Point2F(x1, y1),
			D2D1::Point2F(x2, y2),
			m_pBrush,
			strokeWidth
		);
	}

	void D2DRender::DrawRect(float x, float y, float w, float h, const ColorType::COLORF& strokeColor, float strokeWidth, const ColorType::COLORF& fillColor)
	{
		if (!m_pRenderTarget || !m_pBrush) return;
		D2D1_RECT_F rect = D2D1::RectF(x, y, x + w, y + h);

		// 先填充
		SetBrushColor(fillColor);
		m_pRenderTarget->FillRectangle(rect, m_pBrush);

		// 再描边
		SetBrushColor(strokeColor);
		m_pRenderTarget->DrawRectangle(rect, m_pBrush, strokeWidth);
	}

	void D2DRender::DrawRect(float x, float y, float w, float h, const ColorType::COLORF& fillColor)

	{
		if (!m_pRenderTarget || !m_pBrush) return;
		

		D2D1_RECT_F rect = D2D1::RectF(x, y, x + w, y + h);
		SetBrushColor(fillColor);
		m_pRenderTarget->FillRectangle(rect, m_pBrush);
	}

	void D2DRender::DrawCircle(float cx, float cy, float radius, const ColorType::COLORF& fillColor)
	{
		if (!m_pRenderTarget || !m_pBrush) return;
		SetBrushColor(fillColor);
		D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(cx, cy), radius, radius);
		m_pRenderTarget->FillEllipse(ellipse, m_pBrush);
	}

	void D2DRender::DrawCircle(float cx, float cy, float radius, const ColorType::COLORF& strokeColor, float strokeWidth, const ColorType::COLORF& fillColor)
	{
		if (!m_pRenderTarget || !m_pBrush) return;
		D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(cx, cy), radius, radius);

		// 先填充
		SetBrushColor(fillColor);
		m_pRenderTarget->FillEllipse(ellipse, m_pBrush);

		// 再描边
		SetBrushColor(strokeColor);
		m_pRenderTarget->DrawEllipse(ellipse, m_pBrush, strokeWidth);
	}

	void D2DRender::DrawRoundedRect(float x, float y, float w, float h, float radiusX, float radiusY, const ColorType::COLORF& fillColor)
	{
		if (!m_pRenderTarget || !m_pBrush) return;
		D2D1_ROUNDED_RECT rect = D2D1::RoundedRect(D2D1::RectF(x, y, x + w, y + h), radiusX, radiusY);
		SetBrushColor(fillColor);
		m_pRenderTarget->FillRoundedRectangle(rect, m_pBrush);
	}

	void D2DRender::DrawRoundedRect(float x, float y, float w, float h, float radiusX, float radiusY, const ColorType::COLORF& strokeColor, float strokeWidth, const ColorType::COLORF& fillColor)
	{
		if (!m_pRenderTarget || !m_pBrush) return;
		D2D1_ROUNDED_RECT rect = D2D1::RoundedRect(D2D1::RectF(x, y, x + w, y + h), radiusX, radiusY);

		// 先填充
		SetBrushColor(fillColor);
		m_pRenderTarget->FillRoundedRectangle(rect, m_pBrush);

		// 再描边
		SetBrushColor(strokeColor);
		m_pRenderTarget->DrawRoundedRectangle(rect, m_pBrush, strokeWidth);
	}

	void D2DRender::SetBrushColor(const ColorType::COLORF& color)
	{
		if (m_pBrush)
		{
			m_pBrush->SetColor(D2D1::ColorF(color.r, color.g, color.b, color.a));
		}
	}

	bool D2DRender::RenderToTexture(Texture* texture)
	{
		if (!texture || !texture->m_pBitmap || !m_pRenderTarget)
		{
			return false;
		}

		HRESULT hr = texture->m_pBitmap->CopyFromRenderTarget(
			nullptr,
			m_pRenderTarget,
			nullptr
		);

		return SUCCEEDED(hr);
	}


}
