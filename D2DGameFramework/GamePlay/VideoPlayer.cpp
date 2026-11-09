#include "GameObject.h"
#include "../D2DCore/D2DRender.h"
#include "../D2DCore/TextureManager.h"
#include <algorithm>
#include <string>
#include <cstdint>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

#include "../../ThirdParty/bassSDK/include/bass.h"

#ifdef _WIN64
#pragma comment(lib, "../ThirdParty/FFmpeg/bin/x64/avformat.lib")
#pragma comment(lib, "../ThirdParty/FFmpeg/bin/x64/avcodec.lib")
#pragma comment(lib, "../ThirdParty/FFmpeg/bin/x64/avutil.lib")
#pragma comment(lib, "../ThirdParty/FFmpeg/bin/x64/swscale.lib")
#pragma comment(lib, "../ThirdParty/FFmpeg/bin/x64/swresample.lib")
#else
#pragma comment(lib, "../ThirdParty/FFmpeg/lib/msvc/x86/avformat.lib")
#pragma comment(lib, "../ThirdParty/FFmpeg/lib/msvc/x86/avcodec.lib")
#pragma comment(lib, "../ThirdParty/FFmpeg/lib/msvc/x86/avutil.lib")
#pragma comment(lib, "../ThirdParty/FFmpeg/lib/msvc/x86/swscale.lib")
#pragma comment(lib, "../ThirdParty/FFmpeg/lib/msvc/x86/swresample.lib")
#endif

namespace GamePlay
{

struct VideoPlayerInternal
{
	AVFormatContext* formatCtx;
	AVCodecContext* videoCodecCtx;
	AVCodecContext* audioCodecCtx;
	const AVCodec* videoCodec;
	const AVCodec* audioCodec;
	SwsContext* swsCtx;
	SwrContext* swrCtx;
	AVFrame* frame;
	AVFrame* pendingFrame;
	AVFrame* rgbFrame;
	AVPacket* packet;
	int videoStreamIndex;
	int audioStreamIndex;
	uint8_t* rgbBuffer;
	int rgbBufferSize;
	double timeBase;
	int64_t videoStartTime;
	bool seekRequested;
	int64_t seekTarget;
	uint8_t* audioOutBuffer;
	int audioOutBufferSize;
	int64_t lastVideoPts;
	bool hasPendingVideo;

	VideoPlayerInternal()
	{
		memset(this, 0, sizeof(*this));
		videoStreamIndex = -1;
		audioStreamIndex = -1;
		lastVideoPts = AV_NOPTS_VALUE;
		videoStartTime = AV_NOPTS_VALUE;
		hasPendingVideo = false;
	}
};

	VideoPlayer::VideoPlayer(class GameObject* pObj) : RenderComponent(pObj)
	{
		mType = COMTYPE::ComType_VideoPlayer;

		mVideoPath = "";
		mIsPlaying = false;
		mIsPaused = false;
		mVolume = 1.0f;
		mMute = false;
		mLoop = false;

		mWidth = 0;
		mHeight = 0;
		mFrameRate = 30.0f;
		mDuration = 0.0f;

		mCurrentFrame = nullptr;

		mCurrentTime = 0.0;
		mCurrentFrameIndex = 0;

		mInternal = new VideoPlayerInternal();
		mHasVideo = false;
		mHasAudio = false;
		mAudioStreamHandle = 0;
		mPauseAccumulated = 0.0;
		mNewFrameReady = false;
		mStartTime.QuadPart = 0;
		mPauseTime.QuadPart = 0;

		mAnchor = { 0.5f, 0.5f };
		mSrcRect = {};
		mOpacity = 1.0f;
		mOrderInLayer = 0;
		mTransformMatrix.Identity();

		mFrameTextureKey = L"__vp_frame_" + std::to_wstring(reinterpret_cast<uintptr_t>(this));
	}

	VideoPlayer::~VideoPlayer()
	{
		Stop();
		CloseVideo();

		if (mCurrentFrame) {
			TexMgr.UnloadTexture(mFrameTextureKey.c_str());
			mCurrentFrame = nullptr;
		}

		VideoPlayerInternal* ctx = static_cast<VideoPlayerInternal*>(mInternal);
		if (ctx) {
			delete ctx;
			mInternal = nullptr;
		}
	}

	bool VideoPlayer::OpenVideo(const char* path)
	{
		VideoPlayerInternal* ctx = static_cast<VideoPlayerInternal*>(mInternal);
		if (!ctx || !path || !*path) return false;

		CloseVideo();

		int ret = avformat_open_input(&ctx->formatCtx, path, nullptr, nullptr);
		if (ret < 0) return false;

		ret = avformat_find_stream_info(ctx->formatCtx, nullptr);
		if (ret < 0) {
			avformat_close_input(&ctx->formatCtx);
			ctx->formatCtx = nullptr;
			return false;
		}

		for (unsigned int i = 0; i < ctx->formatCtx->nb_streams; i++) {
			AVCodecParameters* par = ctx->formatCtx->streams[i]->codecpar;
			if (par->codec_type == AVMEDIA_TYPE_VIDEO && ctx->videoStreamIndex < 0) {
				ctx->videoStreamIndex = i;
			}
			else if (par->codec_type == AVMEDIA_TYPE_AUDIO && ctx->audioStreamIndex < 0) {
				ctx->audioStreamIndex = i;
			}
		}

		if (ctx->videoStreamIndex >= 0) {
			AVCodecParameters* par = ctx->formatCtx->streams[ctx->videoStreamIndex]->codecpar;
			ctx->videoCodec = avcodec_find_decoder(par->codec_id);
			if (ctx->videoCodec) {
				ctx->videoCodecCtx = avcodec_alloc_context3(ctx->videoCodec);
				avcodec_parameters_to_context(ctx->videoCodecCtx, par);
				ret = avcodec_open2(ctx->videoCodecCtx, ctx->videoCodec, nullptr);
				if (ret < 0) {
					avcodec_free_context(&ctx->videoCodecCtx);
					ctx->videoStreamIndex = -1;
				}
				else {
					mWidth = ctx->videoCodecCtx->width;
					mHeight = ctx->videoCodecCtx->height;

					AVRational frameRate = ctx->formatCtx->streams[ctx->videoStreamIndex]->avg_frame_rate;
					if (frameRate.num > 0 && frameRate.den > 0) {
						mFrameRate = static_cast<float>(frameRate.num) / frameRate.den;
					}
					else {
						mFrameRate = 30.0f;
					}

					if (ctx->formatCtx->duration != AV_NOPTS_VALUE) {
						mDuration = static_cast<float>(ctx->formatCtx->duration) / AV_TIME_BASE;
					}
					else {
						AVStream* stream = ctx->formatCtx->streams[ctx->videoStreamIndex];
						if (stream->duration != AV_NOPTS_VALUE) {
							mDuration = static_cast<float>(stream->duration * av_q2d(stream->time_base));
						}
					}

					ctx->timeBase = av_q2d(ctx->formatCtx->streams[ctx->videoStreamIndex]->time_base);

					ctx->swsCtx = sws_getContext(
						mWidth, mHeight, ctx->videoCodecCtx->pix_fmt,
						mWidth, mHeight, AV_PIX_FMT_BGRA,
						SWS_BILINEAR, nullptr, nullptr, nullptr);

					ctx->rgbFrame = av_frame_alloc();
					ctx->rgbBufferSize = av_image_get_buffer_size(AV_PIX_FMT_BGRA, mWidth, mHeight, 1);
					ctx->rgbBuffer = static_cast<uint8_t*>(av_malloc(ctx->rgbBufferSize));
					av_image_fill_arrays(ctx->rgbFrame->data, ctx->rgbFrame->linesize,
						ctx->rgbBuffer, AV_PIX_FMT_BGRA, mWidth, mHeight, 1);

					mHasVideo = true;
				}
			}
			else {
				ctx->videoStreamIndex = -1;
			}
		}

		if (ctx->audioStreamIndex >= 0) {
			AVCodecParameters* par = ctx->formatCtx->streams[ctx->audioStreamIndex]->codecpar;
			ctx->audioCodec = avcodec_find_decoder(par->codec_id);
			if (ctx->audioCodec) {
				ctx->audioCodecCtx = avcodec_alloc_context3(ctx->audioCodec);
				avcodec_parameters_to_context(ctx->audioCodecCtx, par);
				ret = avcodec_open2(ctx->audioCodecCtx, ctx->audioCodec, nullptr);
				if (ret < 0) {
					avcodec_free_context(&ctx->audioCodecCtx);
					ctx->audioStreamIndex = -1;
				}
				else {
					AVChannelLayout outLayout;
					av_channel_layout_default(&outLayout, 2);
					swr_alloc_set_opts2(&ctx->swrCtx,
						&outLayout, AV_SAMPLE_FMT_S16, 44100,
						&ctx->audioCodecCtx->ch_layout, ctx->audioCodecCtx->sample_fmt,
						ctx->audioCodecCtx->sample_rate,
						0, nullptr);
					swr_init(ctx->swrCtx);

					ctx->audioOutBufferSize = 192000;
					ctx->audioOutBuffer = static_cast<uint8_t*>(av_malloc(ctx->audioOutBufferSize));

					bool bassAvailable = false;
					if (BASS_Init(-1, 44100, 0, nullptr, nullptr)) {
						bassAvailable = true;
					}
					else if (BASS_ErrorGetCode() == BASS_ERROR_ALREADY) {
						bassAvailable = true;
					}

					if (bassAvailable) {
						mAudioStreamHandle = BASS_StreamCreate(44100, 2, 0, STREAMPROC_PUSH, nullptr);
						if (mAudioStreamHandle) {
							mHasAudio = true;
							BASS_ChannelSetAttribute(mAudioStreamHandle, BASS_ATTRIB_VOL,
								mMute ? 0.0f : mVolume);
						}
					}
				}
			}
			else {
				ctx->audioStreamIndex = -1;
			}
		}

		ctx->frame = av_frame_alloc();
		ctx->pendingFrame = av_frame_alloc();
		ctx->packet = av_packet_alloc();

		return mHasVideo || mHasAudio;
	}

	void VideoPlayer::CloseVideo()
	{
		VideoPlayerInternal* ctx = static_cast<VideoPlayerInternal*>(mInternal);
		if (!ctx) return;

		if (mAudioStreamHandle) {
			BASS_StreamFree(mAudioStreamHandle);
			mAudioStreamHandle = 0;
		}

		if (ctx->swrCtx) { swr_free(&ctx->swrCtx); ctx->swrCtx = nullptr; }
		if (ctx->audioOutBuffer) { av_free(ctx->audioOutBuffer); ctx->audioOutBuffer = nullptr; }
		ctx->audioOutBufferSize = 0;

		if (ctx->swsCtx) { sws_freeContext(ctx->swsCtx); ctx->swsCtx = nullptr; }
		if (ctx->rgbBuffer) { av_free(ctx->rgbBuffer); ctx->rgbBuffer = nullptr; }
		ctx->rgbBufferSize = 0;

		if (ctx->rgbFrame) { av_frame_free(&ctx->rgbFrame); }
		if (ctx->pendingFrame) { av_frame_free(&ctx->pendingFrame); }
		if (ctx->frame) { av_frame_free(&ctx->frame); }
		if (ctx->packet) { av_packet_free(&ctx->packet); }

		if (ctx->videoCodecCtx) { avcodec_free_context(&ctx->videoCodecCtx); }
		if (ctx->audioCodecCtx) { avcodec_free_context(&ctx->audioCodecCtx); }

		if (ctx->formatCtx) { avformat_close_input(&ctx->formatCtx); }

		ctx->videoStreamIndex = -1;
		ctx->audioStreamIndex = -1;
		ctx->videoCodec = nullptr;
		ctx->audioCodec = nullptr;
		ctx->lastVideoPts = AV_NOPTS_VALUE;
		ctx->hasPendingVideo = false;

		mHasVideo = false;
		mHasAudio = false;
		mWidth = 0;
		mHeight = 0;
		mFrameRate = 30.0f;
		mDuration = 0.0f;
	}

	void VideoPlayer::DecodeAudioPacket()
	{
		VideoPlayerInternal* ctx = static_cast<VideoPlayerInternal*>(mInternal);
		if (!mHasAudio || !mAudioStreamHandle || !ctx->audioCodecCtx) return;

		int ret = avcodec_send_packet(ctx->audioCodecCtx, ctx->packet);
		if (ret < 0) return;

		while (ret >= 0) {
			ret = avcodec_receive_frame(ctx->audioCodecCtx, ctx->frame);
			if (ret < 0) break;

			uint8_t* outData[1] = { ctx->audioOutBuffer };
			int maxOutSamples = ctx->audioOutBufferSize / 4;
			int outSamples = swr_convert(ctx->swrCtx,
				outData, maxOutSamples,
				(const uint8_t**)ctx->frame->data,
				ctx->frame->nb_samples);

			if (outSamples > 0) {
				int outBytes = outSamples * 4;
				BASS_StreamPutData(mAudioStreamHandle, ctx->audioOutBuffer, outBytes);
			}
		}
	}

	void VideoPlayer::ProcessPackets()
	{
		VideoPlayerInternal* ctx = static_cast<VideoPlayerInternal*>(mInternal);
		if (!ctx->formatCtx) return;

		double targetTime = mCurrentTime;

		if (ctx->hasPendingVideo) {
			double pts = 0;
			if (ctx->pendingFrame->pts != AV_NOPTS_VALUE) {
				pts = ctx->pendingFrame->pts * ctx->timeBase;
			}
			else if (ctx->pendingFrame->best_effort_timestamp != AV_NOPTS_VALUE) {
				pts = ctx->pendingFrame->best_effort_timestamp * ctx->timeBase;
			}

			if (pts <= targetTime + 0.001) {
				sws_scale(ctx->swsCtx,
					ctx->pendingFrame->data, ctx->pendingFrame->linesize,
					0, mHeight,
					ctx->rgbFrame->data, ctx->rgbFrame->linesize);

				int rowBytes = mWidth * 4;
				uint8_t* top = ctx->rgbBuffer;
				uint8_t* bot = ctx->rgbBuffer + (mHeight - 1) * rowBytes;
				while (top < bot) {
					for (int x = 0; x < rowBytes; x++) {
						uint8_t tmp = top[x];
						top[x] = bot[x];
						bot[x] = tmp;
					}
					top += rowBytes;
					bot -= rowBytes;
				}

				mNewFrameReady = true;
				av_frame_unref(ctx->pendingFrame);
				ctx->hasPendingVideo = false;
			}
			else {
				return;
			}
		}

		const int MAX_PACKETS = 30;
		for (int i = 0; i < MAX_PACKETS; i++) {
			int ret = av_read_frame(ctx->formatCtx, ctx->packet);
			if (ret < 0) {
				av_packet_unref(ctx->packet);
				break;
			}

			if (ctx->packet->stream_index == ctx->audioStreamIndex) {
				DecodeAudioPacket();
				av_packet_unref(ctx->packet);
			}
			else if (ctx->packet->stream_index == ctx->videoStreamIndex) {
				ret = avcodec_send_packet(ctx->videoCodecCtx, ctx->packet);
				av_packet_unref(ctx->packet);
				if (ret == 0) {
					while (avcodec_receive_frame(ctx->videoCodecCtx, ctx->frame) == 0) {
						double pts = 0;
						if (ctx->frame->pts != AV_NOPTS_VALUE) {
							pts = ctx->frame->pts * ctx->timeBase;
						}
						else if (ctx->frame->best_effort_timestamp != AV_NOPTS_VALUE) {
							pts = ctx->frame->best_effort_timestamp * ctx->timeBase;
						}

						if (pts <= targetTime + 0.001) {
							sws_scale(ctx->swsCtx,
								ctx->frame->data, ctx->frame->linesize,
								0, mHeight,
								ctx->rgbFrame->data, ctx->rgbFrame->linesize);

							int rowBytes = mWidth * 4;
							uint8_t* top = ctx->rgbBuffer;
							uint8_t* bot = ctx->rgbBuffer + (mHeight - 1) * rowBytes;
							while (top < bot) {
								for (int x = 0; x < rowBytes; x++) {
									uint8_t tmp = top[x];
									top[x] = bot[x];
									bot[x] = tmp;
								}
								top += rowBytes;
								bot -= rowBytes;
							}

							mNewFrameReady = true;
						}
						else {
							av_frame_move_ref(ctx->pendingFrame, ctx->frame);
							ctx->hasPendingVideo = true;
							return;
						}
					}
				}
			}
			else {
				av_packet_unref(ctx->packet);
			}
		}
	}

	double VideoPlayer::CalculateCurrentTime()
	{
		if (!mIsPlaying || mIsPaused) return mCurrentTime;

		LARGE_INTEGER now, freq;
		QueryPerformanceCounter(&now);
		QueryPerformanceFrequency(&freq);
		double elapsed = static_cast<double>(now.QuadPart - mStartTime.QuadPart) / freq.QuadPart;
		return mPauseAccumulated + elapsed;
	}

	void VideoPlayer::SetVideoPath(const char* path)
	{
		if (mVideoPath != path) {
			Stop();
			CloseVideo();

			mVideoPath = path;

			if (!mVideoPath.empty()) {
				OpenVideo(mVideoPath.c_str());
			}
		}

		transform()->SetScale({1.0f,-1.0f});
	}

	void VideoPlayer::Play()
	{
		VideoPlayerInternal* ctx = static_cast<VideoPlayerInternal*>(mInternal);
		if (!ctx || !ctx->formatCtx || mVideoPath.empty())
			return;

		if (!mIsPlaying) {
			if (mIsPaused) {
				if (mAudioStreamHandle) {
					BASS_ChannelPlay(mAudioStreamHandle, FALSE);
				}
				QueryPerformanceCounter(&mStartTime);
				mIsPaused = false;
			}
			else {
				if (mAudioStreamHandle) {
					BASS_ChannelPlay(mAudioStreamHandle, TRUE);
				}
				mCurrentTime = 0.0;
				mCurrentFrameIndex = 0;
				mPauseAccumulated = 0.0;
				mNewFrameReady = false;
				QueryPerformanceCounter(&mStartTime);
			}

			mIsPlaying = true;
			NotifyOnPlay();
		}
	}

	void VideoPlayer::Pause()
	{
		VideoPlayerInternal* ctx = static_cast<VideoPlayerInternal*>(mInternal);
		if (ctx && mIsPlaying && !mIsPaused) {
			if (mAudioStreamHandle) {
				BASS_ChannelPause(mAudioStreamHandle);
			}

			LARGE_INTEGER now, freq;
			QueryPerformanceCounter(&now);
			QueryPerformanceFrequency(&freq);
			mPauseAccumulated += static_cast<double>(now.QuadPart - mStartTime.QuadPart) / freq.QuadPart;
			mIsPaused = true;
			NotifyOnPause();
		}
	}

	void VideoPlayer::Stop()
	{
		VideoPlayerInternal* ctx = static_cast<VideoPlayerInternal*>(mInternal);
		if (!ctx) return;

		if (mIsPlaying) {
			if (mAudioStreamHandle) {
				BASS_ChannelStop(mAudioStreamHandle);
			}

			mIsPlaying = false;
			mIsPaused = false;
			mCurrentTime = 0.0;
			mCurrentFrameIndex = 0;
			mPauseAccumulated = 0.0;

			if (ctx->formatCtx) {
				av_seek_frame(ctx->formatCtx, -1, 0, AVSEEK_FLAG_BACKWARD);
			}
			if (ctx->videoCodecCtx) {
				avcodec_flush_buffers(ctx->videoCodecCtx);
			}
			if (ctx->audioCodecCtx) {
				avcodec_flush_buffers(ctx->audioCodecCtx);
			}

			NotifyOnStop();
		}
	}

	void VideoPlayer::SetVolume(float volume)
	{
		float clampedVolume = (volume < 0.0f) ? 0.0f : (volume > 1.0f) ? 1.0f : volume;
		mVolume = clampedVolume;

		if (mAudioStreamHandle) {
			float vol = mMute ? 0.0f : mVolume;
			BASS_ChannelSetAttribute(mAudioStreamHandle, BASS_ATTRIB_VOL, vol);
		}
	}

	void VideoPlayer::SetMute(bool mute)
	{
		mMute = mute;
		SetVolume(mVolume);
	}

	void VideoPlayer::SetLoop(bool loop)
	{
		mLoop = loop;
	}

	void VideoPlayer::Seek(float time)
	{
		VideoPlayerInternal* ctx = static_cast<VideoPlayerInternal*>(mInternal);
		if (!ctx || !ctx->formatCtx || mDuration <= 0)
			return;

		float clampedTime = (time < 0.0f) ? 0.0f : (time > mDuration) ? mDuration : time;
		int64_t seekTarget = static_cast<int64_t>(clampedTime * AV_TIME_BASE);

		int ret = av_seek_frame(ctx->formatCtx, -1, seekTarget, AVSEEK_FLAG_BACKWARD);
		if (ret < 0) {
			ret = avformat_seek_file(ctx->formatCtx, -1, INT64_MIN, seekTarget, seekTarget, 0);
		}

		if (ctx->videoCodecCtx) {
			avcodec_flush_buffers(ctx->videoCodecCtx);
		}
		if (ctx->audioCodecCtx) {
			avcodec_flush_buffers(ctx->audioCodecCtx);
		}

		if (ctx->hasPendingVideo) {
			av_frame_unref(ctx->pendingFrame);
			ctx->hasPendingVideo = false;
		}

		if (mAudioStreamHandle) {
			QWORD bytePos = BASS_ChannelSeconds2Bytes(mAudioStreamHandle, clampedTime);
			BASS_ChannelSetPosition(mAudioStreamHandle, bytePos, BASS_POS_BYTE);
		}

		mCurrentTime = clampedTime;
		mCurrentFrameIndex = static_cast<int>(clampedTime * mFrameRate);
		mNewFrameReady = false;

		if (mIsPlaying && !mIsPaused) {
			mPauseAccumulated = clampedTime;
			QueryPerformanceCounter(&mStartTime);
		}
	}

	float VideoPlayer::Time() const
	{
		if (mIsPlaying && !mIsPaused) {
			const_cast<VideoPlayer*>(this)->mCurrentTime = const_cast<VideoPlayer*>(this)->CalculateCurrentTime();
		}
		return static_cast<float>(mCurrentTime);
	}

	D2DCore::Texture* VideoPlayer::GetCurrentFrame() const
	{
		return mCurrentFrame;
	}

	void VideoPlayer::CaclRenderMatrix()
	{
		auto& matrixOrder = transform()->MatOrder();
		auto& matT = transform()->MatT();
		auto& matS = transform()->MatS();
		auto& matR = transform()->MatR();

		switch (matrixOrder)
		{
		case GamePlay::Transform::MatrixOrder::RST:
		{
			mTransformMatrix.SetTranslate(-mDestRect.right * mAnchor.x, -mDestRect.bottom * mAnchor.y);
			((mTransformMatrix *= matR) *= matS) *= matT;
		}break;
		case GamePlay::Transform::MatrixOrder::SRT:
		{
			mTransformMatrix.SetTranslate(-mDestRect.right * mAnchor.x, -mDestRect.bottom * mAnchor.y);
			((mTransformMatrix *= matS) *= matR) *= matT;
		}break;
		case GamePlay::Transform::MatrixOrder::STR:
		{
			mTransformMatrix.SetTranslate(-mDestRect.right * mAnchor.x, -mDestRect.bottom * mAnchor.y);
			((mTransformMatrix *= matS) *= matT) *= matR;
		}break;
		case GamePlay::Transform::MatrixOrder::TSR:
		{
			mTransformMatrix.SetTranslate(-mDestRect.right * mAnchor.x, -mDestRect.bottom * mAnchor.y);
			((mTransformMatrix *= matT) *= matS) *= matR;
		}break;
		case GamePlay::Transform::MatrixOrder::TRS:
		{
			mTransformMatrix.SetTranslate(-mDestRect.right * mAnchor.x, -mDestRect.bottom * mAnchor.y);
			((mTransformMatrix *= matT) *= matR) *= matS;
		}break;
		case GamePlay::Transform::MatrixOrder::RTS:
		{
			mTransformMatrix.SetTranslate(-mDestRect.right * mAnchor.x, -mDestRect.bottom * mAnchor.y);
			((mTransformMatrix *= matR) *= matT) *= matS;
		}break;
		}

		MathType::vec2f corner[4] = {
			{0,0},
			{mDestRect.right,0},
			{mDestRect.right,mDestRect.bottom},
			{0,mDestRect.bottom}
		};

		float& minx = mRenderRect.left;
		float& maxx = mRenderRect.right;
		float& miny = mRenderRect.top;
		float& maxy = mRenderRect.bottom;

		for (int i = 0; i < 4; ++i)
		{
			corner[i] *= mTransformMatrix;
		}

		minx = corner[0].x; maxx = corner[0].x; miny = corner[0].y; maxy = corner[0].y;

		for (int i = 1; i < 4; ++i)
		{
			if (minx > corner[i].x) { minx = corner[i].x; }
			if (maxx < corner[i].x) { maxx = corner[i].x; }
			if (miny > corner[i].y) { miny = corner[i].y; }
			if (maxy < corner[i].y) { maxy = corner[i].y; }
		}
	}

	void VideoPlayer::Render(const MathType::Mat3x3f& vpv)
	{
		D2D1_MATRIX_3X2_F originalTransform;
		auto pD2DTarget = D2DRd.GetD2DRenderTarget();
		if (pD2DTarget != nullptr && mCurrentFrame != nullptr)
		{
			pD2DTarget->GetTransform(&originalTransform);
			MathType::Mat3x3f combinedMat = mTransformMatrix * vpv;
			float* matData = combinedMat.GetMat3x2();
			D2D1_MATRIX_3X2_F mat = D2D1::Matrix3x2F(matData[0], matData[1], matData[2], matData[3], matData[4], matData[5]);
			pD2DTarget->SetTransform(mat);
			pD2DTarget->DrawBitmap(mCurrentFrame->GetD2DBitmap(), mDestRect,
				mOpacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, mSrcRect);
			pD2DTarget->SetTransform(originalTransform);
		}
	}

	const MathType::RectF& VideoPlayer::GetRenderRect() const
	{
		return mRenderRect;
	}

	void VideoPlayer::Update()
	{
		VideoPlayerInternal* ctx = static_cast<VideoPlayerInternal*>(mInternal);
		if (!mIsPlaying || !ctx)
			return;

		if (mIsPaused) return;

		double currentTime = CalculateCurrentTime();
		if (mDuration > 0 && currentTime >= mDuration) {
			if (mLoop) {
				Seek(0.0f);
				if (mAudioStreamHandle) {
					BASS_ChannelPlay(mAudioStreamHandle, TRUE);
					mPauseAccumulated = 0.0;
					mNewFrameReady = false;
					QueryPerformanceCounter(&mStartTime);
				}
				mIsPlaying = true;
				mIsPaused = false;
				currentTime = 0.0;
			}
			else {
				Stop();
				NotifyOnEnd();
				return;
			}
		}

		mCurrentTime = currentTime;
		mCurrentFrameIndex = static_cast<int>(currentTime * mFrameRate);

		ProcessPackets();

		if (mNewFrameReady) {
			UpdateCurrentFrame();
			mNewFrameReady = false;
			NotifyOnFrame(mCurrentFrameIndex, static_cast<float>(mCurrentTime));
		}
	}

	void VideoPlayer::UpdateCurrentFrame()
	{
		VideoPlayerInternal* ctx = static_cast<VideoPlayerInternal*>(mInternal);
		if (!mHasVideo || !ctx || !ctx->rgbBuffer || mWidth <= 0 || mHeight <= 0)
			return;

		if (mCurrentFrame) {
			ID2D1Bitmap* pBitmap = mCurrentFrame->GetD2DBitmap();
			if (pBitmap) {
				D2D1_SIZE_U bmpSize = pBitmap->GetPixelSize();
				if (bmpSize.width == static_cast<UINT>(mWidth) && bmpSize.height == static_cast<UINT>(mHeight)) {
					HRESULT hr = pBitmap->CopyFromMemory(nullptr, ctx->rgbBuffer, mWidth * 4);
					if (FAILED(hr)) {
						TexMgr.UnloadTexture(mFrameTextureKey.c_str());
						mCurrentFrame = nullptr;
					}
				}
				else {
					TexMgr.UnloadTexture(mFrameTextureKey.c_str());
					mCurrentFrame = nullptr;
				}
			}
		}

		if (!mCurrentFrame) {
			mCurrentFrame = TexMgr.CreateTextureFromMemory(
				mFrameTextureKey.c_str(),
				ctx->rgbBuffer,
				mWidth, mHeight, mWidth * 4
			);
		}

		mSrcRect = { 0, 0, static_cast<float>(mWidth), static_cast<float>(mHeight) };
		mDestRect = { 0, 0, static_cast<float>(mWidth), static_cast<float>(mHeight) };
		CaclRenderMatrix();
	}

	void VideoPlayer::NotifyOnPlay()
	{
		if (mOnPlayCallback) {
			mOnPlayCallback();
		}
	}

	void VideoPlayer::NotifyOnPause()
	{
		if (mOnPauseCallback) {
			mOnPauseCallback();
		}
	}

	void VideoPlayer::NotifyOnStop()
	{
		if (mOnStopCallback) {
			mOnStopCallback();
		}
	}

	void VideoPlayer::NotifyOnEnd()
	{
		if (mOnEndCallback) {
			mOnEndCallback();
		}
	}

	void VideoPlayer::NotifyOnFrame(int frameIndex, float time)
	{
		if (mOnFrameCallback) {
			mOnFrameCallback(frameIndex, time);
		}
	}

}
