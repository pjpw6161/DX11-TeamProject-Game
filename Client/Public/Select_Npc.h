#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CSelect_Npc final : public CGameObject
{


private:
	CSelect_Npc(_dev pDevice, _context pContext);
	CSelect_Npc(const CSelect_Npc& rhs);
	virtual ~CSelect_Npc() = default;

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

private:
	wstring m_strModelTag = {};
	ANIM_DESC m_Animation{};

public:
	static CSelect_Npc* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END