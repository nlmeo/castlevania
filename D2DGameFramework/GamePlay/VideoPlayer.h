#pragma once
#ifndef __GAME_PLAY_FRAME_VIDEO_PLAYER_H__
#define __GAME_PLAY_FRAME_VIDEO_PLAYER_H__

#include <d2d1.h>
#include <string>
#include <functional>
#include <windows.h>
#include "Component.h"
#include "../TypeDefine/Matrix3x3.h"
#include "../TypeDefine/Geometric.h"

namespace GamePlay
{
	/**
	 * @brief 视频播放器组件
	 *
	 * 使用FFmpeg解码视频，支持播放、暂停、停止、循环等功能，提供丰富的回调接口
	 */
	class VideoPlayer : public RenderComponent
	{
	public:
		/**
		 * @brief 构造函数
		 *
		 * @param pObj 所属游戏对象指针
		 */
		VideoPlayer(class GameObject* pObj);

		/** @brief 析构函数 */
		~VideoPlayer();

		/**
		 * @brief 设置视频路径
		 *
		 * @param path 视频文件路径
		 */
		void SetVideoPath(const char* path);

		/** @brief 获取视频路径 */
		const std::string& VideoPath() const { return mVideoPath; }

		/** @brief 播放视频 */
		void Play();

		/** @brief 暂停视频 */
		void Pause();

		/** @brief 停止视频 */
		void Stop();

		/** @brief 判断是否正在播放 */
		bool IsPlaying() const { return mIsPlaying; }

		/**
		 * @brief 设置音量
		 *
		 * @param volume 音量值（0~1）
		 */
		void SetVolume(float volume);

		/** @brief 获取音量 */
		const float& Volume() const { return mVolume; }

		/**
		 * @brief 设置静音
		 *
		 * @param mute 是否静音
		 */
		void SetMute(bool mute);

		/** @brief 获取静音状态 */
		const bool& Mute() const { return mMute; }

		/**
		 * @brief 设置循环播放
		 *
		 * @param loop 是否循环
		 */
		void SetLoop(bool loop);

		/** @brief 获取循环状态 */
		const bool& Loop() const { return mLoop; }

		/**
		 * @brief 跳转到指定时间
		 *
		 * @param time 时间位置（秒）
		 */
		void Seek(float time);

		/** @brief 获取视频总时长（秒） */
		float Duration() const { return mDuration; }

		/** @brief 获取当前播放时间（秒） */
		float Time() const;

		/** @brief 获取视频宽度（像素） */
		int Width() const { return mWidth; }

		/** @brief 获取视频高度（像素） */
		int Height() const { return mHeight; }

		/** @brief 获取帧率 */
		float FrameRate() const { return mFrameRate; }

		/** @brief 获取当前帧纹理 */
		class D2DCore::Texture* GetCurrentFrame() const;

		/** @brief 更新视频播放 */
		void Update();

		/** @brief 设置播放回调 */
		void SetOnPlayCallback(std::function<void()> callback) { mOnPlayCallback = callback; }

		/** @brief 设置暂停回调 */
		void SetOnPauseCallback(std::function<void()> callback) { mOnPauseCallback = callback; }

		/** @brief 设置停止回调 */
		void SetOnStopCallback(std::function<void()> callback) { mOnStopCallback = callback; }

		/** @brief 设置播放结束回调 */
		void SetOnEndCallback(std::function<void()> callback) { mOnEndCallback = callback; }

		/** @brief 设置帧回调 */
		void SetOnFrameCallback(std::function<void(int, float)> callback) { mOnFrameCallback = callback; }

		/**
		 * @brief 设置锚点
		 *
		 * @param x X轴锚点（0~1）
		 * @param y Y轴锚点（0~1）
		 */
		void SetAnchor(float x, float y) { mAnchor = { x, y }; }

		/**
		 * @brief 设置透明度
		 *
		 * @param op 透明度值（0~1）
		 */
		void SetOpacity(float op) { mOpacity = MathType::clamp(op); }

		/**
		 * @brief 设置渲染层级顺序
		 *
		 * @param layerordr 层级顺序值
		 */
		void SetOrderInLayer(int layerordr) { mOrderInLayer = layerordr; }

		/** @brief 计算渲染矩阵 */
		virtual void CaclRenderMatrix();

		/** @brief 获取锚点 */
		MathType::vec2f Anchor() const { return mAnchor; }

		/** @brief 获取透明度 */
		float Opacity() const { return mOpacity; }

		/** @brief 获取渲染层级顺序 */
		const int& OrderInLayer() const override { return mOrderInLayer; }

		/**
		 * @brief 渲染视频帧
		 *
		 * @param vpv 视图-投影-视口变换矩阵
		 */
		void Render(const MathType::Mat3x3f& vpv) override;

		/** @brief 获取渲染矩形区域 */
		const MathType::RectF& GetRenderRect() const override;

	protected:
		/** @brief 更新当前帧 */
		void UpdateCurrentFrame();

		/** @brief 通知播放事件 */
		void NotifyOnPlay();

		/** @brief 通知暂停事件 */
		void NotifyOnPause();

		/** @brief 通知停止事件 */
		void NotifyOnStop();

		/** @brief 通知播放结束事件 */
		void NotifyOnEnd();

		/**
		 * @brief 通知帧事件
		 *
		 * @param frameIndex 帧索引
		 * @param time 当前时间
		 */
		void NotifyOnFrame(int frameIndex, float time);

	private:
		/**
		 * @brief 打开视频文件
		 *
		 * @param path 视频文件路径
		 * @return 是否成功打开
		 */
		bool OpenVideo(const char* path);

		/** @brief 关闭视频 */
		void CloseVideo();

		/** @brief 解码音频包 */
		void DecodeAudioPacket();

		/** @brief 处理数据包 */
		void ProcessPackets();

		/** @brief 计算当前播放时间 */
		double CalculateCurrentTime();

		std::string mVideoPath;                    ///< 视频文件路径
		bool mIsPlaying;                           ///< 是否正在播放
		bool mIsPaused;                            ///< 是否暂停
		float mVolume;                             ///< 音量（0~1）
		bool mMute;                                ///< 是否静音
		bool mLoop;                                ///< 是否循环播放
		int mWidth;                                ///< 视频宽度
		int mHeight;                               ///< 视频高度
		float mFrameRate;                          ///< 帧率
		float mDuration;                           ///< 总时长（秒）
		class D2DCore::Texture* mCurrentFrame;     ///< 当前帧纹理
		double mCurrentTime;                       ///< 当前播放时间
		int mCurrentFrameIndex;                    ///< 当前帧索引

		std::function<void()> mOnPlayCallback;      ///< 播放回调
		std::function<void()> mOnPauseCallback;     ///< 暂停回调
		std::function<void()> mOnStopCallback;      ///< 停止回调
		std::function<void()> mOnEndCallback;       ///< 结束回调
		std::function<void(int, float)> mOnFrameCallback;  ///< 帧回调

		void* mInternal;                           ///< 内部数据指针
		bool mHasVideo;                            ///< 是否有视频流
		bool mHasAudio;                            ///< 是否有音频流
		unsigned long mAudioStreamHandle;          ///< 音频流句柄
		LARGE_INTEGER mStartTime;                  ///< 播放开始时间
		LARGE_INTEGER mPauseTime;                  ///< 暂停时间
		double mPauseAccumulated;                  ///< 累计暂停时间
		bool mNewFrameReady;                       ///< 是否有新帧就绪
		std::wstring mFrameTextureKey;             ///< 帧纹理缓存键

		MathType::vec2f mAnchor;                   ///< 锚点
		MathType::RectF mSrcRect;                  ///< 源矩形区域
		MathType::RectF mDestRect;                 ///< 目标矩形区域
		MathType::RectF mRenderRect;               ///< 渲染矩形区域

		float mOpacity;                            ///< 透明度
		int mOrderInLayer;                         ///< 渲染层级顺序

		MathType::Mat3x3f mTransformMatrix;        ///< 变换矩阵
	};
}

#endif