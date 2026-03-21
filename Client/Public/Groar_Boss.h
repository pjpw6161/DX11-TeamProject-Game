#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CGroar_Boss final : public CGameObject
{
public:
	enum GROAR_NPC_ANIM
	{
		DIE,
		NPC_IDLE,
		TALK,
		NPC_ANIM_END
	};

	enum GROAR_ANIM
	{
		ATTACK01,
		ATTACK02,
		ATTACK03,
		ATTACK04,
		ATTACK05,
		ATTACK06,
		ATTACK07,
		ATTACK_RAGE,
		DIE01,
		GROAR_DEAD_SC01_MON_GROAR,
		GROAR_DEAD_SC02_MON_GROAR,
		GROAR_DEAD_SC03_MON_GROAR,
		HIT_ADD,
		IDLE,
		MON_GROAR_ASGARD_ATTACK00, // 오른손에서 초록색 투사체 던지기
		MON_GROAR_ASGARD_ATTACK01, // 왼손에서 초록색 투사체 던지기
		MON_GROAR_ASGARD_ATTACK02, // 초록색 투사체 6개 날리기
		MON_GROAR_ASGARD_ATTACK03, // 바닥 쾅쾅 찍기
		MON_GROAR_ASGARD_ATTACK04, // X
		MON_GROAR_ASGARD_ATTACK05, // 거미 소환
		MON_GROAR_ASGARD_ATTACK06, // 꼬리찍어서 촉수
		MON_GROAR_ASGARD_ATTACK07, // 손 X자
		MON_GROAR_ASGARD_ATTACK08, // 울부짖기(장판)
		MON_GROAR_ASGARD_ATTACK_RAGE,
		// 00, 01, 02, 03, 04, 05, 06, 07, 08
		MON_GROAR_ASGARD_DIE,
		MON_GROAR_ASGARD_IDLE,
		MON_GROAR_ASGARD_ROAR,
		MON_GROAR_ASGARD_SPAWN,
		MON_GROAR_ASGARD_WALK,
		ROAR,
		RUN,
		STUN,
		TURN_L,
		TURN_R,
		W_ATTACK01,
		W_ATTACK02,
		W_ATTACK04,
		W_ATTACK05,
		W_ATTACK06,
		W_ATTACK07,
		W_ATTACK08,
		W_GROAR_RAGE,
		WALK,
		ANIM_END
	};

	enum GROAR_STATE
	{
		STATE_NPC,
		STATE_SCENE01,
		STATE_SCENE02,
		STATE_BOSS,
		STATE_END
	};

	enum GROAR_BOSS_STATE
	{
		BOSS_STATE_IDLE,
		BOSS_STATE_ROAR,
		BOSS_STATE_CHASE,
		BOSS_STATE_THROW_ATTACK, // 00, 01
		BOSS_STATE_SIX_MISSILE, // 02
		BOSS_STATE_TAKE_DOWN, // 03
		BOSS_STATE_SPIDER, // 05
		BOSS_STATE_TENTACLE, // 06
		BOSS_STATE_XBEAM, // 07
		BOSS_STATE_RAGE, // 08
		BOSS_STATE_DIE,
		BOSS_STATE_END
	};

	enum GROAR_ATTACK // 랜덤 방지용
	{
		ATTACK_THROW,
		ATTACK_SIX_MISSILE,
		ATTACK_TAKE_DOWN,
		ATTACK_SPIDER,
		ATTACK_TENTACLE,
		ATTACK_YELL,
		ATTACK_XBEAM,
		ATTACK_END
	};

	enum GROAR_NPCSTATE { NPC_TALK, NPC_QUEST, NPC_END };


private:
	CGroar_Boss(_dev pDevice, _context pContext);
	CGroar_Boss(const CGroar_Boss& rhs);
	virtual ~CGroar_Boss() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void Set_Damage(_int iDamage, _uint iDamageType = 0) override;

public:
	void Init_State(_float fTimeDelta);
	void Tick_State(_float fTimeDelta);

public:
	HRESULT Add_Collider();
	void Update_Collider();

private:
	HRESULT Init_Dialog();
	HRESULT Add_Parts();
	void NPC_Tick(_float fTimeDelta);
	void NPC_LateTick(_float fTimeDelta);
	void Set_Text(GROAR_NPCSTATE eState);
	void Play_TalkSound(const wstring& strTalkText);


private:
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };

	CModel* m_pNPCModelCom = { nullptr };
	CModel* m_pScene01ModelCom = { nullptr };
	CModel* m_pScene02ModelCom = { nullptr };
	CModel* m_pBossModelCom = { nullptr };

	CCollider* m_pBodyColliderCom = { nullptr };
	CCollider* m_pAttackColliderCom = { nullptr };
	CCollider* m_pNpcColliderCom = { nullptr };

	class CHPBoss* m_pHpBoss{ nullptr };

private:
	GROAR_STATE m_ePreState = STATE_END;
	GROAR_STATE m_eCurState = STATE_END;

	GROAR_BOSS_STATE m_eBossPreState = BOSS_STATE_END;
	GROAR_BOSS_STATE m_eBossCurState = BOSS_STATE_END;

private:
	static const _float m_fChaseRange;
	static const _float m_fAttackRange;

private:
	ANIM_DESC m_Animation{};
	_int m_iSoundChannel = -1;

private:
	_float m_fIdleTime = {};

private:
	_uint m_iAttackPattern = {};
	_bool m_bSelectAttackPattern = { false };

private:
	_bool m_bAttacked1 = { false };
	_bool m_bAttacked2 = { false };
	_bool m_bAttacked3 = { false };

private:
	_bool m_bCreateMissile = { false };
	_uint m_iThrowAttackCombo = {};

private:
	_bool m_bCreateSpider = { false };

private:
	_bool m_bCreateXBeam = { false };

private:
	_bool m_bTentacleOn = { false };
	_float m_fTentacleTime = {};

private:
	_float m_fRageTime = {};

private:
	_bool m_bAttack_Selected[ATTACK_END] = { false };

private:
	_bool m_bChangePass = { false };
	_uint m_iPassIndex = {};
	_float m_fHitTime = {};

private:
	_bool m_bChangePos = { false };

private:
	GROAR_NPCSTATE				m_eState{ NPC_TALK };
	_bool						m_bTalking = { false };
	_bool						m_bNextDialog = { false };
	_bool						m_isColl = { false };

	_float						m_fDir{ -1.f };
	_float						m_fButtonTime{ 600.f };

	wstring						m_strQuestOngoing{};
	list<wstring>				m_vecDialog;
	vector<wstring>				m_vecChatt;
	list<wstring>				m_TalkSounds;

	class CTextButton* m_pLine{ nullptr };
	CTextButton* m_pArrow{ nullptr };
	CTextButton* m_pSkipButton{ nullptr };
	class CDialogText* m_pDialogText{ nullptr };
	class CTextButtonColor* m_pBackGround{ nullptr };
	class C3DUITex* m_pSpeechBubble{ nullptr };

private:
	const wstring m_strLines[14]
	{
		TEXT("여보.."),
		TEXT("놀라게 해서 죄송해요 저는 그로아 마나하임의 신녀입니다."),
		TEXT("다름이 아니라 돌아오지 않는 남편을 찾고 있었어요."),
		TEXT("모르겠어요 무슨일인지.. 돌아올시간이 훌쩍 지났는데도.."),
		TEXT("아무런 연락이 없어요.. 절대 그럴사람이 아닌데.."),
		TEXT("분명 무슨 일이 생긴것이 틀림없어요"),
		TEXT("도움을 부탁드려도 될까요? 여보.."),
		TEXT("!그로아의 부탁"),
		TEXT("오셨군요! 혹시 제 남편은?"),
		TEXT("이..이건..어째서 이 팔찌만? 제.. 제 남편은.. 제 남편은요?"),
		TEXT("저는 괜찮아요. 다만 남편의 마지막을 확인하고 싶을 뿐이에요. 얼굴이라도 손끝이라도 보고싶을 뿐이에요.."),
		TEXT("염치없지만 꼭 부탁드릴게요. 남편의 흔적을 찾는동안 저 끔찍한 마물들을 막아주세요. 부탁드립니다."),
		TEXT("!그로아를 지켜라"),
		TEXT("END")
	};

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CGroar_Boss* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END