#pragma once
#include "Client_Define.h"
#include "Trigger_Manager.h"
#include "GameObject.h"

BEGIN(Client)


class CTrigger final : public CGameObject
{

private:
	CTrigger(_dev pDevice, _context pContext);
	CTrigger(const CTrigger& rhs);
	virtual ~CTrigger() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;

	void Set_Limited(_bool bLimited) { m_isLimited = bLimited; }

	_uint Get_TriggerNum() { return m_iTriggerNumber; }
	TriggerType Get_TriggerType() { return m_eTriggerType; }
	_bool Get_Collision() { return m_isCollision; }
	_bool Get_Limited() { return m_isLimited; }

private:
	CRenderer* m_pRendererCom{ nullptr };
	CCollider* m_pCollider{ nullptr };

private:
	_vec4 m_vPos{};
	_float m_iColliderSize{};
	_int 	m_iTriggerNumber{};
	_bool	m_isCollision{false};
	TriggerType m_eTriggerType{ TRIGGER_END };
	_bool	m_isLimited{ false };

private:
	HRESULT Add_Components();


public:
	static CTrigger* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END