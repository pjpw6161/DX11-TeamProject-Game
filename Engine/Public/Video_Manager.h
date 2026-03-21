#pragma once
#include "Base.h"

BEGIN(Engine)

class CVideo_Manager : public CBase
{
private:
	CVideo_Manager(HWND hWnd, _uint2 vWinSize);
	virtual ~CVideo_Manager() = default;

public:
	HRESULT Play_Video(const wstring& strVideoFilePath);
	void Stop_Video();
	const _bool Is_Playing_Video();

	void Set_StopKey(_ubyte iKey);

private:
	HWND m_hWnd{};
	_uint2 m_vWinSize{};

	HWND m_hVideo{};
	HDC m_DC{};
	HDC m_BackDC{};
	HBITMAP m_hBackBit{};
	HBITMAP m_hOldBackBit{};

	_ubyte m_iStopKey{};

public:
	static CVideo_Manager* Create(HWND hWnd, _uint2 vWinSize);
	virtual void Free() override;
};

END