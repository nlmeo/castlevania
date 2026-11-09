#pragma once
#ifndef __GAME_UI_ANIMATOR_H__
#define __GAME_UI_ANIMATOR_H__

#include "UIElement.h"
#include "../D2DCore/TextureManager.h"
#include "../TypeDefine/Vector2.h"
#include <unordered_map>
#include <vector>
#include <string>
#include <tuple>

namespace GameUI
{
	/** @brief 动画播放状态 */
	enum class UIAnimStatus
	{
		Stopped,
		Playing,
		Paused
	};

	/**
	 * @brief 动画帧
	 *
	 * 一帧纹理 + 锚点 + 可选的精灵区域裁剪。
	 * 对应 GamePlay::AF。
	 */
	struct UIFrame
	{
		D2DCore::Texture* texture = nullptr;  ///< 帧纹理
		MathType::vec2f anchor;               ///< 锚点（0,0=左上, 0.5,0.5=居中, 1,1=右下）
		bool useSpriteRegion = false;          ///< 是否使用源区域裁剪
		MathType::vec2f srcStart;              ///< 源区域起点（像素）
		MathType::vec2f srcSize;               ///< 源区域尺寸（像素）
	};

	/**
	 * @brief 动画片段
	 *
	 * 帧序列 + 帧率 + 循环标记。
	 * 对应 GamePlay::AC。
	 */
	struct UIAnimationClip
	{
		std::vector<UIFrame> frames;      ///< 帧列表
		float frameRate = 60.0f;          ///< 帧率
		bool loop = false;                ///< 是否循环

		/**
		 * @brief 批量添加帧（直接指定锚点）
		 *
		 * @param tasks 元素: { 纹理路径, 锚点, 重复次数, 颜色键 }
		 */
		bool AddFrames(const std::vector<std::tuple<std::wstring, MathType::vec2f, UINT, UINT>>& tasks);

		/**
		 * @brief 批量添加帧（像素偏移自动计算锚点）
		 *
		 * @param tasks 元素: { 纹理路径, 像素偏移X, 像素偏移Y, 重复次数, 颜色键 }
		 */
		bool AddFrames(const std::vector<std::tuple<std::wstring, float, float, UINT, UINT>>& tasks);

		/** 根据时间获取帧索引 */
		int GetFrameIndex(float time) const;

		/** 获取总时长（秒） */
		float GetTotalDuration() const
		{
			return frames.empty() ? 0.0f : (frames.size() - 1) / frameRate;
		}

	};

	/**
	 * @brief UI 动画播放器
	 *
	 * 继承 UIElement，内部保存一帧纹理。
	 * 移植自 GamePlay::Animator，可直接加入 UICanvas 使用。
	 *
	 * 使用方式：
	 *   1. new UIAnimator
	 *   2. SetPosition(x, y)
	 *   3. UIAnimationClip clip; clip.AddFrames(...); clip.loop = true;
	 *   4. animator->AddClip(L"name", clip);
	 *   5. animator->Play(L"name");
	 *   6. canvas->AddChild(animator);
	 */
	class UIAnimator : public UIElement
	{
	public:
		UIAnimator();
		~UIAnimator();

		/** 注册动画片段 */
		void AddClip(const wchar_t* clipName, const UIAnimationClip& clip)
		{
			mClips[clipName] = clip;
		}

		/** 从头播放片段 */
		void Play(const wchar_t* clipName)
		{
			auto it = mClips.find(clipName);
			if (it == mClips.end()) return;
			mCurrentClip = &it->second;
			mCurrentClipName = clipName;
			mCurrentTime = 0.0f;
			mStatus = UIAnimStatus::Playing;
			// 立即设置第 0 帧，避免首帧空白
			if (!mCurrentClip->frames.empty())
			{
				mCurrentFrame = mCurrentClip->frames[0];
				if (mCurrentFrame.texture)
					UIElement::SetSize(mCurrentFrame.texture->GetWidth(), mCurrentFrame.texture->GetHeight());
			}
		}

		/** 续播（同片段从当前时间继续，不同片段从头播放） */
		void Continue(const wchar_t* clipName)
		{
			auto it = mClips.find(clipName);
			if (it == mClips.end()) return;
			mCurrentClip = &it->second;
			mCurrentClipName = clipName;
			if (mCurrentTime > mCurrentClip->GetTotalDuration())
				mCurrentTime = 0.0f;
			mStatus = UIAnimStatus::Playing;
		}

		void Stop() { mStatus = UIAnimStatus::Stopped; }
		void Pause() { mStatus = UIAnimStatus::Paused; }

		void RePlay()
		{
			if (!mCurrentClip) return;
			mCurrentTime = 0.0f;
			mStatus = UIAnimStatus::Playing;
		}

		UIAnimStatus GetState() const { return mStatus; }

		bool HasClip(const std::wstring& name) const { return mClips.find(name) != mClips.end(); }

		const std::wstring& GetCurrentClipName() const { return mCurrentClipName; }

		int GetCurrentFrameIndex() const
		{
			return mCurrentClip ? mCurrentClip->GetFrameIndex(mCurrentTime) : 0;
		}

		virtual void Update(float deltaTime) override;
		virtual void Render() override;

	private:
		std::unordered_map<std::wstring, UIAnimationClip> mClips;
		UIAnimationClip* mCurrentClip = nullptr;
		std::wstring mCurrentClipName;
		float mCurrentTime = 0.0f;
		UIAnimStatus mStatus = UIAnimStatus::Stopped;
		UIFrame mCurrentFrame;  ///< 当前帧快照
	};
}

#endif
