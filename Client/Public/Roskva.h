#pragma once

#include "Client_Define.h"
#include "NPC.h"

BEGIN(Client)
class C3DUITex;
class CDialogText;
class CTextButton;
class CTextButtonColor;
class CRoskva final : public CNPC
{
public:
	enum ROSKVA_ANIM
	{
		COAST02_SC02_NPC_ROSKVA,
		IDLE01,
		LOADINGSCENE_POSE_ROSKVA,
		TALK01_OLD,
		TALK02,
		TALK03,
		WALK,
		ANIM_END
	};
	enum ROSKVA_STATE { TALK, QUEST_ING, ROSKVA_END };

private:
	CRoskva(_dev pDevice, _context pContext);
	CRoskva(const CRoskva& rhs);
	virtual ~CRoskva() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:

private:
	ROSKVA_STATE				m_eState{ TALK };
	_bool						m_bTalking = { false };
	_bool						m_bNextDialog = { false };
	_bool						m_isColl = { false };

	_float						m_fDir{ -1.f };
	_float						m_fButtonTime{};

	wstring						m_strQuestOngoing{};
	vector<wstring>				m_vecDialog;
	vector<wstring>				m_vecChatt;

	CTextButton*				m_pLine{ nullptr };
	CTextButton*				m_pArrow{ nullptr };
	CDialogText*				m_pDialogText{ nullptr };
	CTextButton*				m_pSkipButton{ nullptr };
	CTextButtonColor*			m_pBackGround{ nullptr };
	C3DUITex*					m_pSpeechBubble{ nullptr };

private:
	void Set_Text(ROSKVA_STATE eState);

private:
	HRESULT Init_Dialog();
	HRESULT Add_Parts();

public:
	static CRoskva* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END