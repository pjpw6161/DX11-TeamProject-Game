#include "Timer_Manager.h"
#include "Timer.h"
#include "GameInstance.h"

CTimer_Manager::CTimer_Manager()
{
}

HRESULT CTimer_Manager::Add_Timer(const wstring& strTimerTag)
{
	if (Find_Timer(strTimerTag))
	{
		return E_FAIL;
	}

	m_Timers.emplace(strTimerTag, CTimer::Create());

	return S_OK;
}

_float CTimer_Manager::Compute_TimeDelta(const wstring& strTimerTag)
{
	CTimer* pTimer = Find_Timer(strTimerTag);

	if (!pTimer)
	{
		return 0.f;
	}

	return pTimer->Compute_TimeDelta();
}

CTimer* CTimer_Manager::Find_Timer(const wstring& strTimerTag)
{
	auto it = m_Timers.find(strTimerTag);

	if (it == m_Timers.end())
	{
		return nullptr;
	}

	return it->second;
}

CTimer_Manager* CTimer_Manager::Create()
{
	CTimer_Manager* pInstance = new CTimer_Manager();

	return pInstance;
}

void CTimer_Manager::Free()
{
	for (auto& Pair : m_Timers)
	{
		Safe_Release(Pair.second);
	}
	m_Timers.clear();
}
