#pragma once
#ifndef __MY_TIMER_H__
#define __MY_TIMER_H__
#include <functional>
namespace HelpTool
{
	class CTimer
	{
	private:
		float m_pass_time = 0; //经过的时间
		float m_wait_time = 0; //等待触发的时间点
		bool m_paused = false;
		bool m_shotted = false;   //是否已经命中，与 one_shot并用
		bool m_one_shot = false;  //开启为一次性计时，不开启为循环计时
		std::function<void()> m_on_timeout;
	public:
		CTimer();
		~CTimer();
		void Resume();
		void Pause();
		void Restart();
		void SetWaitTime(float val);
		void SetOneShot(bool flag);
		void SetOnTimeout(std::function<void()> on_timeout);
		void OnUpdate(float delta);
	};
	
}


#endif // !__MY_TIMER_H__
