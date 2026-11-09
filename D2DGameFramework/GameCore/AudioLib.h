#pragma once
#ifndef __GAME_CORE_MY_AUDIO_LIB_H__
#define __GAME_CORE_MY_AUDIO_LIB_H__

#include "../HelpTool/SingletonTemplate.h"
#include "../ResHeader.h"
#include <string>
namespace GameCore
{
	class MyAudioLib:public Singleton<MyAudioLib>
	{
		PRIVATE_COPY(MyAudioLib)
		float m_bgmVolume = 1.0f;
		float m_soundVolume = 1.0f;
		float m_voiceVolume = 1.0f;
		std::string mCurrentBGM;   // 当前播放的背景音乐路径
	private:
		MyAudioLib();

	public:
		~MyAudioLib();

		bool Initialize();
		bool FreeLib();
		static bool IsInit();
		bool LoadMusic(const char* path);
		bool LoadSound(const char* path);
		bool EraseMusic(const char* path);
		bool EraseSound(const char* path);
		bool ReleaseAllMusic();
		bool ReleaseAllSound();
		void ReleaseAll();
		bool PlayMusic(const char* path, bool restart = true);
		bool PlaySoundEx(const char* path);
		bool PauseMusic(const char* path);
		void StopMusic(const char* path);
		void StopAllMusic();
		float GetVolume();
		void SetVolume(float v);
		static unsigned long CreateStream(const char* path);
		static void FreeStream(unsigned long handle);

		bool PlayBGM(const char* path, bool restart = false);//播放背景音乐
		bool PlayVoice(const char* path);//播放人声
		bool PlaySE(const char* path);//播放音效


		void SetBGMVolume(float v);//设置背景音乐音量
		void SetSEVolume(float v);//设置音效音量
		void SetVoiceVolume(float v);//设置人声音量

		float GetBGMVolume();//获取背景音乐音量
		float GetSEVolume();//获取音效音量
		float GetVoiceVolume();//获取人声音量
		const std::string& GetCurrentBGM() const { return mCurrentBGM; }//获取当前背景音乐路径（空串表示无）


	protected:
	private:
		static struct stAudioData* mData;
	};
}

#define AudioLib GameCore::MyAudioLib::Instance()

#endif