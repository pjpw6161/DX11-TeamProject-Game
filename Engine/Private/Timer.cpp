#include "Timer.h"

CTimer::CTimer()
{
}

HRESULT CTimer::Init()
{
	QueryPerformanceCounter(&m_CurrentTime);
	QueryPerformanceCounter(&m_OldTime);
	QueryPerformanceCounter(&m_OriginTime);

	QueryPerformanceFrequency(&m_CpuTick);

	return S_OK;
}

_float CTimer::Compute_TimeDelta()
{
	QueryPerformanceCounter(&m_CurrentTime);

	m_fTimeDelta = static_cast<_float>(m_CurrentTime.QuadPart - m_OldTime.QuadPart) / m_CpuTick.QuadPart;

	m_OldTime = m_CurrentTime;

	return m_fTimeDelta;
}

CTimer* CTimer::Create()
{
	CTimer* pInstance = new CTimer();

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Created : CTimer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTimer::Free()
{
}
