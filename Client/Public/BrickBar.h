#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CBrickBar final : public CGameObject
{
private:
	CBrickBar(_dev pDevice, _context pContext);
	CBrickBar(const CBrickBar& rhs);
	virtual ~CBrickBar() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Add_Collider();
	void Update_Collider();

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

private:
	_bool					m_isColl{};
	_uint					m_iCollNum{};

	_float					m_fSpeed{};
	_vec3					m_vDir{};

	_mat					m_EffectMatrix{};
	class CEffect_Dummy*	m_pEffect_Ball{};

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBrickBar* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END