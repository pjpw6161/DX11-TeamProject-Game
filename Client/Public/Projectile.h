#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CProjectile final : public CGameObject
{
public:
	enum PROJECTILE_TYPE
	{
		TYPE_RANDOM_POS,
		TYPE_FLOOR,
		TYPE_GUIDED_MISSILE,
		TYPE_TANGHURU,
		TYPE_END
	};

	using PROJECTILE_DESC = struct ProjectileDesc
	{
		PROJECTILE_TYPE eType;
		_vec3 vStartPos;
		CTransform* pLauncherTransform = { nullptr };
	};

private:
	CProjectile(_dev pDevice, _context pContext);
	CProjectile(const CProjectile& rhs);
	virtual ~CProjectile() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	//CShader* m_pShaderCom = { nullptr };
	//CRenderer* m_pRendererCom = { nullptr };
	//CModel* m_pModelCom = { nullptr };
	//CCollider* m_pColliderCom = { nullptr };

private:
	class CEffect_Dummy* m_pFrameEffect = { nullptr };
	class CEffect_Dummy* m_pBaseEffect = { nullptr };

private:
	class CEffect_Dummy* m_pBall = { nullptr };
	class CEffect_Dummy* m_pBallParticle = { nullptr };

	_mat m_UpdateMatrix = {};
	_float m_fCircleRange = { 0.001f };

private:
	PROJECTILE_DESC m_ProjectileDesc = {};

private:
	_float m_fTime = {};

private:
	static _uint m_iProjectileID;


public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CProjectile* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END