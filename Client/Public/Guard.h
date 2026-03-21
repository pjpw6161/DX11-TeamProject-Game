#pragma once

#include "Client_Define.h"
#include "Monster.h"

BEGIN(Client)


class CGuard final :
    public CGameObject
{
public:
	enum GUARD_ANIM {

	};

	enum GAURD_STATE {
		STATE_IDLE,
		STATE_READY,
		STATE_CHASE,
		STATE_ATTACK,
		STATE_HIT,
		STATE_DIE,
		STATE_END
	};

private:
	CGuard(_dev pDevice, _context pContext);
	CGuard(const CGuard& rhs);
	virtual ~CGuard() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Set_Damage(_int iDamage, _uint iDamageType = 0) override;



private:
	CShader* m_pShaderCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CVTFModel* m_pModelCom = nullptr;

	CCollider* m_pBodyColliderCom = nullptr;
	CTexture* m_pDissolveTextureCom = nullptr;

private:
	_uint m_iPassIndex{};
	_float m_fDeadTime = 0.f;
	_float m_fDissolveRatio = 0.f;

	_bool m_bChangePass = false;
	_float m_fHitTime = 0.f;
	_float m_fHittedTime = 0.f;

private:
	ANIM_DESC m_Animation{};

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CGuard* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END