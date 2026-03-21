#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"
#include "Skill_Model.h"
#include "Scarecrow.h"

BEGIN(Client)
class CSkillBook final : public COrthographicObject
{
private:
	CSkillBook(_dev pDevice, _context pContext);
	CSkillBook(const CSkillBook& rhs);
	virtual ~CSkillBook() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };

private:
	WEAPON_TYPE									m_ePrevType{ WP_BOW };
	WEAPON_TYPE									m_eCurType{ WP_BOW };

	_uint										m_iCurIndex{};
	_int										m_iSoundChannel = -1;

	_bool										m_isPicking{ false };
	_bool										m_isPrototype{ false };
	_bool										m_bNewSkillIn{ false };
	_bool										m_isActive{ false };

	_float										m_fDir{ 1.f };
	_float										m_fTime{ 0.2f };
	CGameObject*								m_pMoney{ nullptr };
	CGameObject*								m_pDiamond{ nullptr };
	CGameObject*								m_pNotify{ nullptr };
	CGameObject*								m_pTitleButton{ nullptr };
	CGameObject*								m_pExitButton{ nullptr };
	CGameObject*								m_pBackGround{ nullptr };

	CGameObject*								m_pUnderBar{ nullptr };
	CGameObject*								m_pSelectButton{ nullptr };
	CGameObject*								m_pSkillType[WP_END];

	class CTextButtonColor*						m_pSlotBackGround{ nullptr };
	CTextButtonColor*							m_pResetSlot{ nullptr };
	CTextButtonColor*							m_pSelectSlot[4];

	class CSkillSlot*							m_pSkillSlot[WP_END][4];

	vector<class CSkillDesc*>					m_vecSkillDesc[WP_END];

	LIGHT_DESC									m_Light_Desc{};

	CSkill_Model*								m_pSkill_Model{ nullptr };
	CScarecrow*									m_pScarecorw{ nullptr };

public:
	HRESULT Unlock_Skill(_uint iIndex);

private:
	void Init_SkillBookState(); 
	HRESULT Init_SkillDesc(); 

private:
	HRESULT Add_Parts();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSkillBook* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END