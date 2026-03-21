#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CLauncher final : public CGameObject
{
public:
	enum LAUNCHER_TYPE
	{
		TYPE_RANDOM_POS,
		TYPE_FLOOR,
		TYPE_LASER,
		TYPE_PIZZA,
		TYPE_END
	};

private:
	CLauncher(_dev pDevice, _context pContext);
	CLauncher(const CLauncher& rhs);
	virtual ~CLauncher() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CCollider* m_pColliderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };

	CTexture* m_pDissolveTextureCom = { nullptr };

private:
	class CEffect_Dummy* m_pFrameEffect = { nullptr };
	class CEffect_Dummy* m_pBaseEffect = { nullptr };

	class CEffect_Dummy* m_pLauncher = { nullptr };
	class CEffect_Dummy* m_pLauncherParticle = { nullptr };


private:
	LAUNCHER_TYPE m_eType = { TYPE_END };
	wstring m_strModelTag = {};

private:
	ANIM_DESC m_Animation = {};

private:
	_uint m_iPassIndex = {};

private:
	_float m_fTime = {};
	_float m_fProjectileCreateTime = {};

	_uint m_iProjectileCount = {};

private:
	_float m_fDissolveRatio = { 1.f };

private:
	_bool m_bCreateProjectile = { false };

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CLauncher* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END