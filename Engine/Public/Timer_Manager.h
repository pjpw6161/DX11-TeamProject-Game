#pragma once
#include "Base.h"

BEGIN(Engine)

class CTimer_Manager final : public CBase
{
private:
	CTimer_Manager();
	virtual ~CTimer_Manager() = default;

public:
	HRESULT Add_Timer(const wstring& strTimerTag);
	_float Compute_TimeDelta(const wstring& strTimerTag);

private:
	map<const wstring, class CTimer*> m_Timers;

private:
	class CTimer* Find_Timer(const wstring& strTimerTag);

public:
	static CTimer_Manager* Create();
	virtual void Free() override;
};

END
