#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"
#include "SkillSlot.h"

BEGIN(Client)
class CSkillSlot;
class CSkillBlock final : public CGameObject
{
public: 
	enum SKILLSLOT{ SKILL1, SKILL2, SKILL3, SKILL4, SKILL_END };

private:
	CSkillBlock(_dev pDevice, _context pContext);
	CSkillBlock(const CSkillBlock& rhs);
	virtual ~CSkillBlock() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };

private:
	WEAPON_TYPE		m_eCurType{ WP_BOW };
	_bool			m_isPrototype{ false };
	CSkillSlot*		m_pSlots[WP_END][SKILL_END];

public:
	_bool	Use_Skill(WEAPON_TYPE eType, SKILLSLOT eSlot, _int* iIndex, _int* iMp);

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Add_Slots();

public:
	static CSkillBlock* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END