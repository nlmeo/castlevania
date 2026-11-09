#include "UIAnimator.h"
#include "../D2DCore/D2DRender.h"
#include <algorithm>

namespace GameUI
{
	// ==================== UIAnimationClip ====================

	bool UIAnimationClip::AddFrames(const std::vector<std::tuple<std::wstring, MathType::vec2f, UINT, UINT>>& tasks)
	{
		if (tasks.empty()) return false;

		for (const auto& task : tasks)
		{
			for (UINT i = 0; i < std::get<2>(task); ++i)
			{
				UIFrame frame;
				frame.texture = TexMgr.LoadTextureWithoutColor(std::get<0>(task).c_str(), std::get<3>(task));
				if (!frame.texture) return false;

				frame.anchor = std::get<1>(task);
				frame.useSpriteRegion = false;
				frame.srcStart = { 0, 0 };
				frame.srcSize = { 0, 0 };
				frames.push_back(frame);
			}
		}
		return true;
	}

	bool UIAnimationClip::AddFrames(const std::vector<std::tuple<std::wstring, float, float, UINT, UINT>>& tasks)
	{
		if (tasks.empty()) return false;

		for (const auto& task : tasks)
		{
			for (UINT i = 0; i < std::get<3>(task); ++i)
			{
				UIFrame frame;
				frame.texture = TexMgr.LoadTextureWithoutColor(std::get<0>(task).c_str(), std::get<4>(task));
				if (!frame.texture) return false;

				float anchorX = -(std::get<1>(task) / frame.texture->GetWidth()) + 0.5f;
				float anchorY = -(std::get<2>(task) / frame.texture->GetHeight()) + 0.5f;
				frame.anchor = { anchorX, anchorY };
				frame.useSpriteRegion = false;
				frame.srcStart = { 0, 0 };
				frame.srcSize = { 0, 0 };
				frames.push_back(frame);
			}
		}
		return true;
	}

	int UIAnimationClip::GetFrameIndex(float time) const
	{
		if (frames.empty()) return 0;

		float total = GetTotalDuration();
		float dur = loop ? fmod(time, total) : min(time, total);

		return static_cast<int>(dur * frameRate);
	}

	// ==================== UIAnimator ====================

	UIAnimator::UIAnimator()
	{
		mType = UIElementType::UI_Animator;
		mIsActive = true;
	}

	UIAnimator::~UIAnimator()
	{
	}

	void UIAnimator::Update(float deltaTime)
	{
		if (!mCurrentClip || mStatus != UIAnimStatus::Playing)
			return;

		mCurrentTime += deltaTime;

		int idx = mCurrentClip->GetFrameIndex(mCurrentTime);
		if (idx < 0) idx = 0;
		if (idx >= static_cast<int>(mCurrentClip->frames.size()))
			idx = static_cast<int>(mCurrentClip->frames.size()) - 1;
		if (idx < 0 || mCurrentClip->frames.empty())
			return;

		mCurrentFrame = mCurrentClip->frames[idx];

		// 更新 Size 为当前帧纹理的实际尺寸
		if (mCurrentFrame.texture)
		{
			UIElement::SetSize(mCurrentFrame.texture->GetWidth(), mCurrentFrame.texture->GetHeight());
		}

		if (mCurrentTime >= mCurrentClip->GetTotalDuration())
		{
			if (mCurrentClip->loop)
				mCurrentTime = 0.0f;
			else
				mStatus = UIAnimStatus::Stopped;
		}
	}

	void UIAnimator::Render()
	{
		if (!mIsActive || !mCurrentFrame.texture || !mCurrentFrame.texture->IsValid())
			return;

		auto* rt = D2DRd.GetD2DRenderTarget();
		if (!rt) return;

		float texW, texH;
		D2D1_RECT_F srcRect;

		if (mCurrentFrame.useSpriteRegion)
		{
			srcRect = D2D1::RectF(
				mCurrentFrame.srcStart.x,
				mCurrentFrame.srcStart.y,
				mCurrentFrame.srcStart.x + mCurrentFrame.srcSize.x,
				mCurrentFrame.srcStart.y + mCurrentFrame.srcSize.y);
			texW = mCurrentFrame.srcSize.x;
			texH = mCurrentFrame.srcSize.y;
		}
		else
		{
			srcRect = D2D1::RectF(0, 0, mCurrentFrame.texture->GetWidth(), mCurrentFrame.texture->GetHeight());
			texW = mCurrentFrame.texture->GetWidth();
			texH = mCurrentFrame.texture->GetHeight();
		}

		// 锚点偏移：anchor (0,0) 纹理左上角对齐 worldRect 左上角
		//          anchor (0.5,0.5) 纹理居中对齐
		float offsetX = -texW * mCurrentFrame.anchor.x;
		float offsetY = -texH * mCurrentFrame.anchor.y;

		D2D1_RECT_F destRect = D2D1::RectF(
			mWorldRect.left + offsetX,
			mWorldRect.top + offsetY,
			mWorldRect.left + offsetX + texW,
			mWorldRect.top + offsetY + texH);

		// 水平翻转：以绘制矩形中心为轴镜像（用于方向相反的装饰箭头等）
		D2D1::Matrix3x2F previousTransform;
		rt->GetTransform(&previousTransform);
		if (mFlipX)
		{
			float cx = (destRect.left + destRect.right) * 0.5f;
			rt->SetTransform(D2D1::Matrix3x2F::Scale(-1.0f, 1.0f, D2D1::Point2F(cx, 0.0f)) * previousTransform);
		}

		rt->DrawBitmap(
			mCurrentFrame.texture->GetD2DBitmap(),
			destRect,
			1.0f,
			D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
			srcRect);

		if (mFlipX)
		{
			rt->SetTransform(previousTransform);
		}
	}
}
