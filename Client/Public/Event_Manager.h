#pragma once
#include "Client_Define.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CEvent_Manager final : public CBase
{
	DECLARE_SINGLETON(CEvent_Manager)

public:
	enum EVENT_TYPE { QUESTIN, QUESTEND, LEVELUP, TUTORIAL, UNLOCKSKILL, TYPE_END};
	enum QUEST_TRIGGER { POTION, GROAR_MONSTER, TRIGGER_END};

	typedef struct tagEventDesc
	{
		EVENT_TYPE eType;
		_bool isMain;
		_float	fExp;
		_uint	iNum;
		_uint	iMoney;
		wstring strQuestTitle;
		wstring strText;
	}EVENT_DESC;

private:
	CEvent_Manager();
	virtual ~CEvent_Manager() = default;

private:
	CGameInstance*						m_pGameInstance{ nullptr };

	TUTO_SEQ							m_eCurTuto{ TUTO_END };

	_bool								m_TutoComplete[TUTO_END]{};
	_bool								m_isTutoStarted{ false };
	_bool								m_isEventIn = { false };
	_bool								m_isWaiting = { false };

	_bool								m_QuestTrigger[TRIGGER_END];

	class CQuest*						m_pQuest{ nullptr };
	class CPop_Alert*					m_pAlert{ nullptr };

	vector<EVENT_DESC>					m_vecPopEvents;
	map <const wstring, EVENT_DESC>		m_QuestMap;

public:
	HRESULT Init();
	void	Tick(_float fTimeDelta);
	void	Late_Tick(_float fTimeDelta);
	HRESULT Render();

private:
	HRESULT Set_Event(EVENT_DESC pDesc);
	HRESULT Init_Quest();

public:
	void Set_QuestTrigger(QUEST_TRIGGER eTrigger) { m_QuestTrigger[eTrigger] = true; }

	_bool Get_QuestTrigger(QUEST_TRIGGER eTrigger) { return m_QuestTrigger[eTrigger]; }
	_bool Find_Quest(const wstring& strQuest);
	HRESULT Set_Quest(const wstring& strQuest);
	HRESULT Update_Quest(const wstring& strQuest);

	void Set_LevelUp(_uint iLevel);
	void Set_SkillUnlock(_uint iIndex);

	void Set_Alert(const wstring strAlert);

	void Set_TutorialComplete(TUTO_SEQ eTuto);
	void Set_TutorialSeq(TUTO_SEQ eTuto);
	_bool Get_TutorialComplete(TUTO_SEQ eTuto);
	TUTO_SEQ Get_TutorialLevel() { return m_eCurTuto; }

public:
	virtual void Free() override;
};

END
