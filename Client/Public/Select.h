#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"
#include "CharacterSelect.h"
#include "TextButton.h"
#include "SelectDesc.h"
#include "TextButtonColor.h"
#include "Select_Model.h"

BEGIN(Client)
class CTextButton;
class CSelect_Model;
class CCharacterSelect;
class CTextButtonColor;
class CSelect final : public CGameObject
{
public:
	enum SELECT_MODEL { SORCERESS, WARRIOR, ROGUE, PRIEST, MODEL_END };
private:
	CSelect(_dev pDevice, _context pContext);
	CSelect(const CSelect& rhs);
	virtual ~CSelect() = default;

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
	HRESULT Bind_ShaderResources();

private:
	SELECT_MODEL		m_eCurModel{ MODEL_END };

	_bool				m_isPrototype{ false };
	_bool				m_bShow{ false };

	_float				m_fAlpha{ 0.f };

	CTextButton*		m_pTitleButton{ nullptr };
	CTextButton*		m_pClassButton{ nullptr };
	CSelectDesc*		m_pSelectDesc{ nullptr };
	CCharacterSelect*	m_pCharacterSelect{ nullptr };
	CTextButtonColor*	m_pSelectButton{ nullptr };
	CTextButtonColor*	m_pBackButton{ nullptr };
	CSelect_Model*		m_pSelectModels[MODEL_END];

private:
	void Set_SelectDesc(_uint iSelect);
	void Set_CameraState(_uint iSelect);

public:
	static CSelect* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END