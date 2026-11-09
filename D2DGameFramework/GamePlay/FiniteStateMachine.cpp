#include "FiniteStateMachine.h"

GamePlay::FiniteStateMachine::FiniteStateMachine(GameObject* pObj)
	:Component(pObj)
{
	mType = COMTYPE::ComType_FSM;
	m_currentState = nullptr;
	m_nextState = nullptr;
}

GamePlay::FiniteStateMachine::~FiniteStateMachine()
{
	std::unordered_map<std::wstring, GamePlay::LPSN>::iterator it = m_states.begin();

	while (it != m_states.end())
	{
		GamePlay::LPSN pState = it->second;
		delete pState;
		it++;
	}
}

void GamePlay::FiniteStateMachine::InitState(const wchar_t* entry_state)
{
	
	if (auto* entry = GetState(entry_state))
	{
		m_currentState = entry;
		m_nextState = nullptr;
		entry->OnEnter();
	}
}

void GamePlay::FiniteStateMachine::Update(float delta_time)
{
	//切换逻辑
	auto* next = m_nextState;
	m_nextState = nullptr;
	if (next != nullptr && next != m_currentState && next->CanEnter())
	{
		if (m_currentState) m_currentState->OnQuit();
		next->OnEnter();
		//后置状态成员切换，下一个状态进入时可以获取上一个状态值
		m_currentState = next;
	}

	if (m_currentState) m_currentState->OnUpdate(delta_time);
}

bool GamePlay::FiniteStateMachine::SetNextState(const wchar_t* next_state)
{
	auto* next = GetState(next_state);
	if (!next) return false;
	m_nextState = next;
	return true;
}

void GamePlay::FiniteStateMachine::RegisterState(const wchar_t* state, GamePlay::LPSN pstate)
{
	pstate->SetOwner(gameobject());
	m_states[state] = pstate;
}

GamePlay::LPSN GamePlay::FiniteStateMachine::GetCurrentState() const
{
	return m_currentState;
}

GamePlay::LPSN GamePlay::FiniteStateMachine::GetNextState() const
{
	return m_nextState;
}

GamePlay::LPSN GamePlay::FiniteStateMachine::GetState(const wchar_t* state) const
{
	if (!state) return nullptr;
	return m_states.find(state) != m_states.end() ? m_states.at(state) : nullptr;
}

bool GamePlay::FiniteStateMachine::IsStateExist(const wchar_t* state) const
{
	return m_states.find(state) != m_states.end();
}
