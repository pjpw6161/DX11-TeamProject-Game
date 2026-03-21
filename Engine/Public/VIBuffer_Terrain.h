#pragma once
#include "VIBuffer.h"
#include "Mesh.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
	CVIBuffer_Terrain(_dev pDevice, _context pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;


public:
	_uint Get_NumVerticesX() { return m_iNumVerticesX; }
	_uint Get_NumVerticesZ() { return m_iNumVerticesZ; }
private:
	TERRAIN_INFO m_pTerrain{};

private:
	_bool	m_isClone;
	_uint m_iNumVerticesX{ 0 };
	_uint m_iNumVerticesZ{ 0 };
	VTXNORTEX* pVertices;
	_ulong* pIndices;
public:
	HRESULT ModifyTerrainVertexBuffer(_uint iNumVerticesX, _uint iNumVerticesZ);
	HRESULT Mesh_Terrain( vector<VTXSTATICMESH> vVertices, vector<_ulong> vIndices);
	HRESULT Modify_Terrain_Hight(_uint iNumVerticesX, _uint iNumVerticesZ, _vec3 PickedPosition, _float fNewHeight, _float fSize);
	HRESULT Terrain_Init(_uint iNumVerticesX, _uint iNumVerticesZ, _vec3 PickedPosition, _float fNewHeight, _float fSize);

public:
	static CVIBuffer_Terrain* Create(_dev pDevice, _context pContext);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END