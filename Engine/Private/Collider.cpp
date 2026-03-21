#include "Collider.h"
#include "DebugDraw.h"
#include "GameInstance.h"

CCollider::CCollider(_dev pDevice, _context pContext)
	: CComponent(pDevice, pContext)
{
}

CCollider::CCollider(const CCollider& rhs)
	: CComponent(rhs)
#ifdef _DEBUG
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
#endif // _DEBUG

{
#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif // _DEBUG

}

HRESULT CCollider::Init_Prototype()
{
#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderBytecode{ nullptr };
	size_t iLength{};

	m_pEffect->GetVertexShaderBytecode(&pShaderBytecode, &iLength);

	m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderBytecode, iLength, &m_pInputLayout);
#endif // _DEBUG

	return S_OK;
}

HRESULT CCollider::Init(void* pArg)
{
	if (not pArg)
	{
		MSG_BOX("Missing Argument");
		return E_FAIL;
	}

	Collider_Desc Desc = *reinterpret_cast<Collider_Desc*>(pArg);
	m_eType = Desc.eType;

	switch (m_eType)
	{
	case Engine::ColliderType::AABB:
		m_pBounder = new BoundingBox(Desc.vCenter, Desc.vExtents);
		m_pBounder_Origin = new BoundingBox(*reinterpret_cast<BoundingBox*>(m_pBounder));
		break;
	case Engine::ColliderType::OBB:
	{
		_float4 vRotation{};
		XMStoreFloat4(&vRotation, XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&Desc.vRadians)));
		m_pBounder = new BoundingOrientedBox(Desc.vCenter, Desc.vExtents, vRotation);
		m_pBounder_Origin = new BoundingOrientedBox(*reinterpret_cast<BoundingOrientedBox*>(m_pBounder));
		break;
	}
	case Engine::ColliderType::Sphere:
		m_pBounder = new BoundingSphere(Desc.vCenter, Desc.fRadius);
		m_pBounder_Origin = new BoundingSphere(*reinterpret_cast<BoundingSphere*>(m_pBounder));
		break;
	case Engine::ColliderType::Frustum:
		m_pBounder = new BoundingFrustum(XMLoadFloat4x4(&Desc.matFrustum));
		m_pBounder_Origin = new BoundingFrustum(*reinterpret_cast<BoundingFrustum*>(m_pBounder));
		break;
	}

	return S_OK;
}

void CCollider::Update(_fmatrix WorldMatrix)
{
	switch (m_eType)
	{
	case Engine::ColliderType::AABB:
		_matrix Transform = WorldMatrix;
		Transform.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(Transform.r[0]));
		Transform.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(Transform.r[1]));
		Transform.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(Transform.r[2]));
		reinterpret_cast<BoundingBox*>(m_pBounder_Origin)->Transform(*reinterpret_cast<BoundingBox*>(m_pBounder), Transform);
		break;
	case Engine::ColliderType::OBB:
		reinterpret_cast<BoundingOrientedBox*>(m_pBounder_Origin)->Transform(*reinterpret_cast<BoundingOrientedBox*>(m_pBounder), WorldMatrix);
		break;
	case Engine::ColliderType::Sphere:
		reinterpret_cast<BoundingSphere*>(m_pBounder_Origin)->Transform(*reinterpret_cast<BoundingSphere*>(m_pBounder), WorldMatrix);
		break;
	case Engine::ColliderType::Frustum:
		reinterpret_cast<BoundingFrustum*>(m_pBounder_Origin)->Transform(*reinterpret_cast<BoundingFrustum*>(m_pBounder), WorldMatrix);
		break;
	}
	
}

_bool CCollider::Intersect(const CCollider* pTargetCollider)
{
	switch (pTargetCollider->m_eType)
	{
	case Engine::ColliderType::AABB:
		switch (m_eType)
		{
		case Engine::ColliderType::AABB:
			m_isCollided = reinterpret_cast<BoundingBox*>(m_pBounder)->Intersects(*reinterpret_cast<BoundingBox*>(pTargetCollider->m_pBounder));
			break;
		case Engine::ColliderType::OBB:
			m_isCollided = reinterpret_cast<BoundingOrientedBox*>(m_pBounder)->Intersects(*reinterpret_cast<BoundingBox*>(pTargetCollider->m_pBounder));
			break;
		case Engine::ColliderType::Sphere:
			m_isCollided = reinterpret_cast<BoundingSphere*>(m_pBounder)->Intersects(*reinterpret_cast<BoundingBox*>(pTargetCollider->m_pBounder));
			break;
		case Engine::ColliderType::Frustum:
			m_isCollided = reinterpret_cast<BoundingFrustum*>(m_pBounder)->Intersects(*reinterpret_cast<BoundingBox*>(pTargetCollider->m_pBounder));
			break;
		}
		break;
	case Engine::ColliderType::OBB:
		switch (m_eType)
		{
		case Engine::ColliderType::AABB:
			m_isCollided = reinterpret_cast<BoundingBox*>(m_pBounder)->Intersects(*reinterpret_cast<BoundingOrientedBox*>(pTargetCollider->m_pBounder));
			break;
		case Engine::ColliderType::OBB:
			m_isCollided = reinterpret_cast<BoundingOrientedBox*>(m_pBounder)->Intersects(*reinterpret_cast<BoundingOrientedBox*>(pTargetCollider->m_pBounder));
			break;
		case Engine::ColliderType::Sphere:
			m_isCollided = reinterpret_cast<BoundingSphere*>(m_pBounder)->Intersects(*reinterpret_cast<BoundingOrientedBox*>(pTargetCollider->m_pBounder));
			break;
		case Engine::ColliderType::Frustum:
			m_isCollided = reinterpret_cast<BoundingFrustum*>(m_pBounder)->Intersects(*reinterpret_cast<BoundingOrientedBox*>(pTargetCollider->m_pBounder));
			break;
		}
		break;
	case Engine::ColliderType::Sphere:
		switch (m_eType)
		{
		case Engine::ColliderType::AABB:
			m_isCollided = reinterpret_cast<BoundingBox*>(m_pBounder)->Intersects(*reinterpret_cast<BoundingSphere*>(pTargetCollider->m_pBounder));
			break;
		case Engine::ColliderType::OBB:
			m_isCollided = reinterpret_cast<BoundingOrientedBox*>(m_pBounder)->Intersects(*reinterpret_cast<BoundingSphere*>(pTargetCollider->m_pBounder));
			break;
		case Engine::ColliderType::Sphere:
			m_isCollided = reinterpret_cast<BoundingSphere*>(m_pBounder)->Intersects(*reinterpret_cast<BoundingSphere*>(pTargetCollider->m_pBounder));
			break;
		case Engine::ColliderType::Frustum:
			m_isCollided = reinterpret_cast<BoundingFrustum*>(m_pBounder)->Intersects(*reinterpret_cast<BoundingSphere*>(pTargetCollider->m_pBounder));
			break;
		}
		break;
	case Engine::ColliderType::Frustum:
		switch (m_eType)
		{
		case Engine::ColliderType::AABB:
			m_isCollided = reinterpret_cast<BoundingBox*>(m_pBounder)->Intersects(*reinterpret_cast<BoundingFrustum*>(pTargetCollider->m_pBounder));
			break;
		case Engine::ColliderType::OBB:
			m_isCollided = reinterpret_cast<BoundingOrientedBox*>(m_pBounder)->Intersects(*reinterpret_cast<BoundingFrustum*>(pTargetCollider->m_pBounder));
			break;
		case Engine::ColliderType::Sphere:
			m_isCollided = reinterpret_cast<BoundingSphere*>(m_pBounder)->Intersects(*reinterpret_cast<BoundingFrustum*>(pTargetCollider->m_pBounder));
			break;
		case Engine::ColliderType::Frustum:
			m_isCollided = reinterpret_cast<BoundingFrustum*>(m_pBounder)->Intersects(*reinterpret_cast<BoundingFrustum*>(pTargetCollider->m_pBounder));
			break;
		}
		break;
	}

	return m_isCollided;
}

void CCollider::Change_Extents(_vec3 vExtents)
{
	switch (m_eType)
	{
	case Engine::ColliderType::AABB:
		reinterpret_cast<BoundingBox*>(m_pBounder_Origin)->Extents = vExtents;
		break;
	case Engine::ColliderType::OBB:
		reinterpret_cast<BoundingOrientedBox*>(m_pBounder_Origin)->Extents = vExtents;
		reinterpret_cast<BoundingOrientedBox*>(m_pBounder_Origin)->Center.y = vExtents.y;
		break;
	}
}

void CCollider::Set_Radius(const _float fRadius) const
{
	if (m_eType != ColliderType::Sphere)
	{
		return;
	}

	reinterpret_cast<BoundingSphere*>(m_pBounder)->Radius = fRadius;
	reinterpret_cast<BoundingSphere*>(m_pBounder_Origin)->Radius = fRadius;
}

_vec3 CCollider::Get_Extents()
{
	switch (m_eType)
	{
	case Engine::ColliderType::AABB:
		return reinterpret_cast<BoundingBox*>(m_pBounder_Origin)->Extents;
		break;
	case Engine::ColliderType::OBB:
		return reinterpret_cast<BoundingOrientedBox*>(m_pBounder_Origin)->Extents;
		break;
	}

	return _vec3{};
}

_vec3 CCollider::Get_ColliderPos()
{
	switch (m_eType)
	{
	case Engine::ColliderType::AABB:
		return reinterpret_cast<BoundingBox*>(m_pBounder)->Center;
		break;
	case Engine::ColliderType::OBB:
		return reinterpret_cast<BoundingOrientedBox*>(m_pBounder)->Center;
		break;
	case Engine::ColliderType::Sphere:
		return reinterpret_cast<BoundingSphere*>(m_pBounder)->Center;
		break;
	}


	return _vec3();
}

void CCollider::Set_Normal()
{
	switch (m_eType)
	{
	case Engine::ColliderType::AABB:
		_vec3 vExtents = reinterpret_cast<BoundingBox*>(m_pBounder_Origin)->Extents;

		m_vNormals[C_TOP] = _vec3(0.f, 1.f, 0.f);
		m_vNormals[C_BOTTOM] = _vec3(0.f, -1.f, 0.f);
		m_vNormals[C_FRONT] = _vec3(0.f, 0.f, 1.f);
		m_vNormals[C_BACK] = _vec3(0.f, 0.f, -1.f);
		m_vNormals[C_LEFT] = _vec3(-1.f, 0.f, 0.f);
		m_vNormals[C_RIGHT] = _vec3(1.f, 0.f, 0.f);
	}
	
}

_vec3 CCollider::Get_Normal(CollideFace eColliderFace)
{
	_vec3 vNormal{};
	if (eColliderFace < C_END)
	{
		return m_vNormals[eColliderFace];
	}

	return vNormal;
}

const ColliderType& CCollider::Get_ColliderType() const
{
	return m_eType;
}

#ifdef _DEBUG
HRESULT CCollider::Render()
{
	m_pEffect->SetWorld(XMMatrixIdentity());
	m_pEffect->SetView(m_pGameInstance->Get_Transform(TransformType::View));
	m_pEffect->SetProjection(m_pGameInstance->Get_Transform(TransformType::Proj));

	m_pContext->GSSetShader(nullptr, nullptr, 0);

	m_pBatch->Begin();

	m_pContext->IASetInputLayout(m_pInputLayout);
	m_pEffect->Apply(m_pContext);

	_vector vColor{};
	if (m_isCollided)
	{
		vColor = Colors::Red;
	}
	else
	{
		vColor = Colors::Green;
	}

	switch (m_eType)
	{
	case Engine::ColliderType::AABB:
		DX::Draw(m_pBatch, *reinterpret_cast<BoundingBox*>(m_pBounder), vColor);
		break;
	case Engine::ColliderType::OBB:
		DX::Draw(m_pBatch, *reinterpret_cast<BoundingOrientedBox*>(m_pBounder), vColor);
		break;
	case Engine::ColliderType::Sphere:
		DX::Draw(m_pBatch, *reinterpret_cast<BoundingSphere*>(m_pBounder), vColor);
		break;
	case Engine::ColliderType::Frustum:
		DX::Draw(m_pBatch, *reinterpret_cast<BoundingFrustum*>(m_pBounder), vColor);
		break;
	}

	m_pBatch->End();

	return S_OK;
}
#endif // _DEBUG


CCollider* CCollider::Create(_dev pDevice, _context pContext)
{
	CCollider* pInstance = new CCollider(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCollider::Clone(void* pArg)
{
	CCollider* pInstance = new CCollider(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

	Safe_Delete(m_pBounder);
	Safe_Delete(m_pBounder_Origin);
#ifdef _DEBUG
	Safe_Release(m_pInputLayout);

	if (not m_hasCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}
#endif // _DEBUG

}
