#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CBlackhole final : public CGameObject
{
private:
	CBlackhole(_dev pDevice, _context pContext);
	CBlackhole(const CBlackhole& rhs);
	virtual ~CBlackhole() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

//private:
//	CShader* m_pShaderCom = { nullptr };
//	CRenderer* m_pRendererCom = { nullptr };

private:
	class CEffect_Dummy* m_pFrameEffect = { nullptr };
	class CEffect_Dummy* m_pBaseEffect = { nullptr };

private:
	_float m_fLifeTime = {};
	_int m_iSoundChannel = -1;

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBlackhole* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END