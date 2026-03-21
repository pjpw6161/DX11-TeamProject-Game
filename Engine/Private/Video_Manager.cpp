#include "Video_Manager.h"
#include "GameInstance.h"

CVideo_Manager::CVideo_Manager(HWND hWnd, _uint2 vWinSize)
	: m_hWnd(hWnd)
	, m_vWinSize(vWinSize)
{
}

HRESULT CVideo_Manager::Play_Video(const wstring& strVideoFilePath)
{
	m_DC = GetDC(m_hWnd);

	m_BackDC = CreateCompatibleDC(m_DC);

	m_hBackBit = CreateCompatibleBitmap(m_DC, m_vWinSize.x, m_vWinSize.y);

	m_hOldBackBit = (HBITMAP)SelectObject(m_BackDC, m_hBackBit);

	wstring strFullPath = L"../Bin/Resources/Video/" + strVideoFilePath;
	m_hVideo = MCIWndCreate(m_hWnd, NULL, WS_CHILD | WS_VISIBLE | MCIWNDF_NOPLAYBAR
							, strFullPath.c_str());

	MCIWndSetVolume(m_hWnd, 1.f);

	MoveWindow(m_hVideo, 0, 0, m_vWinSize.x, m_vWinSize.y, FALSE);

	MCIWndPlay(m_hVideo);

	return S_OK;
}

void CVideo_Manager::Stop_Video()
{
	MCIWndStop(m_hVideo);
	MCIWndClose(m_hVideo);
	m_hVideo = NULL;

	SelectObject(m_BackDC, m_hOldBackBit); //DC에 원래 설정을 돌려줍니다.
	DeleteDC(m_BackDC);  // 메모리를 반환합니다.
	DeleteObject(m_hBackBit); // 메모리를 반환합니다.
	ReleaseDC(m_hWnd, m_DC); // 윈도우에 DC 해제를 요청합니다.
}

const _bool CVideo_Manager::Is_Playing_Video()
{
	if (CGameInstance::Get_Instance()->Key_Down(m_iStopKey, InputChannel::Engine))
	{
		Stop_Video();
		return false;
	}

	if (m_hVideo)
	{
		long length = MCIWndGetLength(m_hVideo);
		long position = MCIWndGetPosition(m_hVideo);
		
		if (length == position)
		{
			Stop_Video();
			return false;
		}
		else
		{
			return true;
		}
	}

	return false;
}

void CVideo_Manager::Set_StopKey(_ubyte iKey)
{
	m_iStopKey = iKey;
}

CVideo_Manager* CVideo_Manager::Create(HWND hWnd, _uint2 vWinSize)
{
	CVideo_Manager* pInstance = new CVideo_Manager(hWnd, vWinSize);

	if (not pInstance)
	{
		MSG_BOX("Failed to Create : CVideo_Manager");
	}

	return pInstance;
}

void CVideo_Manager::Free()
{
	if (Is_Playing_Video())
	{
		Stop_Video();
	}
}
