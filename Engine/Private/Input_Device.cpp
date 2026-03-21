#include "Input_Device.h"
CInput_Device::CInput_Device()
{
}

HRESULT CInput_Device::Init(HINSTANCE hInst, HWND hWnd)
{
	m_hWnd = hWnd;

	if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, nullptr)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
	{
		return E_FAIL;
	}
	m_pMouse->SetDataFormat(&c_dfDIMouse);
	m_pMouse->SetCooperativeLevel(m_hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	m_pMouse->Acquire();

	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, nullptr)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pKeyboard->SetDataFormat(&c_dfDIKeyboard)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pKeyboard->SetCooperativeLevel(m_hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pKeyboard->Acquire()))
	{
		return E_FAIL;
	}
	_ulong result = XInputGetState(0, nullptr);
	if (result != ERROR_SUCCESS)
	{
		if (result == ERROR_DEVICE_NOT_CONNECTED)
		{
			return S_OK;
		}
		//return E_FAIL;
	}

	Gamepad_Vibration(32000, 16000);

	return S_OK;
}

void CInput_Device::Update_InputDev()
{
	if (GetForegroundWindow() != m_hWnd)
	{
		for (_uint i = 0; i < UCHAR_MAX + 1; i++)
		{
			m_byKeyState[i] = {};
		}
		for (_uint i = 0; i < DIM_END; i++)
		{
			m_MouseState.rgbButtons[i] = {};
		}
		return;
	}

	if (FAILED(m_pMouse->GetDeviceState(sizeof m_MouseState, &m_MouseState)))
	{
		MSG_BOX("Failed to Get Mouse State");
	}
	if (FAILED(m_pKeyboard->GetDeviceState(256, &m_byKeyState)))
	{
		MSG_BOX("Failed to Get Keyboard State");
	}

	//_ulong result = XInputGetState(0, &m_GamepadState);
	//if (result != ERROR_SUCCESS && result != ERROR_DEVICE_NOT_CONNECTED)
	//{
	//	MSG_BOX("Failed to Get Gamepad State");
	//}

	//if (abs(m_GamepadState.Gamepad.sThumbLX) < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
	//	abs(m_GamepadState.Gamepad.sThumbLY) < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	//{
	//	m_vLStick = _float2(0.f, 0.f);
	//}
	//else
	//{
	//	m_vLStick.x = static_cast<_float>(m_GamepadState.Gamepad.sThumbLX) / SHRT_MAX;
	//	m_vLStick.y = static_cast<_float>(m_GamepadState.Gamepad.sThumbLY) / SHRT_MAX;
	//}

	//if (abs(m_GamepadState.Gamepad.sThumbRX) < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
	//	abs(m_GamepadState.Gamepad.sThumbRY) < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
	//{
	//	m_vRStick = _float2(0.f, 0.f);
	//}
	//else
	//{
	//	m_vRStick.x = static_cast<_float>(m_GamepadState.Gamepad.sThumbRX) / SHRT_MAX;
	//	m_vRStick.y = static_cast<_float>(m_GamepadState.Gamepad.sThumbRY) / SHRT_MAX;
	//}

	//if (abs(m_GamepadState.Gamepad.bLeftTrigger) < XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
	//{
	//	m_fLTrigger = 0.f;
	//}
	//else
	//{
	//	m_fLTrigger = static_cast<_float>(m_GamepadState.Gamepad.bLeftTrigger) / CHAR_MAX;
	//}

	//if (abs(m_GamepadState.Gamepad.bRightTrigger) < XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
	//{
	//	m_fRTrigger = 0.f;
	//}
	//else
	//{
	//	m_fRTrigger = static_cast<_float>(m_GamepadState.Gamepad.bRightTrigger) / CHAR_MAX;
	//}

	//if (m_GamepadState.dwPacketNumber != m_dwPrevPacket) // 패드 입력
	//{
	//	m_isGamepadMode = true;
	//}
	//m_dwPrevPacket = m_GamepadState.dwPacketNumber;

	if (m_MouseState.lX || m_MouseState.lY || m_MouseState.lZ || *(_long*)(&m_MouseState.rgbButtons)) // 마우스 입력
	{
		m_isGamepadMode = false;
	}

	_bool isKeyInput{};
	for (auto& Input : m_byKeyState)
	{
		if (Input)
		{
			isKeyInput = true;
		}
	}
	if (isKeyInput) // 키보드 입력
	{
		m_isGamepadMode = false;
	}
}

void CInput_Device::Late_Update_InputDev()
{
	for (_uint i = 0; i < UCHAR_MAX + 1; i++)
	{
		m_byPreKeyState[i] = m_byKeyState[i];
	}
	for (_uint i = 0; i < DIM_END; i++)
	{
		m_PreMouseState.rgbButtons[i] = m_MouseState.rgbButtons[i];
	}
}

_bool CInput_Device::Key_Pressing(_ubyte iKey)
{
	if (m_byKeyState[iKey] & 0x80)
		return true;

	return false;
}

_bool CInput_Device::Key_Down(_ubyte iKey, InputChannel eInputChannel)
{
	if (!m_bPrevFrame_KeyState[ToIndex(eInputChannel)][iKey] && (m_byKeyState[iKey] & 0x80))
	{
		m_bPrevFrame_KeyState[ToIndex(eInputChannel)][iKey] = true;
		return true;
	}

	if (m_bPrevFrame_KeyState[ToIndex(eInputChannel)][iKey] && !(m_byKeyState[iKey] & 0x80))
		m_bPrevFrame_KeyState[ToIndex(eInputChannel)][iKey] = false;

	return false;
}

_bool CInput_Device::Key_Up(_ubyte iKey, InputChannel eInputChannel)
{
	if (m_bPrevFrame_KeyState[ToIndex(eInputChannel)][iKey] && !(m_byKeyState[iKey] & 0x80))
	{
		m_bPrevFrame_KeyState[ToIndex(eInputChannel)][iKey] = false;
		return true;
	}

	if (!m_bPrevFrame_KeyState[ToIndex(eInputChannel)][iKey] && (m_byKeyState[iKey] & 0x80))
		m_bPrevFrame_KeyState[ToIndex(eInputChannel)][iKey] = true;

	return false;
}

_bool CInput_Device::Mouse_Pressing(_long iKey)
{
	if (m_MouseState.rgbButtons[iKey] & 0x80)
		return true;

	return false;
}

_bool CInput_Device::Mouse_Down(_long iKey, InputChannel eInputChannel)
{
	if (eInputChannel == InputChannel::UI)
	{
		if (!m_PreMouseState.rgbButtons[iKey] && (m_MouseState.rgbButtons[iKey] & 0x80))
		{
			return true;
		}

		return false;
	}
	else
	{
		if (!m_bPrevFrame_MouseState[ToIndex(eInputChannel)][iKey] && (m_MouseState.rgbButtons[iKey] & 0x80))
		{
			m_bPrevFrame_MouseState[ToIndex(eInputChannel)][iKey] = true;
			return true;
		}

		if (m_bPrevFrame_MouseState[ToIndex(eInputChannel)][iKey] && !(m_MouseState.rgbButtons[iKey] & 0x80))
			m_bPrevFrame_MouseState[ToIndex(eInputChannel)][iKey] = false;

		return false;
	}
}

_bool CInput_Device::Mouse_Up(_long iKey, InputChannel eInputChannel)
{
	if (eInputChannel == InputChannel::UI)
	{
		if (m_PreMouseState.rgbButtons[iKey] && !(m_MouseState.rgbButtons[iKey] & 0x80))
		{
			return true;
		}

		return false;
	}
	else
	{
		if (m_bPrevFrame_MouseState[ToIndex(eInputChannel)][iKey] && !(m_MouseState.rgbButtons[iKey] & 0x80))
		{
			m_bPrevFrame_MouseState[ToIndex(eInputChannel)][iKey] = false;
			return true;
		}

		if (!m_bPrevFrame_MouseState[ToIndex(eInputChannel)][iKey] && (m_MouseState.rgbButtons[iKey] & 0x80))
			m_bPrevFrame_MouseState[ToIndex(eInputChannel)][iKey] = true;

		return false;
	}
}

_long CInput_Device::Get_MouseMove(MouseState eMouseState)
{
	return *(((_long*)&m_MouseState) + ToIndex(eMouseState));
}

_bool CInput_Device::Gamepad_Pressing(GAMPAD_KEY_STATE eKey)
{
	if (m_GamepadState.Gamepad.wButtons & eKey)
	{
		return true;
	}

	return false;
}

_bool CInput_Device::Gamepad_Down(GAMPAD_KEY_STATE eKey, InputChannel eInputChannel)
{
	if (!m_bPrevFrame_GampadState[ToIndex(eInputChannel)][eKey] && m_GamepadState.Gamepad.wButtons & eKey)
	{
		m_bPrevFrame_GampadState[ToIndex(eInputChannel)][eKey] = true;
		return true;
	}

	if (m_bPrevFrame_GampadState[ToIndex(eInputChannel)][eKey] && !(m_GamepadState.Gamepad.wButtons & eKey))
		m_bPrevFrame_GampadState[ToIndex(eInputChannel)][eKey] = false;

	return false;
}

_bool CInput_Device::Gamepad_Up(GAMPAD_KEY_STATE eKey, InputChannel eInputChannel)
{
	if (m_bPrevFrame_GampadState[ToIndex(eInputChannel)][eKey] && !(m_GamepadState.Gamepad.wButtons & eKey))
	{
		m_bPrevFrame_GampadState[ToIndex(eInputChannel)][eKey] = false;
		return true;
	}

	if (!m_bPrevFrame_GampadState[ToIndex(eInputChannel)][eKey] && (m_GamepadState.Gamepad.wButtons & eKey))
		m_bPrevFrame_GampadState[ToIndex(eInputChannel)][eKey] = true;

	return false;
}

const _float& CInput_Device::Gamepad_LTrigger() const
{
	return m_fLTrigger;
}

const _float& CInput_Device::Gamepad_RTrigger() const
{
	return m_fRTrigger;
}

const _float2& CInput_Device::Gamepad_LStick() const
{
	return m_vLStick;
}

const _float2& CInput_Device::Gamepad_RStick() const
{
	return m_vRStick;
}

void CInput_Device::Gamepad_Vibration(_ushort LMotorSpeed, _ushort RMotorSpeed)
{
	XINPUT_VIBRATION Vibe{};
	Vibe.wLeftMotorSpeed = { LMotorSpeed };
	Vibe.wRightMotorSpeed = { RMotorSpeed };

	XInputSetState(0, &Vibe);
}

const _bool& CInput_Device::IsGamepadMode() const
{
	return m_isGamepadMode;
}

CInput_Device* CInput_Device::Create(HINSTANCE hInst, HWND hWnd)
{
	CInput_Device* pInstance = new CInput_Device();

	if (FAILED(pInstance->Init(hInst, hWnd)))
	{
		MSG_BOX("Failed to Create : CInput_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInput_Device::Free()
{
	Safe_Release(m_pGamepad);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}
