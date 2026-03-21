#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CVTFTest final : public CGameObject
{
private:
	CVTFTest(_dev pDevice, _context pContext);
	CVTFTest(const CVTFTest& rhs);
	virtual ~CVTFTest() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Instance() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CVTFModel* m_pModelCom = { nullptr };

private:
	ANIM_DESC m_Animation{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CVTFTest* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END