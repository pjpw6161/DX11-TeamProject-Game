#pragma once
#include "MapEditor_Define.h"
#include "BlendObject.h"
#include "ImGui_Manager.h"

BEGIN(MapEditor)

class CMap final : public CGameObject
{
private:
	CMap(_dev pDevice, _context pContext);
	CMap(const CMap& rhs);
	virtual ~CMap() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Select(const _bool& isSelected);
	void Mode(const _bool& isMode);
	_bool Get_Selected() { return m_isSelected; }
	_int Get_ID() const { return m_iID; }
	MapInfo Get_Info() const { return m_Info; }
	void Set_Dead() { m_isDead = true; }
	HRESULT Create_HightMap(vector<VTXSTATICMESH> VerticesPos);

	_float lerp(_float a, _float b, _float f);

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CModel* m_pModelCom{ nullptr };

	//CImGui_Manager* m_pImGui_Manager{ nullptr };
private:

	MapInfo m_Info{};
	_bool m_isSelected{};
	_uint m_iShaderPass{};
	_uint m_iOutLineShaderPass{};

	_bool m_isMode{ false };
	_bool m_isCreate{ false };
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();


public:
	static CMap* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END