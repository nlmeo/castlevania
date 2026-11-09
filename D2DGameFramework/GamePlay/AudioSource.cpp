#include "AudioSource.h"
#include "../GameCore/AudioLib.h"
#include "../../ThirdParty/bassSDK/include/bass.h"

namespace GamePlay
{

	AudioSource::~AudioSource()
	{
		_ReleaseStream();
	}


	void AudioSource::SetClip(const char* path)
	{
		if (path == nullptr) return;

		mClipPath = path;
		mIsLoaded = true;
	}


	void AudioSource::Play()
	{
		if (!AudioLib.IsInit() || mClipPath.empty()) return;

		_ReleaseStream();

		mStreamHandle = AudioLib.CreateStream(mClipPath.c_str());
		if (mStreamHandle == 0) return;

		if (mLoop)
		{
			BASS_ChannelFlags(mStreamHandle, BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP);
		}

		_ApplyVolume();
		_ApplyPitch();

		BASS_ChannelPlay(mStreamHandle, TRUE);
	}

	void AudioSource::Stop()
	{
		if (mStreamHandle != 0)
		{
			BASS_ChannelStop(mStreamHandle);
		}
	}


	void AudioSource::Pause()
	{
		if (mStreamHandle != 0)
		{
			BASS_ChannelPause(mStreamHandle);
		}
	}

	void AudioSource::UnPause()
	{
		if (mStreamHandle != 0)
		{
			BASS_ChannelPlay(mStreamHandle, FALSE);
		}
	}

	bool AudioSource::IsPlaying() const
	{
		if (mStreamHandle == 0) return false;
		DWORD state = BASS_ChannelIsActive(mStreamHandle);
		return (state == BASS_ACTIVE_PLAYING || state == BASS_ACTIVE_STALLED);
	}

	void AudioSource::SetVolume(float volume)
	{
		mVolume = volume;
		mVolume = mVolume > 1.0f ? 1.0f : mVolume;
		mVolume = mVolume < 0.0f ? 0.0f : mVolume;
		_ApplyVolume();
	}


	void AudioSource::_ApplyVolume()
	{
		if (mStreamHandle != 0)
		{
			float actualVol = mMute ? 0.0f : mVolume;
			BASS_ChannelSetAttribute(mStreamHandle, BASS_ATTRIB_VOL, actualVol);
		}
	}


	void AudioSource::SetPitch(float pitch)
	{
		mPitch = pitch;
		mPitch = mPitch > 2.0f ? 2.0f : mPitch;
		mPitch = mPitch < 0.5f ? 0.5f : mPitch;
		_ApplyPitch();
	}


	void AudioSource::_ApplyPitch()
	{
		if (mStreamHandle != 0)
		{
			float freq = 44100.0f * mPitch;
			BASS_ChannelSetAttribute(mStreamHandle, BASS_ATTRIB_FREQ, freq);
		}
	}

	void AudioSource::SetLoop(bool loop)
	{
		mLoop = loop;
		if (mStreamHandle != 0)
		{
			if (mLoop)
			{
				BASS_ChannelFlags(mStreamHandle, BASS_SAMPLE_LOOP, BASS_SAMPLE_LOOP);
			}
			else
			{
				BASS_ChannelFlags(mStreamHandle, 0, BASS_SAMPLE_LOOP);
			}
		}
	}


	void AudioSource::SetMute(bool mute)
	{
		mMute = mute;
		_ApplyVolume();
	}


	void AudioSource::SetSpatialBlend(float blend)
	{
		mSpatialBlend = blend;
		mSpatialBlend = mSpatialBlend > 1.0f ? 1.0f : mSpatialBlend;
		mSpatialBlend = mSpatialBlend < 0.0f ? 0.0f : mSpatialBlend;
	}


	float AudioSource::Time() const
	{
		if (mStreamHandle == 0) return 0.0f;
		QWORD pos = BASS_ChannelGetPosition(mStreamHandle, BASS_POS_BYTE);
		double seconds = BASS_ChannelBytes2Seconds(mStreamHandle, pos);
		return (float)seconds;
	}

	float AudioSource::Duration() const
	{
		if (mStreamHandle == 0) return 0.0f;
		QWORD len = BASS_ChannelGetLength(mStreamHandle, BASS_POS_BYTE);
		double seconds = BASS_ChannelBytes2Seconds(mStreamHandle, len);
		return (float)seconds;
	}

	void AudioSource::_ReleaseStream()
	{
		if (mStreamHandle != 0)
		{
			AudioLib.FreeStream(mStreamHandle);
			mStreamHandle = 0;
		}
	}
}