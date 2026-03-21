#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"
#include "TextButton.h"
#include "TextButtonColor.h"

BEGIN(Client)
class CTextButton;
class CTextButtonColor;
class CCustom final : public CGameObject
{
public:
	enum CUSTOM_MENU { C_FACE, C_HAIR, C_END };
private:
	CCustom(_dev pDevice, _context pContext);
	CCustom(const CCustom& rhs);
	virtual ~CCustom() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Add_Components();
	HRESULT Add_Parts();
	HRESULT Add_Models();
	HRESULT Init_Menu();
	HRESULT Bind_ShaderResources();

private:
	_bool				m_isPrototype{ false };
	_bool				m_bShow{ false };

	_float				m_fAlpha{ 0.f };

	_vec2				m_vCurSelect[C_END];
	_bool				m_isMenuClick[C_END];
	CTextButton*		m_pCustomMenu[C_END];
	
	pair<_vec2, _uint>  m_Hair[9]; //pos와 인덱스 저장
	pair<_vec2, _uint>  m_Face[6];

	CGameObject*		m_pBackGround{ nullptr };

	CTextButtonColor*	m_pLine1{ nullptr };
	CTextButtonColor*	m_pLine2{ nullptr };

	CTextButton*		m_pFaceGroup{ nullptr };
	CTextButton*		m_pHairGroup{ nullptr };
	CTextButton*		m_pSelectCustomEffect{ nullptr };

	CTextButton*		m_pTitleButton{ nullptr };
	CTextButton*		m_pClassButton{ nullptr };
	CTextButton*		m_pSelectMenuEffect{ nullptr };
	CTextButton*		m_pSelectColorEffect{ nullptr };
	CTextButtonColor*	m_pSelectButton{ nullptr }; 
	CTextButtonColor*	m_pColorButtons[10];

private:
	void Set_CameraState(_uint iSelect);
	_bool Set_PickingRange(POINT& ptMouse);

public:
	static CCustom* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END