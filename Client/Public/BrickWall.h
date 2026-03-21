#pragma once

#include "Client_Define.h"
#include "GameObject.h"


BEGIN(Client)


class CBrickWall final : public CGameObject
{
public:
	//enum DIR { FRONT, BACK, LEFT, RIGHT, DIR_END };
	typedef struct tagWallDesc
	{
		RECT rcRect{};
	}WALL_DESC;
private:
	CBrickWall(_dev pDevice, _context pContext);
	CBrickWall(const CBrickWall& rhs);
	virtual ~CBrickWall() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	CShader*	m_pShaderCom = { nullptr };
	CRenderer*  m_pRendererCom = { nullptr };
	CModel*		m_pModelCom = { nullptr };
	CCollider*  m_pColliderCom = { nullptr };
	CTexture*	m_pDissolveTextureCom = { nullptr };
	CTexture*	m_pMaskTextureCom = { nullptr };


private:

	_float m_fX{};
	_float m_fDir = {1.f};
	_float m_fDissolveRatio = {0.5f};

	_bool m_isColl{};
	_bool m_bParticle{};
	_bool m_bDamaged = { false };
	_bool m_isBlur = { false };

	_vec4 m_vColor{};
	//_vec3 m_vNormals[DIR_END]{};

	RECT				m_rcRect{};
	_mat				m_EffectMatrix{};
	class CEffect_Dummy* m_pEffect_Ball{};

public:
	const _bool& Is_Coll() const { return m_isColl; }


private:
	//void Set_Normal();

private:
	HRESULT Add_Collider();
	void Update_Collider();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBrickWall* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END