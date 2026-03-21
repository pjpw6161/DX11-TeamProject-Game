#pragma once

#include "Client_Define.h"
#include "GameObject.h"
//#include "CommonSurfaceTrail.h"

BEGIN(Client)

class CMeteor final : public CGameObject
{
private:
	CMeteor(_dev pDevice, _context pContext);
	CMeteor(const CMeteor& rhs);
	virtual ~CMeteor() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Add_Collider();
	void Update_Collider();

//public:
//	void Update_Trail(_float fTimeDelta);

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };

	CModel* m_pModelCom = { nullptr };

private: // Dragon_Boss Component
	CTransform* m_pDragonTransform = { nullptr };

private:
	class CEffect_Dummy* m_pMeteor = { nullptr };
	class CEffect_Dummy* m_pMeteorParti = { nullptr };

	_mat m_MeteorMatrix = {};
	_mat m_MeteorPartiMatrix = {};

//private:
//	CCommonSurfaceTrail* m_pMeteorTrail = { nullptr };

private:
	_vec3 m_vTargetPos = {};

private:
	_float m_fTime = {}; // Test

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CMeteor* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END