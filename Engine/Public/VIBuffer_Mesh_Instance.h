#pragma once
#include "VIBuffer.h"
#include "Mesh.h"
#include "GameObject.h"
BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Mesh_Instance : public CVIBuffer
{

protected:
	CVIBuffer_Mesh_Instance(_dev pDevice, _context pContext);
	CVIBuffer_Mesh_Instance(const CVIBuffer_Mesh_Instance& rhs);
	virtual ~CVIBuffer_Mesh_Instance() = default;

public:
	virtual HRESULT Init_Prototype(_uint iMaxCount = 1500);
	virtual HRESULT Init(void* pArg) override;
	virtual void Update(_float fTimeDelta, _mat WorldMatrix, _int iNumUse = -1, _bool bApplyGravity = false, _vec3 vGravityDir = _vec3());
	virtual HRESULT Render(CMesh* pMesh);
	void Add_Instance(Instance_Data& MeshData);
	void Clear_Instance();
protected:
	D3D11_BUFFER_DESC m_InstancingBufferDesc{};
	D3D11_SUBRESOURCE_DATA m_InstancingInitialData{};


	_uint m_iNumInstances{};
	_uint m_iInstanceStride{};

	_uint m_iMaxCount{ 0 };
	_bool m_isLoop{};

	vector<Instance_Data> m_vMeshInstance;

public:
	static CVIBuffer_Mesh_Instance* Create(_dev pDevice, _context pContext);
	virtual CComponent* Clone(void* pArg) { return nullptr; }
	virtual void Free() override;
};

END