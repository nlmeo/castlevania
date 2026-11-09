#include "UIFloatingNumber.h"
#include "../TypeDefine/Universal.h"
#include "../GamePlay/Camera2d.h"
#include "../D2DCore/D2DRender.h"
namespace GameUI
{
	UIFloatingNumber::UIFloatingNumber()
	{
		mIsActive = true;
	}

	UIFloatingNumber::~UIFloatingNumber()
	{
	}

	void UIFloatingNumber::Spawn(int number, float startX, float startY,
		float floatHeight, float duration, float isWordPos)
	{
		SetNumber(number);
		//SetPosition(screenX, screenY);

		mStartPos.x = startX;
		mStartPos.y = startY;
		mFloatHeight = (floatHeight < 0.0f) ? 0.0f : floatHeight;
		mDuration = (duration <= 0.0f) ? 0.001f : duration;
		mElapsed = 0.0f;
		mIsFinished = false;
		mOpacity = 1.0f;
		mIsUseWordPos = isWordPos;

		SetActive(true);
	}

	void UIFloatingNumber::SetOnFinished(std::function<void(UIFloatingNumber*)> callback)
	{
		mOnFinished = callback;
	}

	float UIFloatingNumber::_EaseOutCubic(float t)
	{
		float u = 1.0f - t;
		return 1.0f - u * u * u;
	}

	void UIFloatingNumber::Update(float deltaTime)
	{
		if (!mIsActive || mIsFinished)
		{
			return;
		}

		mElapsed += deltaTime;

		// 进度 [0, 1]
		float progress = MathType::clamp(mElapsed / mDuration, 0.0f, 1.0f);

		// --- 位置：向上浮动（缓出：先快后慢） ---
		float eased = mUseEaseOut ? _EaseOutCubic(progress) : progress;
		
		MathType::vec2f curPos;
		curPos.x = mStartPos.x;
		curPos.y = mStartPos.y - eased * mFloatHeight;
		
		// Keep animation in its original coordinate space; project the whole number at render time.
		
		SetPosition(curPos.x, curPos.y);
		
		CalcWorldRect();  // 无父 Canvas 时也更新世界坐标

		// --- 透明度：平方曲线渐隐（前慢后快） ---
		float fadeProgress = progress * progress;
		mOpacity = 1.0f - fadeProgress;

		// --- 完成 ---
		if (progress >= 1.0f)
		{
			mIsFinished = true;
			mOpacity = 0.0f;
			SetActive(false);

			if (mOnFinished)
			{
				mOnFinished(this);
			}
		}
	}

	void UIFloatingNumber::Render()
	{
		if (!mIsUseWordPos)
		{
			// Canvas children inherit the canvas scale (e.g. equipment menu numbers).
			UINumber::Render();
			return;
		}
		auto* target = D2DRd.GetD2DRenderTarget();
		if (!target || !MainCamera) return;
		D2D1_MATRIX_3X2_F previous;
		target->GetTransform(&previous);
		auto transform = MainCamera->GetVPV();
		target->SetTransform(reinterpret_cast<const D2D1_MATRIX_3X2_F*>(transform.GetMat3x2()));
		UINumber::Render();
		target->SetTransform(previous);
	}
}
