#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CScarecrow final : public CGameObject
{
private:
	CScarecrow(_dev pDevice, _context pContext);
	CScarecrow(const CScarecrow& rhs);
	virtual ~CScarecrow() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CCollider* m_pHitCollider{ nullptr };
	CRenderer* m_pRendererCom = { nullptr };

public:
	static CScarecrow* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END