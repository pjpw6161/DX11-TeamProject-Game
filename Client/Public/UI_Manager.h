#pragma once
#include "Client_Define.h"
#include "Base.h"
#include "ItemBlock.h"
#include "InvenFrame.h"
#include "SkillBlock.h"
#include "Symbol.h"
#include "FadeBox.h"
#include "Riding.h"
#include "Trigger_Manager.h"
#include "InfinityTower.h"
#include "Player.h"
#include "Balloon.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CSkillSlot;
class CItemBlock;
class CSkillBlock;
class CInvenFrame;

class CUI_Manager final : public CBase
{
	DECLARE_SINGLETON(CUI_Manager)
public:
	enum TYPE { MONSTER, NPC, TYPE_END };
	enum MOUSESTATE { M_DEFAULT, M_TEXT, M_GRAB, M_HIDE, MOUSESTATE_END };
private:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

public:
	HRESULT Init();
	/*

	void	Tick(_float fTimeDelta);
	void	Late_Tick(_float fTimeDelta);
	HRESULT Render();
	*/

private:
	HRESULT Init_Items();
	HRESULT Init_Skills();


public:
	void Set_Symbol(CSymbol::TYPE eType);
	SKILLINFO Get_SkillInfo(WEAPON_TYPE eType, _uint iIdx);
	void Set_TimeStop(_bool bStop) { m_bTimeStop = bStop; }
	_bool Get_TimeStop() { return m_bTimeStop; }

	ITEM Find_Item(wstring& strItemName);
	//ITEM Find_Item(WEAPON_TYPE eType, _uint iPartIndex);

	HRESULT Set_Item(wstring& strItemName, _uint iNum = 1);
	HRESULT Set_Inven(CGameObject* pGameObject);
	HRESULT Set_InvenFrame(CGameObject* pGameObject);
	HRESULT Set_ItemSlots(CItemBlock::ITEMSLOT eSlot, CGameObject* pGameObject);
	HRESULT Set_InvenItemSlots(CItemBlock::ITEMSLOT eSlot, CGameObject* pGameObject);
	HRESULT Set_SkillBookSlots(WEAPON_TYPE eType, CSkillBlock::SKILLSLOT eSlot, CSkillSlot* pGameObject);
	HRESULT Set_SkillBlock(CSkillBlock* pGameObject);
	HRESULT Set_SkillBook(CGameObject* pGameObject);

	HRESULT Unlock_Skill(_uint iIndex);
	_bool Use_Skill(WEAPON_TYPE eType, CSkillBlock::SKILLSLOT eSlot, _int* iIndex, _int* iMp);
	CGameObject* Get_InvenFrame();
	CSkillSlot* Get_SkillSlot(WEAPON_TYPE eType, CSkillBlock::SKILLSLOT eSlot);
	CGameObject* Get_ItemSlots(CItemBlock::ITEMSLOT eSlot);
	_bool Is_ItemSlotFull(CItemBlock::ITEMSLOT eSlot);
	HRESULT Set_Item_In_EmptySlot(CItemBlock::ITEMSLOT eSlot, CItem* pItem, _int* iItemNum);
	ITEM Set_Item_In_FullSlot(CItemBlock::ITEMSLOT eSlot, CItem* pItem, _int* iItemNum, _int* iChangeItemNum = nullptr);
	void Delete_Item_In_Slot(CItemBlock::ITEMSLOT eSlot);
	void Use_Item_In_Slot(CItemBlock::ITEMSLOT eSlot);
	void Delete_Item(INVEN_TYPE eInvenType, wstring& strName);

	void Set_RadarPos(TYPE eType, CTransform* pTransform);
	void Delete_RadarPos(TYPE eType, CTransform* pTransform);
	const vector<CTransform*> Get_RadarPosition(TYPE eType) const;

	void Set_HairColor(_vec4 vColor) { m_vHairColor = vColor; }
	void Set_Picking_UI(_bool isPicking) { m_isPicking = isPicking; }
	void Set_FullScreenUI(_bool isShowing) { m_isShowing = isShowing; }
	void Set_InvenActive(_bool isInvenActive) { if (m_isInvenActive && !isInvenActive) { m_isSetInvenState = false; } m_isInvenActive = isInvenActive; }
	void Set_Exp_ByPercent(_float fExp);
	HRESULT Set_Coin(_int iCoin);
	HRESULT Set_Diamond(_uint iDia);
	HRESULT Set_WeaponType(WEAPON_TYPE eWpType);
	HRESULT Set_CustomPart(PART_TYPE eType, _uint iIndex, _uint iStatus);
	HRESULT Set_WearableItem(WEARABLE_TYPE eType, ITEM eItemDesc);
	_bool Set_CurrentPlayerPos(_vec4 vPos);
	void Set_MouseState(MOUSESTATE eState) { m_eMouseState = eState; }
	void Set_Hp(_int iHp, _int iMaxHp) { m_vHp = _vec2((_float)iHp, (_float)iMaxHp); }
	void Set_Mp(_int iMp, _int iMaxMp) { m_vMp = _vec2((_float)iMp, (_float)iMaxMp); }
	void Set_isBoss(_bool isBoss) { m_isBoss = isBoss; }
	void Set_isTargeting(_bool isTargeting) { m_isTargeting = isTargeting; }
	void Set_TargetPos(_vec4 vTargetPos) { m_vTargetPos = vTargetPos; }

	void Set_Riding(VEHICLE_TYPE eVCType, Riding_Type eRidingType) { m_CurRidingType[eVCType] = eRidingType; }
	void Set_NewRiding(Riding_Type eRidingType) { m_eRidingType = eRidingType; }
	void Set_Pet(_bool isPet) { m_isPet = isPet; }
	void Open_InfinityTower(_bool isOpen) { m_isInfinityTower = isOpen; }
	_bool IsInfinityTower() { return m_isInfinityTower; }

	const _bool& Get_IsPetDie() const { return m_isPet; }

	const Riding_Type& Get_Riding(VEHICLE_TYPE eVCType) const { return m_CurRidingType[eVCType]; }
	const Riding_Type& Get_NewRiding() const { return m_eRidingType; }

	void Set_MpState(_bool isMp, _uint iMp = 0);
	void Set_Heal(_bool isHeal, _uint iHeal = 0);
	_bool Get_Heal(_uint* iHeal = nullptr);
	_bool Get_MpState(_uint* iMp = nullptr);

	void Set_SkillSlotChange(_bool isSkillSlotChange) { m_isSetSkillSlot = isSkillSlotChange; }
	_bool Is_SkillSlotChange() { return m_isSetSkillSlot; }

	const _vec2& Get_Hp() const { return m_vHp; }
	const _vec2& Get_Mp() const { return m_vMp; }

	const _vec4& Get_TargetPos() const { return m_vTargetPos; }
	const _vec4& Get_HairColor() const { return m_vHairColor; }
	const _vec4& Get_InvenPos() const { return m_vInvenPos; }
	const _vec4& Get_LastPlayerPos() const { return m_vPlayerPos; }
	const _uint& Get_CustomPart(PART_TYPE eType);
	const _uint& Get_WeaponType(PART_TYPE eType, WEAPON_TYPE* wpType);
	const _uint& Get_Coin() const { return m_iCoin; }
	const _uint& Get_Diamond() const { return m_iDiamond; }
	const _float2& Get_Exp() const { return m_fExp; }
	const _uint& Get_Level() const { return m_iLevel; }
	const MOUSESTATE& Get_MouseState() const { return m_eMouseState; }
	const CPlayer::PLAYER_STATUS& Get_ExtraStatus() const { return m_tExtraStaus; }

	const PART_TYPE& Is_CustomPartChanged() const { return m_eChangedPart; }
	const _bool& Is_Picking_UI() const { return m_isPicking; }
	const _bool& Showing_FullScreenUI() const { return m_isShowing; }
	const _bool& Is_InvenActive() const { return m_isInvenActive; }
	const _bool& Is_BossFight() const { return m_isBoss; }
	const _bool& Is_Targeting() const { return m_isTargeting; }

	void Level_Up();

	HRESULT Add_FadeBox(CFadeBox::FADE_DESC& Description);
	CFadeBox* Clone_FadeBox(CFadeBox::FADE_DESC& Description);


	void Set_Teleport(_bool isTeleport, TeleportSpot eSpot = TS_END) { m_bTeleport = isTeleport; m_eTeleportSpot = eSpot; }
	const _bool& Is_Teleport(TeleportSpot* eSpot = nullptr);

	void Set_Collect() { m_isCollect = true; }
	_bool Is_Collecting();

	void Set_MiniGameStage(TOWER eTower) { m_iCurrentMiniGame = eTower; }
	_uint Get_CurrentMiniGame();
	
	void Set_WorldMap() { m_isWorldMap = true; }
	_bool Is_WorldMap();
	
	void Set_BrickBallColor(BrickColor eColor) { m_eBrickBallColor = eColor; }
	const BrickColor& Get_BrickBallColor() const { return m_eBrickBallColor; }

private:
	_uint			m_iCurrentMiniGame{(_uint)TOWER_END};
	PART_TYPE		m_eChangedPart{ PT_END };
	MOUSESTATE		m_eMouseState{ M_DEFAULT };
	WEAPON_TYPE		m_eWeaponType{ WP_BOW };

	TeleportSpot	m_eTeleportSpot{ TS_END };
	_bool			m_bTeleport{};

	_bool			m_isCollect{ false };
	_bool			m_isWorldMap{ false };

	_bool			m_isInfinityTower{ false };
	_bool			m_isPet{ false };
	_bool			m_isMpState{ false };
	_bool			m_isHeal{ false };
	_bool			m_isTargeting{ false };
	_bool			m_isFirstKill{ false };
	_bool			m_isSetSkillSlot{ false };
	_bool			m_isPicking{ false };
	_bool			m_isShowing{ false };
	_bool			m_isInvenActive{ false };
	_bool			m_isSetInvenState{ false };
	_bool			m_bTimeStop{ false };
	_bool			m_isBoss{ false };


	Riding_Type		m_eRidingType{ Type_End };
	Riding_Type     m_CurRidingType[VC_END];
	_uint			m_iLevel{ 1 };
	_uint			m_iHeal{  };
	_uint			m_iMpState{  };


	_uint			m_iCoin{};
	_uint			m_iDiamond{};
	_uint			m_CustomPart[PART_TYPE::PT_END]{};
	CPlayer::PLAYER_STATUS   m_tExtraStaus{};

	_float2			m_fExp{ 0.f, 1000.f };

	_vec2			m_vHp{ 1000.f, 1000.f };
	_vec2			m_vMp{ 500.f, 500.f };

	_vec4			m_vInvenPos{ 0.f, 1000.f, 0.f, 1.f };
	_vec4			m_vPlayerPos{ 0.f, 0.f, 0.f, 0.f };
	_vec4			m_vCameraPos{ 0.f, 0.f, 0.f, 0.f };
	_vec4			m_vTargetPos{ 0.f, 0.f, 0.f, 0.f };
	_vec4			m_vHairColor{ 0.f, 0.f, 0.f, 0.f };

	BrickColor		m_eBrickBallColor{};

	map<const wstring, ITEM> m_mapItem{};

	CGameInstance* m_pGameInstance{ nullptr };
	CSkillBlock* m_pSkillBlock{ nullptr };
	CSkillSlot* m_pSkillSlots[WP_END][CSkillBlock::SKILL_END]{};
	CGameObject* m_pItemSlots[CItemBlock::ITEMSLOT_END]{};
	CGameObject* m_pInvenItemSlots[CItemBlock::ITEMSLOT_END]{};
	CGameObject* m_pInven{ nullptr };
	CGameObject* m_pInvenFrame{ nullptr };
	CGameObject* m_pSkillBook{ nullptr };

	vector<CTransform*> m_vecRadarPos[TYPE_END]{};
	SKILLINFO		m_SkillInfo[WP_END][4]{};

public:
	virtual void Free() override;
	};

	END
