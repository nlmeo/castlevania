#include "AudioLib.h"
#include <map>
#include <string>

#include "../../ThirdParty/bassSDK/include/bass.h"

#ifdef _WIN64
#pragma comment(lib,"../ThirdParty/bassSDK/bin/x64/bass.lib")
#else
#pragma comment(lib,"../ThirdParty/bassSDK/bin/x86/bass.lib")
#endif

namespace GameCore
{
	typedef std::map<std::string, HSTREAM> ResMap;
	typedef ResMap::iterator ResMapItr;

	typedef struct stAudioData
	{
		stAudioData()
		{
			mIsInit = false;
			mMusicMap.clear();
			mSoundMap.clear();
		}

		~stAudioData()
		{
			for (auto& item : mMusicMap)
			{
				BASS_ChannelStop(item.second);
				BASS_StreamFree(item.second);
			}
			mMusicMap.clear();

			for (auto& item : mSoundMap)
			{
				BASS_ChannelStop(item.second);
				BASS_StreamFree(item.second);
			}
			mSoundMap.clear();

			if (mIsInit)
			{
				BASS_Free();
			}
			mIsInit = false;
		}

		HSTREAM mBGMHandle;
		ResMap mMusicMap;
		ResMap mSoundMap;
		bool mIsInit;
	}AUDIODATA, * LPAUDIODATA;

	stAudioData* MyAudioLib::mData;

	MyAudioLib::MyAudioLib()
	{
		mData = new struct stAudioData;
	}

	MyAudioLib::~MyAudioLib()
	{
		if (mData != nullptr)
		{
			delete mData;
			mData = nullptr;
		}
	}

	bool MyAudioLib::Initialize()
	{
		if (mData != nullptr && !mData->mIsInit)
		{
			if (BASS_Init(-1, 44100, 0, NULL, 0))
			{
				mData->mIsInit = true;
			}
			return mData->mIsInit;
		}
		return false;
	}

	bool MyAudioLib::FreeLib()
	{
		if (mData != nullptr && mData->mIsInit)
		{
			StopAllMusic();
			ReleaseAll();
			mData->mIsInit = false;
			return BASS_Free() == TRUE;
		}
		return false;
	}

	bool MyAudioLib::IsInit()
	{
		return mData != nullptr && mData->mIsInit;
	}

	bool MyAudioLib::LoadMusic(const char* path)
	{
		if (IsInit())
		{
			if (mData->mMusicMap.count(path) == 0)
			{
				HSTREAM hs = BASS_StreamCreateFile(FALSE, path, 0, 0, BASS_SAMPLE_LOOP);
				if (hs == 0)
				{
					return false;
				}
				else
				{
					mData->mMusicMap[path] = hs;
					return true;
				}
			}
			else
			{
				return true;
			}
		}
		return false;
	}

	bool MyAudioLib::LoadSound(const char* path)
	{
		if (IsInit())
		{
			if (mData->mSoundMap.count(path) == 0)
			{
				HSTREAM hs = BASS_StreamCreateFile(FALSE, path, 0, 0, BASS_SAMPLE_MONO);
				if (hs == 0)
				{
					return false;
				}
				else
				{
					mData->mSoundMap[path] = hs;
					return true;
				}
			}
			else
			{
				return true;
			}
		}
		return false;
	}

	bool MyAudioLib::EraseMusic(const char* path)
	{
		if (IsInit())
		{
			ResMapItr itr = mData->mMusicMap.find(path);
			if (itr != mData->mMusicMap.end())
			{
				BASS_ChannelStop(itr->second);
				BASS_StreamFree(itr->second);
				mData->mMusicMap.erase(itr);
				return true;
			}
		}
		return false;
	}

	bool MyAudioLib::EraseSound(const char* path)
	{
		if (IsInit())
		{
			ResMapItr itr = mData->mSoundMap.find(path);
			if (itr != mData->mSoundMap.end())
			{
				BASS_ChannelStop(itr->second);
				BASS_StreamFree(itr->second);
				mData->mSoundMap.erase(itr);
				return true;
			}
		}
		return false;
	}

	bool MyAudioLib::ReleaseAllMusic()
	{
		if (IsInit())
		{
			for (auto& item : mData->mMusicMap)
			{
				BASS_ChannelStop(item.second);
				BASS_StreamFree(item.second);
			}
			mData->mMusicMap.clear();
			return true;
		}
		return false;
	}

	bool MyAudioLib::ReleaseAllSound()
	{
		if (IsInit())
		{
			for (auto& item : mData->mSoundMap)
			{
				BASS_ChannelStop(item.second);
				BASS_StreamFree(item.second);
			}
			mData->mSoundMap.clear();
			return true;
		}
		return false;
	}

	void MyAudioLib::ReleaseAll()
	{
		ReleaseAllMusic();
		ReleaseAllSound();
	}

	bool MyAudioLib::PlayMusic(const char* path, bool restart)
	{
		if (IsInit())
		{
			ResMapItr itr = mData->mMusicMap.find(path);
			if (itr != mData->mMusicMap.end())
			{
				return BASS_ChannelPlay(itr->second, restart ? TRUE : FALSE) == TRUE;
			}
		}
		return false;
	}

	bool MyAudioLib::PlaySoundEx(const char* path)
	{
		if (IsInit())
		{
			ResMapItr itr = mData->mSoundMap.find(path);
			if (itr != mData->mSoundMap.end())
			{
				return BASS_ChannelPlay(itr->second, TRUE) == TRUE;
			}
		}
		return false;
	}

	bool MyAudioLib::PauseMusic(const char* path)
	{
		if (IsInit())
		{
			ResMapItr itr = mData->mMusicMap.find(path);
			if (itr != mData->mMusicMap.end())
			{
				return BASS_ChannelPause(itr->second);
			}
		}
		return false;
	}

	void MyAudioLib::StopMusic(const char* path)
	{
		if (IsInit())
		{
			ResMapItr itr = mData->mMusicMap.find(path);
			if (itr != mData->mMusicMap.end())
			{
				BASS_ChannelStop(itr->second);
			}
		}
	}

	void MyAudioLib::StopAllMusic()
	{
		mCurrentBGM.clear();
		if (IsInit())
		{
			for (auto& i : mData->mMusicMap)
			{
				BASS_ChannelStop(i.second);
			}
		}
	}

	float MyAudioLib::GetVolume()
	{
		if (IsInit())
		{
			DWORD vol = BASS_GetConfig(BASS_CONFIG_GVOL_STREAM);
			return vol / 10000.0f;
		}
		return -1.0f;
	}

	void MyAudioLib::SetVolume(float v)
	{
		if (IsInit())
		{
			v = v > 1.0f ? 1.0f : v;
			v = v < 0 ? 0 : v;
			BASS_SetConfig(BASS_CONFIG_GVOL_STREAM, DWORD(v * 10000));
		}
	}

	unsigned long MyAudioLib::CreateStream(const char* path)
	{
		if (IsInit())
		{
			HSTREAM hs = BASS_StreamCreateFile(FALSE, path, 0, 0, 0);
			return hs;
		}
		return 0;
	}

	void MyAudioLib::FreeStream(unsigned long handle)
	{
		if (handle != 0)
		{
			BASS_ChannelStop(handle);
			BASS_StreamFree(handle);
		}
	}
	bool MyAudioLib::PlayBGM(const char* path, bool restart)
	{
		if (IsInit())
		{
			ResMapItr itr = mData->mMusicMap.find(path);
			if(itr == mData->mMusicMap.end())
			{
				if (LoadMusic(path))
				{
					itr = mData->mMusicMap.find(path);
				}
				else
				{
					return false;
				}
			}

			mData->mBGMHandle = itr->second;
			BASS_ChannelSetAttribute(mData->mBGMHandle, BASS_ATTRIB_VOL, m_bgmVolume);
			mCurrentBGM = path;
			return BASS_ChannelPlay(itr->second, restart ? TRUE : FALSE) == TRUE;
			
		}
		return false;
	}
	bool MyAudioLib::PlayVoice(const char* path)
	{
		if (IsInit())
		{
			ResMapItr itr = mData->mSoundMap.find(path);
			if (itr == mData->mSoundMap.end())
			{
				if (LoadSound(path))
				{
					itr = mData->mSoundMap.find(path);
				}
				else
				{
					return false;
				}
			}

			
			//设置为人声的音量
			BASS_ChannelSetAttribute(itr->second, BASS_ATTRIB_VOL, m_voiceVolume);
			return BASS_ChannelPlay(itr->second, TRUE) == TRUE;
			
		}
		return false;
	}
	bool MyAudioLib::PlaySE(const char* path)
	{
		if (IsInit())
		{
			ResMapItr itr = mData->mSoundMap.find(path);
			if (itr == mData->mSoundMap.end())
			{
				if (LoadSound(path))
				{
					itr = mData->mSoundMap.find(path);
				}
				else
				{
					return false;
				}
			}
			
			//设置为音效的音量
			BASS_ChannelSetAttribute(itr->second, BASS_ATTRIB_VOL, m_soundVolume);
			return BASS_ChannelPlay(itr->second, TRUE) == TRUE;
			
		}
		return false;
	}
	void MyAudioLib::SetBGMVolume(float v)
	{
		if (IsInit())
		{
			//对音量进行一个标准化的处理
			v = v > 1.0f ? 1.0f : v;
			v = v < 0 ? 0 : v;
			m_bgmVolume = v;
		}
		//设置为BGM的音量
		if (mData->mBGMHandle)
		{
			BASS_ChannelSetAttribute(mData->mBGMHandle, BASS_ATTRIB_VOL, m_bgmVolume);
		}
	}
	void MyAudioLib::SetSEVolume(float v)
	{
		if (IsInit())
		{
			//对音量进行一个标准化的处理
			v = v > 1.0f ? 1.0f : v;
			v = v < 0 ? 0 : v;
			m_soundVolume = v;
		}
	}
	void MyAudioLib::SetVoiceVolume(float v)
	{
		if (IsInit())
		{
			//对音量进行一个标准化的处理
			v = v > 1.0f ? 1.0f : v;
			v = v < 0 ? 0 : v;
			m_voiceVolume = v;
		}
	}
	float MyAudioLib::GetBGMVolume()
	{
		return m_bgmVolume;
	}
	float MyAudioLib::GetSEVolume()
	{
		return m_soundVolume;
	}
	float MyAudioLib::GetVoiceVolume()
	{
		return m_voiceVolume;
	}
}
