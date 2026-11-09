#pragma once
#ifndef __GAME_PLAY_FRAME_AUDIO_SOURCE_H__
#define __GAME_PLAY_FRAME_AUDIO_SOURCE_H__

#include "Component.h"
#include <string>

namespace GamePlay
{
	/**
	 * @brief 音频源组件
	 *
	 * 使用BASS库播放音频，支持音量、音调、循环播放等控制
	 */
	class AudioSource : public Component
	{
	public:
		/**
		 * @brief 构造函数
		 *
		 * @param pObj 所属游戏对象指针
		 */
		AudioSource(class GameObject* pObj) : Component(pObj)
		{
			mType = COMTYPE::ComType_AudioSource;

			mStreamHandle = 0;
			mVolume = 1.0f;
			mPitch = 1.0f;
			mLoop = false;
			mMute = false;
			mSpatialBlend = 0.0f;
			mClipPath = "";
			mIsLoaded = false;
		}

		/** @brief 析构函数 */
		~AudioSource();

		/**
		 * @brief 设置音频文件路径
		 *
		 * @param path 音频文件路径
		 */
		void SetClip(const char* path);

		/** @brief 获取音频文件路径 */
		const std::string& Clip() const { return mClipPath; }

		/** @brief 播放音频 */
		void Play();

		/** @brief 停止音频 */
		void Stop();

		/** @brief 暂停音频 */
		void Pause();

		/** @brief 恢复播放 */
		void UnPause();

		/** @brief 判断是否正在播放 */
		bool IsPlaying() const;

		/**
		 * @brief 设置音量
		 *
		 * @param volume 音量值（0~1）
		 */
		void SetVolume(float volume);

		/** @brief 获取音量 */
		const float& Volume() const { return mVolume; }

		/**
		 * @brief 设置音调
		 *
		 * @param pitch 音调值（0~4，默认1.0）
		 */
		void SetPitch(float pitch);

		/** @brief 获取音调 */
		const float& Pitch() const { return mPitch; }

		/**
		 * @brief 设置循环播放
		 *
		 * @param loop 是否循环
		 */
		void SetLoop(bool loop);

		/** @brief 获取循环状态 */
		const bool& Loop() const { return mLoop; }

		/**
		 * @brief 设置静音
		 *
		 * @param mute 是否静音
		 */
		void SetMute(bool mute);

		/** @brief 获取静音状态 */
		const bool& Mute() const { return mMute; }

		/**
		 * @brief 设置空间混合
		 *
		 * @param blend 空间混合值（0~1）
		 */
		void SetSpatialBlend(float blend);

		/** @brief 获取空间混合值 */
		const float& SpatialBlend() const { return mSpatialBlend; }

		/** @brief 获取当前播放时间（秒） */
		float Time() const;

		/** @brief 获取音频总时长（秒） */
		float Duration() const;

	protected:
		/** @brief 应用音量设置 */
		void _ApplyVolume();

		/** @brief 应用音调设置 */
		void _ApplyPitch();

		/** @brief 释放音频流 */
		void _ReleaseStream();

		unsigned long mStreamHandle;  ///< BASS音频流句柄
		std::string mClipPath;        ///< 音频文件路径
		float mVolume;                ///< 音量（0~1）
		float mPitch;                 ///< 音调（0~4）
		bool mLoop;                   ///< 是否循环播放
		bool mMute;                   ///< 是否静音
		bool mIsLoaded;               ///< 是否已加载
		float mSpatialBlend;          ///< 空间混合（0=2D, 1=3D）
	};
}

#endif