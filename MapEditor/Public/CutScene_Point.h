#pragma once
#include "MapEditor_Define.h"
#include "GameObject.h"

BEGIN(MapEditor)

class CCutScene_Point final : public CGameObject
{
private:
	CCutScene_Point(_dev pDevice, _context pContext);
	CCutScene_Point(const CCutScene_Point& rhs);
	virtual ~CCutScene_Point() = default;
public:
	virtual HRESULT Init_Prototype();
	virtual HRESULT Init(void* pArg);
	virtual void Tick(_float TimeDelta) override;
	virtual void Late_Tick(_float TimeDelta) override;
	virtual HRESULT Render() override;
	void Select(const _bool& isSelected);

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CRenderer* m_pRendererCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	
	_uint m_iShaderPass{};
	_uint m_iOutLineShaderPass{};
	_bool m_isSelected{};

public:
	static CCutScene_Point* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END