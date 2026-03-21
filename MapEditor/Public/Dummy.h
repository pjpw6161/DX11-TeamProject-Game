#pragma once
#include "MapEditor_Define.h"
#include "GameObject.h"
#include "ImGui_Manager.h"

BEGIN(MapEditor)

class CDummy final : public CGameObject
{
private:
	CDummy(_dev pDevice, _context pContext);
	CDummy(const CDummy& rhs);
	virtual ~CDummy() = default;

public:
	void Select(const _bool& isSelected);
	
	void Get_State(_float4& vPos, _float4& vLook);
	_bool Get_Selected() { return m_isSelected; }
	DummyInfo Get_Info() { return m_Info; }

	void Set_Create() { m_isCreate = true; }
	void Set_Dead() { m_isDead = true; }
	_bool Get_Create() { return m_isCreate; }
	_float Get_Size() { return m_fTriggerSize; }
	_int Get_ID() const {return m_iID;}
	_uint Get_TriggerNum() { return m_iTrigger; }
	_uint Get_AnimIndex();

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Instance() override;
	_int Get_ID() { return m_iID; }

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CModel* m_pModelCom{ nullptr };
	//CImGui_Manager* m_pImGui_Manager{ nullptr };
	CCollider* m_pCollider{ nullptr };
	//CVIBuffer_Instancing_Mesh* m_pVIBuffer{ nullptr };

private:

	DummyInfo m_Info{};
	_bool m_isSelected{};
	_uint m_iShaderPass{};
	_uint m_iOutLineShaderPass{};
	_bool m_isAnim{};
	ANIM_DESC m_Animation{};
	
	ItemType m_eType{ItemType::End};
	_uint m_iTrigger{ 0 };
	_float m_fTriggerSize{};
	_bool m_isCreate{false};
	_bool m_isInstancing{ false };
	vector<_vec4> m_vInstancePos;

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CDummy* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END