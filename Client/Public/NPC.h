#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

using NPC_INFO = struct NPCInfo
{
	wstring strNPCPrototype{};
	_mat	NPCWorldMat{};
};

class CNPC abstract : public CGameObject
{
protected:
	CNPC(_dev pDevice, _context pContext);
	CNPC(const CNPC& rhs);
	virtual ~CNPC() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

protected:
	NPCInfo m_pInfo{};

//public:
//	virtual _vec4 Compute_PlayerPos();
//	virtual _float Compute_PlayerDistance();

protected:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

protected:
	wstring m_strModelTag = {};
	ANIM_DESC m_Animation{};

protected:
	_uint m_iPassIndex = {};

protected:
	void Update_Collider();

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END