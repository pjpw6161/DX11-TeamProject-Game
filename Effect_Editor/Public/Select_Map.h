#pragma once
#include "Effect_Define.h"
#include "GameObject.h"

BEGIN(Effect)

class CSelect_Map final : public CGameObject
{


private:
	CSelect_Map(_dev pDevice, _context pContext);
	CSelect_Map(const CSelect_Map& rhs);
	virtual ~CSelect_Map() = default;

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
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

public:
	static CSelect_Map* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END