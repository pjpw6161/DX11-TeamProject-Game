#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CVTFMonster abstract : public CGameObject
{
public:
	typedef struct tagVTFMonsterDesc
	{
		wstring strModelTag;
		_vec3 vPosition{};

	}VTFMONSTER_DESC;

protected:
	CVTFMonster(_dev pDevice, _context pContext);
	CVTFMonster(const CVTFMonster& rhs);
	virtual ~CVTFMonster() = default;

public:
	virtual void Set_Damage(_int iDamage, _uint MonAttType = 0) override;

	const wstring& Get_ModelTag() const {
		return m_strModelTag;
	}

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Instance() override;

protected:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CVTFModel* m_pModelCom = { nullptr };

protected:
	ANIM_DESC m_Animation{};
	_randNum m_RandomNumber;

protected:
	wstring m_strModelTag{};
	_bool m_IsHitted{};
	_float m_fHitTime{};

protected:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END