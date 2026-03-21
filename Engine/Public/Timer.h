#pragma once
#include "Base.h"

BEGIN(Engine)

class CTimer final : public CBase
{
private:
	CTimer();
	virtual ~CTimer() = default;

public:
	HRESULT Init();
	_float Compute_TimeDelta();

private:
	_lint m_CurrentTime{};
	_lint m_OldTime{};
	_lint m_OriginTime{};
	_lint m_CpuTick{};

	_float m_fTimeDelta{};

public:
	static CTimer* Create();
	virtual void Free();
};

END