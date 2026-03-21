#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CTestVTFModel final : public CGameObject
{
public:

private:
	CTestVTFModel(_dev pDevice, _context pContext);
	CTestVTFModel(const CTestVTFModel& rhs);
	virtual ~CTestVTFModel() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;


private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CRealtimeVTFModel* m_pModelCom = { nullptr };

private:
	_mat m_OldWorldMatrix{};
	_uint m_iAnimIndex = 0;

	_uint hi = 0;
	_bool m_UsingMotionBlur = false;

	ANIM_DESC m_Animation{};


private:
	wstring m_strModelTag = {};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();


public:
	static CTestVTFModel* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END