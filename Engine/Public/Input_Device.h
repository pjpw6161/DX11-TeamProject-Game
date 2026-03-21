#pragma once
#ifndef InputDev_h__
#define InputDev_h__
#endif // !InputDev_h__

#include "Base.h"

BEGIN(Engine)

enum class InputChannel
{
	Default,
	UI,
	GamePlay,
	Engine,
	End
};

class CInput_Device final : public CBase
{
private:
	CInput_Device();
	virtual ~CInput_Device() = default;

public:
	HRESULT Init(HINSTANCE hInst, HWND hWnd);
	void Update_InputDev();
	void Late_Update_InputDev();

public:
	_bool Key_Pressing(_ubyte iKey);
	_bool Key_Down(_ubyte iKey, InputChannel eInputChannel);
	_bool Key_Up(_ubyte iKey, InputChannel eInputChannel);

	_bool Mouse_Pressing(_long iKey);
	_bool Mouse_Down(_long iKey, InputChannel eInputChannel);
	_bool Mouse_Up(_long iKey, InputChannel eInputChannel);
	_long Get_MouseMove(MouseState eMouseState);

	_bool Gamepad_Pressing(GAMPAD_KEY_STATE eKey);
	_bool Gamepad_Down(GAMPAD_KEY_STATE eKey, InputChannel eInputChannel);
	_bool Gamepad_Up(GAMPAD_KEY_STATE eKey, InputChannel eInputChannel);
	// Trigger pushed amount between 0 and 1
	// Stick coord between 0 and 1
	const _float& Gamepad_LTrigger() const;
	const _float& Gamepad_RTrigger() const;
	const _float2& Gamepad_LStick() const;
	const _float2& Gamepad_RStick() const;
	void Gamepad_Vibration(_ushort LMotorSpeed, _ushort RMotorSpeed);

	const _bool& IsGamepadMode() const;

private:
	LPDIRECTINPUT8 m_pInputSDK{ nullptr };
	LPDIRECTINPUTDEVICE8 m_pMouse{ nullptr };
	LPDIRECTINPUTDEVICE8 m_pKeyboard{ nullptr };
	LPDIRECTINPUTDEVICE8 m_pGamepad{ nullptr };

private:
	HWND m_hWnd{};

	_bool m_bPrevFrame_KeyState[ToIndex(InputChannel::End)][UCHAR_MAX]{};
	_bool m_bPrevFrame_MouseState[ToIndex(InputChannel::End)][DIM_END]{};
	_bool m_bPrevFrame_GampadState[ToIndex(InputChannel::End)][USHRT_MAX]{};

	_byte m_byKeyState[UCHAR_MAX]{};
	_byte m_byPreKeyState[UCHAR_MAX]{};
	DIMOUSESTATE m_MouseState{};
	DIMOUSESTATE m_PreMouseState{};
	XINPUT_STATE m_GamepadState{};

	_float2 m_vLStick{};
	_float2 m_vRStick{};
	_float m_fLTrigger{};
	_float m_fRTrigger{};

	_ulong m_dwPrevPacket{};
	_bool m_isGamepadMode{};

public:
	static CInput_Device* Create(HINSTANCE hInst, HWND hWnd);
	virtual void Free() override;
};

END