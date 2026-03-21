#pragma once

#include "Client_Define.h"
#include "BlendObject.h"
#include "CommonTrail.h"
BEGIN(Client)

struct Arrow_Type
{
	ATTACK_TYPE Att_Type{ AT_End };
	_mat world{};
	_vec4 vPos{};
	_vec4 vLook{};
	_int iDamage{};
	CCollider* MonCollider{ nullptr };
	_bool bAimMode{};
};
class CArrow final : public CBlendObject
{

private:
	CArrow(_dev pDevice, _context pContext);
	CArrow(const CArrow& rhs);
	virtual ~CArrow() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CCollider* m_pCollider{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CRenderer* m_pRendererCom{ nullptr };
	CModel* m_pModelCom{ nullptr };
	CCommonTrail* m_pTrail{ nullptr };
	class CEffect_Dummy* m_pParticle{ nullptr };

private:
	Arrow_Type m_ArrowType{};
	_float m_fDeadTime{};
	_mat m_ParticleMatrix{};
	_int m_iDamage{};
	_vec4 m_vRaycastPos{};
	_bool m_bIntersected{};
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CArrow* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END