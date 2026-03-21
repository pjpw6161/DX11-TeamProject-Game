#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CPop_Alert final : public COrthographicObject
{
private:
	CPop_Alert(_dev pDevice, _context pContext);
	CPop_Alert(const CPop_Alert& rhs);
	virtual ~CPop_Alert() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };

private:
	_float			m_fTime{};
	_float			m_fPopTime{};
	_vec2			m_vFrontAlertPos;
	vector<wstring> m_vecAlert;
	vector<_float>  m_vecTime;

public:
	void Set_Alert(wstring strText);

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CPop_Alert* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END