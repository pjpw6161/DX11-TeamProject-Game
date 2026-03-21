#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CSkill_Model final : public CGameObject
{
public:
	enum SKILLMODEL_ANIM {
		BOW1, BOW3, SWORD3, SWORD4,
		SWORD2, SWORD1, BOW2, BOW4 , SKILL_END};

private:
	CSkill_Model(_dev pDevice, _context pContext);
	CSkill_Model(const CSkill_Model& rhs);
	virtual ~CSkill_Model() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Arrow_Rain();
	void After_BowAtt();
	void Create_Arrow(SKILLMODEL_ANIM skill);

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CModel* m_pWeapon_ModelCom = { nullptr };

private:
	_bool m_ReadyArrow{};
	_bool m_bView{};
	_vec4 m_vArrowLook{};
	ANIM_DESC m_Animation{};
	_uint m_iArrowRain = {};
	_bool m_bArrowRain_Start{};
	SKILLMODEL_ANIM m_eCurAnimState{ SKILL_END };

public:
	void Change_AnimState(SKILLMODEL_ANIM eAnim);// { m_eCurAnimState = eAnim; }

public:
	static CSkill_Model* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END