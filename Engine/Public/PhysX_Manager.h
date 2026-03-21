#pragma once
#include "Base.h"

BEGIN(Engine)

enum CollisionGroup
{
	COLGROUP_NON_COLLIdable = 0,
	COLGROUP_PLAYER =  1 << 0,
	COLGROUP_MONSTER = 1 << 1,
	COLGROUP_TERRAIN = 1 << 2,
};

enum CollisionMask
{
	MASK_PLAYER = COLGROUP_MONSTER | COLGROUP_TERRAIN,
	MASK_MONSTER = COLGROUP_PLAYER | COLGROUP_TERRAIN,
	MASK_TERRAIN = COLGROUP_PLAYER | COLGROUP_MONSTER | COLGROUP_TERRAIN,
};

class CPhysX_Manager final : public CBase
{
private:
	CPhysX_Manager(_dev pDevice, _context pContext);
	virtual ~CPhysX_Manager() = default;

public:
	HRESULT Init();
	void Tick(_float fTimeDelta);
#ifdef _DEBUG
	HRESULT Render();
#endif // _DEBUG


public:
	void Init_PhysX_Character(class CTransform* pTransform, CollisionGroup eGroup, PxCapsuleControllerDesc* pDesc);
	void Init_PhysX_MoveableObject(class CTransform* pTransform);
	void Apply_PhysX(class CTransform* pTransform);
	void Update_PhysX(class CTransform* pTransform);
	PxRigidStatic* Cook_StaticMesh(_uint iNumVertices, void* pVertices, _uint iNumIndices, void* pIndices);
	_bool Raycast(_vec4 vOrigin, _vec4 vDir, _float fDist, PxRaycastBuffer& Buffer);
	_bool Raycast(_vec4 vOrigin, _vec4 vDir, _float fDist, PxRaycastBuffer& Buffer,PxQueryFilterData Filter);
	_bool Raycast(_vec3 vOrigin, _vec3 vDir, _float fDist, PxRaycastBuffer& Buffer);
	_bool Raycast(_float3 vOrigin, _float3 vDir, _float fDist, PxRaycastBuffer& Buffer);
	_bool Raycast(_float4 vOrigin, _float4 vDir, _float fDist, PxRaycastBuffer& Buffer);
	_bool Raycast(_fvector vOrigin, _fvector vDir, _float fDist, PxRaycastBuffer& Buffer);


private:
	_dev m_pDevice{ nullptr };
	_context m_pContext{ nullptr };

	class CGameInstance* m_pGameInstance{ nullptr };

	PxFoundation* m_pFoundation{ nullptr };
	PxPhysics* m_pPhysics{ nullptr };

	PxScene* m_pScene{ nullptr };
	PxControllerManager* m_pControllerManager{ nullptr };
	PxDefaultCpuDispatcher* m_pDispatcher{ nullptr };
	PxMaterial* m_pMaterial{ nullptr };
	PxPvd* m_pPvd{ nullptr };
	PxPvdTransport* m_pTransport{ nullptr };

	map<class CTransform*, PxRigidDynamic*> m_DynamicActors{};

#ifdef _DEBUG
	class CVIBuffer_PhysX* m_pVIBufferCom{ nullptr };
	class CShader* m_pDebugShader{ nullptr };
#endif // _DEBUG

public:
	static CPhysX_Manager* Create(_dev pDevice, _context pContext);
	virtual void Free() override;
};

END