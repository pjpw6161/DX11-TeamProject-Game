#pragma once

#include "Base.h"
#include "MapEditor_Define.h"

BEGIN(MapEditor)
class CMenu final : public CBase
{

public:
	enum TOOL_TYPE { TYPE_MAP, TYPE_OBJECT, TYPE_ANIM, TYPE_MONSTER, TYPE_CAMERA, TYPE_END };



private:
	CMenu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMenu() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta) ;
	virtual void Late_Tick(_float fTimeDelta); 
	virtual HRESULT Render();

	_uint Get_Mode() { return m_isMode; }

private:
	HRESULT Add_Components();

	void Setting_Init();
	void ShowInfo();

	_float44	m_vViewMatrix = {};
	_float44	m_vProjMatrix = {};
	_float44   m_vObjectsMatrix = {};
	_float3		m_vSelectPos = { 0.f, 0.f, 0.f };
	_float3		m_vModelSize = { 1.f, 1.f, 1.f };
	_float3		m_vPosition = { 0.f, 0.f, 0.f };
	_uint		m_iClickCount = { 0 };
	_bool m_isSeleted = { false };
	TOOL_TYPE  m_eType = { TYPE_END };

	bool m_isLoop = { false };
	int			m_isMode = { 0 };


private:
	// 피킹
	HRESULT Picking_On_Depth();

	// 객체 생성
	//HRESULT Create_Map(OBJ_MAP eMap);
	//HRESULT Create_Object(OBJECT_TYPE eObjType);
	//HRESULT Create_Monster(MONSTER_TYPE eMonsterType);
	//HRESULT Create_AnimObject(ANIM_OBJ eAnimObj);

	// 객체 삭제
	//HRESULT Object_Delete();
	//HRESULT Anim_Delete();
	//HRESULT Monster_Delete();

	// 객체 선택
	//HRESULT Selected_Object();
	//HRESULT Selected_Anim();
	//HRESULT Selected_Monster();

	// 객체 변경
	//HRESULT Change_Object();
	//HRESULT Change_Anim();
	//HRESULT Change_Monster();

	// 맵과 오브젝트들 저장, 불러오기
	//HRESULT Save_Data();
	//HRESULT Load_Data();

	// 몬스터 저장, 불러오기
	//HRESULT Save_Monster();
	//HRESULT Load_Monster();

	// 선택해제
	//HRESULT DeSelectedObject();
	//HRESULT DeSelectedAnim();
	//HRESULT DeSelectedMonster();


private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	CRenderer* m_pRendererCom = {nullptr};
	class CGameInstance* m_pGameInstance{ nullptr };


public:
	static CMenu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};
END
