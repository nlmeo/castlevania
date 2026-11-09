#include "Animator.h"
#include "GameObject.h"
#include "SpriteRender.h"
#include <algorithm>
#include <cmath>
bool GamePlay::AnimationClip::AddFrames(const std::vector<std::tuple<std::wstring, MathType::vec2f, UINT, UINT>> tasks)
{
	if (tasks.empty())
		return false;

	for (const auto& task : tasks)
	{
		for(int i = 0; i < std::get<2>(task); ++i)
		{
			AF frame;
			frame.texture = TexMgr.LoadTextureWithoutColor(std::get<0>(task).data(),std::get<3>(task));
			
			if (!frame.texture)
			{
				return false;
			}
				
			frame.anchor = std::get<1>(task);
			frame.isUseSpiritRegion = false;
			frame.start = { 0,0 };
			frame.size = { 0,0 };
			m_frames.push_back(frame);
		}
	}
	return true;
}

bool GamePlay::AnimationClip::AddFrames(const std::vector<std::tuple<std::wstring, float,float, UINT, UINT>> tasks)
{
	if (tasks.empty())
		return false;

	for (const auto& task : tasks)
	{
		for (int i = 0; i < std::get<3>(task); ++i)
		{
			AF frame;
			frame.texture = TexMgr.LoadTextureWithoutColor(std::get<0>(task).data(), std::get<4>(task));
			if (!frame.texture)
			{
				return false;
			}
			
			float anchorX = -(std::get<1>(task)/frame.texture->GetWidth()) + 0.5f;
			float anchorY = -(std::get<2>(task)/frame.texture->GetHeight()) + 0.5f;

			frame.anchor = {anchorX, anchorY};
			frame.isUseSpiritRegion = false;
			frame.start = { 0,0 };
			frame.size = { 0,0 };
			m_frames.push_back(frame);
		}
	}
	return true;
}



int GamePlay::AnimationClip::GetFrameIndex(float time) const
{
	if(m_frames.empty())
		return 0;	

	const float total = GetTotalDuration();
	if (total <= 0 || !std::isfinite(time)) return 0;
	const float safeTime = (std::max)(0.0f, time);
	const float dur = m_isLoop ? std::fmod(safeTime, total) : (std::min)(safeTime, total);
	return (std::min)(static_cast<int>(m_frames.size()) - 1, static_cast<int>(dur * m_frameRate));
}

float GamePlay::AnimationClip::GetTotalDuration() const 
{
	return m_frames.empty() || m_frameRate <= 0 ? 0 : m_frames.size() / m_frameRate;
}


GamePlay::Animator::Animator(GameObject* pObj)
	: RenderComponent(pObj)
{
	mType = COMTYPE::ComType_Animator;
	m_currentClip = nullptr;
	m_currentTime = 0.0f;

	m_render = new SpriteRender(pObj);

	
}

GamePlay::Animator::~Animator()
{
	delete m_render;
}

void GamePlay::Animator::AddClip(const wchar_t* clipName, const AnimationClip& clip)
{
	m_clips[clipName] = clip;	
}

void GamePlay::Animator::AddClipV(const std::vector<std::pair<const wchar_t*, AnimationClip>>& clips)
{
	for (const auto& pair : clips)
	{
		m_clips[pair.first] = pair.second;
	}
}



void GamePlay::Animator::Play(const wchar_t* clipName)
{
	auto it = m_clips.find(clipName);
	m_currentClip = it != m_clips.end() ? &it->second : nullptr;
	m_currentAnikey = clipName;
	m_currentTime = 0.0f;
	m_status = AS::AS_PLAYING;
	Update(0);
}

void GamePlay::Animator::Continue(const wchar_t* clipName)
{
	auto it = m_clips.find(clipName);
	if (it == m_clips.end()) { m_currentClip = nullptr; m_status = AS::AS_STOPPED; return; }
	if (it != m_clips.end())
	{
		m_currentClip = &it->second;
		m_currentAnikey = clipName;
	}

	if (m_currentTime > m_currentClip->GetTotalDuration())
	{
		m_currentTime = 0.0f;
	}
	m_status = AS::AS_PLAYING;
	Update(0);
}

void GamePlay::Animator::Stop()
{
	m_status = AS::AS_STOPPED;
}

void GamePlay::Animator::Pause()
{
	m_status = AS::AS_PAUSED;
}

void GamePlay::Animator::RePlay()
{
	if(m_currentClip == nullptr)
		return;
	m_currentTime = 0.0f;
	m_status = AS::AS_PLAYING;
}

GamePlay::AS GamePlay::Animator::GetState()
{
	return m_status;
}


void GamePlay::Animator::Update(float delta)
{
	if (!m_currentClip || m_currentClip->m_frames.empty()) { m_status = AS::AS_STOPPED; return; }
	if (m_status != AS::AS_PLAYING) return;


	m_currentTime += delta;
	int curFrameIndex = m_currentClip->GetFrameIndex(m_currentTime);
	AF& frame = m_currentClip->m_frames[curFrameIndex];


	if (frame.texture != nullptr)
	{
		m_render->SetTexture(frame.texture);
		m_render->SetAnchor(frame.anchor.x,frame.anchor.y);
		
		if (frame.isUseSpiritRegion)
		{
			m_render->SetSrcRect(frame.start.x, frame.start.y, frame.size.x, frame.size.y);
		}
		
	}


	if (m_status == AS::AS_PAUSED)
		return;

	if (m_currentTime >= m_currentClip->GetTotalDuration())
	{
		if (m_currentClip->m_isLoop)
		{
			m_currentTime = 0;
		}
		else
		{
			m_status = AS::AS_STOPPED;
		}
	}
}

const int& GamePlay::Animator::OrderInLayer() const
{
	return m_render->OrderInLayer();
}

void GamePlay::Animator::Render(const MathType::Mat3x3f& vpv)
{
	return m_render->Render(vpv);
}

const MathType::RectF& GamePlay::Animator::GetRenderRect() const
{
	return m_render->GetRenderRect();
}

void GamePlay::Animator::CaclRenderMatrix()
{
	m_render->CaclRenderMatrix();
}

void GamePlay::Animator::SetOrderInLayer(int layerorder)
{
	if(m_render)
		m_render->SetOrderInLayer(layerorder);
}

void GamePlay::Animator::SetOpcity(float op)
{
	m_render->SetOpcity(op);
}

float GamePlay::Animator::Opacity() const
{
	 return m_render->Opacity(); 
}

void GamePlay::Animator::SetTintColor(float r, float g, float b, float a)
{
	m_render->SetTintColor(r, g, b, a);
}

void GamePlay::Animator::OnDeBugPlay()
{
	if(m_clips.empty())
		return;
	if(m_currentClip == nullptr)
	{
		auto it = m_clips.begin();
		m_currentClip = &it->second;
		Play(it->first.c_str());
	}
	else 
	{
		auto it = m_clips.begin();

		while (it != m_clips.end())
		{
			if (&(it->second) == m_currentClip)
			{
				++it;
				if (it == m_clips.end())
				{
					it = m_clips.begin();
				}
				m_currentClip = &it->second;
				Play(it->first.c_str());
				break;
			}
			++it;
		}
	}
}

const GamePlay::AC& GamePlay::Animator::GetCurrentClip() const
{
	static const AC empty;
	return m_currentClip ? *m_currentClip : empty;
}

const GamePlay::AF& GamePlay::Animator::GetCurrentFrame() const
{
	static const AF empty{};
	if (!m_currentClip || m_currentClip->m_frames.empty()) return empty;
	int curFrameIndex = m_currentClip->GetFrameIndex(m_currentTime);
	AF& frame = m_currentClip->m_frames[curFrameIndex];
	return frame;
}

const std::wstring& GamePlay::Animator::GetCurrentAniKey() const
{
	return m_currentAnikey;
}

int GamePlay::Animator::GetCurrentFrameIndex() const
{
	if(m_currentClip)
		return m_currentClip->GetFrameIndex(m_currentTime);
	return 0;
}

