#pragma once
#include "MapEditor_Define.h"
#include "GameObject.h"

BEGIN(MapEditor)

class CMonster final : public CGameObject
{
private:
	CMonster(_dev pDevice, _context pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	/* Get / Set */
	_bool Get_isCreate() { return m_isCreate; }
	_bool Get_isSelected() { return m_isSelected; }
	void Set_isCreate(_bool isCreate) { m_isCreate = isCreate; }
	void Set_isSelected(_bool isSelected) { m_isSelected = isSelected; }

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CModel* m_pModelCom{ nullptr };

private:
	_float4 m_vPos{};
	_float m_fGravity{};
	_bool m_isCreate{false};
	_bool m_isSelected{false};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CMonster* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END