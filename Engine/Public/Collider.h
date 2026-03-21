#pragma once
#include "Component.h"

BEGIN(Engine)

enum CollideFace {
	C_LEFT,
	C_RIGHT, 
	C_FRONT, 
	C_BACK, 
	C_TOP, 
	C_BOTTOM,
	C_END 
};

enum class ColliderType
{
	AABB,
	OBB,
	Sphere,
	Frustum,
	End
};

struct Collider_Desc
{
	ColliderType eType{};

	_vec3 vCenter{};
	_float fRadius{};
	_vec3 vExtents{};
	_vec3 vRadians{};
	_mat matFrustum{};
};

class ENGINE_DLL CCollider final : public CComponent
{
private:
	CCollider(_dev pDevice, _context pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Init_Prototype();
	virtual HRESULT Init(void* pArg);
	void Update(_fmatrix WorldMatrix);

public:
	_bool Intersect(const class CCollider* pTargetCollider);

public:
	void Change_Extents(_vec3 vExtents);
	void Set_Radius(const _float fRadius) const;
	_vec3 Get_Extents();	
	_vec3 Get_ColliderPos();
	void Set_Normal();
	_vec3 Get_Normal(CollideFace eColliderFace);
	const ColliderType& Get_ColliderType() const;

#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif // _DEBUG

private:
	ColliderType m_eType{ ColliderType::End };
	void* m_pBounder{ nullptr };
	void* m_pBounder_Origin{ nullptr };
	_bool m_isCollided{};
	_vec3 m_vNormals[C_END]{};

#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>* m_pBatch{ nullptr };
	BasicEffect* m_pEffect{ nullptr };
	ID3D11InputLayout* m_pInputLayout{ nullptr };
#endif // _DEBUG


public:
	static CCollider* Create(_dev pDevice, _context pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END