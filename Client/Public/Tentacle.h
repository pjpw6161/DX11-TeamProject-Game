#pragma once

#include "Client_Define.h"
#include "Monster.h"

BEGIN(Client)

class CTentacle final : public CMonster
{
private:
	CTentacle(_dev pDevice, _context pContext);
	CTentacle(const CTentacle& rhs);
	virtual ~CTentacle() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual HRESULT Add_Collider() override;
	virtual void Update_Collider() override;

private:
	class CEffect_Dummy* m_pFrameEffect = { nullptr };
	class CEffect_Dummy* m_pBaseEffect = { nullptr };

private:
	_float m_fTime = {};
	_int m_iSoundChannel = -1;

public:
	static CTentacle* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END