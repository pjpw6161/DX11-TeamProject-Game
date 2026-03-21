#pragma once
#include "Client_Define.h"
#include "BlendObject.h"


struct MapInfo
{
	wstring Prototype{};
	_mat m_Matrix{};
};


BEGIN(Client)

class CMap final : public CBlendObject
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

	MapInfo Get_Info() const { return m_Info; }

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CModel* m_pModelCom{ nullptr };
private:

	MapInfo m_Info{};
	_bool m_isSelected{};
	_uint m_iShaderPass{};
	_uint m_iOutLineShaderPass{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();


public:
	static CMap* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END