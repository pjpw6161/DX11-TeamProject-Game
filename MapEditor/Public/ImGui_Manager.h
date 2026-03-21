#pragma once

#include "Base.h"
#include "MapEditor_Define.h"
#include "Camera.h"
#include "CutScene_Curve.h"

BEGIN(Engine)
class CGameInstance;
class CVIBuffer_Terrain;
END

BEGIN(MapEditor)

enum class ItemType
{
	Map,
	Terrain,
	Objects,
	Environment,
	Monster,
	NPC,
	Trigger,
	Interaction,
	Camera,
	End
};

struct DummyInfo
{
	wstring Prototype{};
	enum class ItemType eType {};
	_uint iStageIndex{};
	_int iTriggerNum{};
	_float fTriggerSize{};
	//_float4 vPos{};
	_float3 vNormal{};
	//_float3 vLook{};
	_mat mMatrix{};
	_bool bCheck{};
	class CDummy** ppDummy{ nullptr };
};

struct MapInfo
{
	wstring Prototype{};
	enum class ItemType eType {};
	_uint iStageIndex{};
	_vec4 vPos{};
	class CMap** ppMap{ nullptr };
};


struct TriggerInfo
{
	_uint iTriggerNum{};
	_float fTriggerSize{};
	_vec4 vPos{};
	class CTrigger** ppTrigger{ nullptr };
};

struct CameraInfo
{
	//string strName{};
	_vec4 vStartCutScene{};
	_vec4 vEndCutScene{};
	_float fCameraSpeed{};
	enum class ItemType eType {};
	class CCamera_CutScene** ppCamera{ nullptr };
	CCamera::Camera_Desc eCamera_Desc{};
};



struct TERRAIN_INFO_MAPTOOL : public TERRAIN_INFO
{
	std::vector<VTXSTATICMESH> Vertices{};
	std::vector<unsigned long> Indices{};
	class CTerrain** ppTerrain{ nullptr };
};


class CImGui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImGui_Manager)

private:
	CImGui_Manager();
	virtual ~CImGui_Manager() = default;

public:
	HRESULT Initialize_Prototype(const GRAPHIC_DESC& GraphicDesc);
	void Tick(_float fTimeDelta);
	HRESULT Render();
	_vec4 Get_MouseWorld() { return m_vMouseWorld; }

private:
	_int FindByName(char* SearchStr, vector<const char*> List);
	HRESULT ImGuiMenu();
	HRESULT ImGuiPos();
	HRESULT ImGuizmoMenu();

	void Create_Dummy(const _int& iListIndex);
	void Create_Camera();
	void Create_Curve(class CCamera_CutScene* pCamera, _vec4 FirstPos, _vec4 SecondPos);
	void Create_Map(const _int& iListIndex);
	HRESULT Create_Terrain();
	HRESULT Modify_Terrain();
	void Delete_Dummy();
	void Delete_Camera();
	void Delete_Map();
	void Delete_Terrain();
	void Delete_Curve(vector<class CCutScene_Curve*>& pAtCurve, vector<class CCutScene_Curve*>& pEyeCurve);
	void Reset();
	void PopBack_Dummy();

	void Search_Map();
	void Search_Object();
	void Search_Monster();
	void Search_NPC();
	void Search_Envir();
	void Search_Inter();

	void Mouse_Pos();
	void FastPicking();
	void PickingRayCast();
	void Picking_On_Terrain();
	void MeshToMask();

	HRESULT Save_Map_Vertex_Info();
	HRESULT Load_Map_Vertex_Info_For_Terrain();

	// 데이터 파싱
	// 맵
	HRESULT Save_Map();
	HRESULT Load_Map();

	// 오브젝트
	HRESULT Save_Object();
	HRESULT Load_Object();

	// 몬스터
	HRESULT Save_Monster();
	HRESULT Load_Monster();

	// NPC
	HRESULT Save_NPC();
	HRESULT Load_NPC();

	// 환경
	HRESULT Save_Envir();
	HRESULT Load_Envir();

	// 상호작용
	HRESULT Save_Interaction();
	HRESULT Load_Interaction();

	// 트리거
	HRESULT Save_Trigger();
	HRESULT Load_Trigger();

	// 카메라
	HRESULT Save_CutScene(class CCamera_CutScene* pCamera);
	HRESULT Load_CutScene();

	// 현재 위치 저장
	HRESULT Save_Pos();
	HRESULT Save_WorldMatrix();

private:
	CGameInstance* m_pGameInstance{ nullptr };
	HWND m_hWnd;
	_uint m_iWinSizeX{ 0 };
	_uint m_iWinSizeY{ 0 };
	float m_iCameraSpeed{ 0 };

	POINT m_ptMouse = {};
	_float2 m_vMousePos{ 0.f,0.f };
	_vec4 m_PickingPos{ 0.f, 0.f, 0.f, 1.f };
	_vec4 m_TerrainPos{ 0.f, 0.f, 0.f, 1.f };
	_float m_fCamDist{};
	_int TerrainCount[2]{ 0,0 };
	_float TerrainHight{ 0.f };
	_float BrushSize{ 1.f };
	_float m_fRadius{ 0.f };

	_float4 m_vPos{ 0.f, 0.f, 0.f, 1.f };
	_float4 m_vLook{ 0.f, 0.f, 1.f, 0.f };
	ItemType m_eItemType{ ItemType::End };
	_vec4 m_vMouseWorld{ 0.f,0.f,0.f,0.f };

	_int DummyIndex{ 0 };
	_int MapIndex{ 0 };
	_int CameraIndex{ 0 };
	_bool m_isMode{ false };
	_int iTriggerNum{ -1 };

	_bool m_isInstancing{false};
	vector<_vec4> m_vInstancePos;
	vector<_vec3> m_vInstanceNor;
	_mat m_mCameraEyePoint{};
	_mat m_mCameraAtPoint{};
	_vec4 m_fCameraPickingPos[2];
	_float fTimeDeltaAcc{0.f};
	_uint iClickCount{0};
	_bool m_isUseNormal{false};

	_vec4 m_vRayCastPos{};
	_vec3 m_vRayCastNor{};

	_vec3 m_vCamPosition{};
	_bool m_isCamPosition{false};

private:
	// 파일의 이름 가져와서 저장
	unordered_map<wstring, vector<const char*>> Maps;
	unordered_map<wstring, vector<const char*>> Objects;
	unordered_map<wstring, vector<const char*>> Monsters;
	vector<const char*> NPCs;
	unordered_map<wstring, vector<const char*>> Envirs;
	unordered_map<wstring, vector<const char*>> Interactions;

	vector<class CMap*> m_MapsList;
	vector<class CDummy*> m_ObjectsList;
	vector<class CDummy*> m_MonsterList;
	vector<class CDummy*> m_NPCList;
	vector<class CDummy*> m_EnvirList;
	vector<class CDummy*> m_TriggerList;
	vector<class CDummy*> m_InteractionList;
	vector<class CCamera_CutScene*> m_CameraList;
	vector<class CCutScene_Curve*> m_SectionEyeList;
	vector<class CCutScene_Curve*> m_SectionAtList;


	map<int, class CDummy*>m_DummyList;
	map<int, class CMap*>m_Map{};
	//map<int, class CCamera_CutScene*>m_Camera{};

	class CDummy* m_pSelectedDummy{ nullptr };
	class CMap* m_pSelectMap{ nullptr };
	class CCamera_CutScene* m_pSelectCamera{ nullptr };
	class CCutScene_Curve* m_pSelectSection{ nullptr };
	class CTerrain* m_pTerrain{ nullptr };

	_mat	m_ObjectMatrix{};
	_mat	m_MapMatrix{};
	_mat	m_CameraMatrix{};
	_mat	m_ViewMatrix = {};
	_mat	m_ProjMatrix = {};

	
	_int m_iSelectIdx = { -1 };
	_bool m_iImGuizmoCheck = { false };
	CTexture* m_pTextures{ nullptr };
	wstring m_eType{};
	_float m_fTriggerSize{1.f};
	_int m_iTriggerNumber{0};
	string SectionName{};
	SectionInfo m_eSectionInfo{};
	_bool m_isTriggerCheck{ false };

	// 카메라 Eye
	_vec4 vEyeStartCurved{};
	_vec4 vEyeStartPos{};
	_vec4 vEyeEndPos{};
	_vec4 vEyeEndCurved{};
	// 카메라 At
	_vec4 vAtStartCurved{};
	_vec4 vAtStartPos{};
	_vec4 vAtEndPos{};
	_vec4 vAtEndCurved{};

	// 컷씬카메라
	_int m_iFrame{0};
	_float fEyeSpeed{1.f};
	_float fCameraSpeed{10.f};
	_float m_fTimeDelta{ 0.f };
public:
	static CImGui_Manager* Create(const GRAPHIC_DESC& GraphicDesc);
	virtual void Free() override;

};
END
