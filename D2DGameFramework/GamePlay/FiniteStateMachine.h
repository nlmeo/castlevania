#pragma once
#ifndef __MY_FSM_STATE_MACHINE_H__
#define __MY_FSM_STATE_MACHINE_H__

#include <string>
#include <unordered_map>
#include "Component.h"
namespace GamePlay
{
	typedef class StateNode
	{
	public:
		virtual ~StateNode() = default;
		/** @brief 提交切换时校验并支付消耗，失败则保留当前状态。 */
		virtual bool CanEnter() { return true; }
		virtual void OnEnter() = 0;  
		virtual void OnUpdate(float delta_time = 0) = 0; 
		virtual void OnQuit() = 0; 
		virtual void SetOwner(class GameObject* pObj) = 0;
	} SN,*LPSN;


	typedef class FiniteStateMachine : public Component
	{
	public:
		FiniteStateMachine(class GameObject* pObj);
		virtual ~FiniteStateMachine();
		void InitState(const wchar_t* entry_state);
		void Update(float delta_time);                            //每帧时间
		bool SetNextState(const wchar_t* next_state);
		void RegisterState(const wchar_t* state, GamePlay::LPSN pstate);
		GamePlay::LPSN GetCurrentState() const;
		GamePlay::LPSN GetNextState() const;
		GamePlay::LPSN GetState(const wchar_t* state) const;
		bool IsStateExist(const wchar_t* state) const;
	private:
		std::unordered_map<std::wstring, GamePlay::LPSN> m_states; //状态列表
		GamePlay::LPSN m_currentState = nullptr; //当前状态
		GamePlay::LPSN m_nextState = nullptr; //下一个状态
	} FSM, * LPFSM;


}
#endif // 
