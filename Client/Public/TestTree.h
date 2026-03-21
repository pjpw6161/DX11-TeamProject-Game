#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CTestTree final :
    public CGameObject
{
private:
	CTestTree(_dev pDevice, _context pContext);
	CTestTree(const CTestTree& rhs);
	virtual ~CTestTree() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Reflection(_float4 vClipPlane) override;

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

private:
	HRESULT Bind_ShaderResources();
	HRESULT Add_Component();

public:
	static CTestTree* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END