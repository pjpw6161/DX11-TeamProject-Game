#pragma once
#include "Base.h"

BEGIN(Engine);

class CPicking final : public CBase
{
private:
	CPicking(_dev pDevice, _context pContext, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	virtual ~CPicking() = default;

public:
	void Tick();

public:
	void TransformRay_ToLocal(_fmatrix WorldMatrix);
	_vec4 Get_World_Pos() { return m_vRayPos_World; }
	_vec4 Get_World_Dir() { return m_vRayDir_World; }
	
	_bool Picking_InWorld(_vec4 vPoint1, _vec4 vPoint2, _vec4 vPoint3, _Inout_ _vec3 * pPickPos);
	_bool Picking_InLocal(_vec4 vPoint1, _vec4 vPoint2, _vec4 vPoint3, _Inout_ _vec4* pPickPos);
	_bool Picking_InLocal(_vec4 vPoint1, _vec4 vPoint2, _vec4 vPoint3, _vec4 vNormal, _Inout_ _vec4* pPickPos);
	_vec4 Compute_MousePicked_Terrain(_float44 matTerrainWorld, _float3* pVerticesPos, _uint iNumVerticesX, _uint iNumVerticesZ);
	_vec4 Compute_MousePicked_MeshTerrain(_float44 matTerrainWorld, vector<VTXNORTEX> vVertices, vector<_ulong> vIndices);
	_float4 PickingDepth(_float x, _float y);
	_vec4 PickingNormal(_float x, _float y);
	_int FastPicking(_uint x, _uint y);
	HRESULT Ready_Texture2D();
	HRESULT Ready_FastPicking();

private:
	_dev m_pDevice{ nullptr };
	_context m_pContext{ nullptr };
	HWND m_hWnd{};
	_uint m_iWinSizeX{}, m_iWinSizeY{};
	class CGameInstance* m_pGameInstance{ nullptr };

	_vec4 m_vRayPos_World{};
	_vec4 m_vRayDir_World{};

	_vec4 m_vRayPos_Local{};
	_vec4 m_vRayDir_Local{};
	ID3D11Texture2D* m_pTexture{ nullptr };
	ID3D11Texture2D* m_pFastTexture{ nullptr };

public:
	static CPicking* Create(_dev pDevice, _context pContext, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	virtual void Free() override;
};

END