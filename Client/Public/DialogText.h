#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CDialogText final : public COrthographicObject
{
public:
	typedef struct tagNameTagDesc
	{
		LEVEL_ID eLevelID;
		_float fDepth;
		wstring strText;
		_vec2	vTextPos;
	}DIALOGTEXT_DESC;

private:
	CDialogText(_dev pDevice, _context pContext);
	CDialogText(const CDialogText& rhs);
	virtual ~CDialogText() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };

private:
	LEVEL_ID		m_eLevel{};
	wstring			m_strText{};

	_uint			m_iIndex{};

	_float			m_fPeriod{};

	_vec2			m_vTextPos{};
	_vec3			m_vPosition{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CDialogText* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END