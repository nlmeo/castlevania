#include "Timer.h"

HelpTool::CTimer::CTimer() = default;

HelpTool::CTimer::~CTimer() = default;

void HelpTool::CTimer::Resume()
{
	m_paused = false;
}

void HelpTool::CTimer::Pause()
{
	m_paused = true;
}

void HelpTool::CTimer::Restart()
{
	m_pass_time = 0;
	m_shotted = false;
}

void HelpTool::CTimer::SetWaitTime(float val)
{
	m_wait_time = val;
}

void HelpTool::CTimer::SetOneShot(bool flag)
{
	m_one_shot = flag;
}

void HelpTool::CTimer::SetOnTimeout(std::function<void()> on_timeout)
{
	m_on_timeout = on_timeout;
}

void HelpTool::CTimer::OnUpdate(float delta)
{
	if (m_paused)
		return;
	m_pass_time += delta;
	if (m_pass_time > m_wait_time)
	{
		//触发条件 1. 循环触发 2. 单次触发
		bool can_shot = (!m_one_shot || (m_one_shot && !m_shotted));
		m_shotted = true;
		if (can_shot && m_on_timeout)
			m_on_timeout();
		m_pass_time -= m_wait_time;
	}
}
