#include "Collision_Manager.h"
#include "GameObject.h"

HRESULT CCollision_Manager::Register_CollisionObject(CGameObject* pObject, class CCollider* pHitCollider, _bool IsPlayer, class CCollider* AttRangeCollider, class CCollider* ParryingColliderr)
{
	if (not pObject or not pHitCollider)
	{
		return E_FAIL;
	}

	if (IsPlayer)
	{
		if (m_pPlayer)
		{
			MSG_BOX("Player Already Exist! : Collision_Manager");
			return E_FAIL;
		}
		m_pPlayer = pObject;
		m_pPlayerHitCollider = pHitCollider;
		m_pPlayerAttRangeCollider = AttRangeCollider;
		m_pPlayerParryingCollider = ParryingColliderr;
		Safe_AddRef(m_pPlayer);
		Safe_AddRef(m_pPlayerHitCollider);
		Safe_AddRef(m_pPlayerAttRangeCollider);
		Safe_AddRef(m_pPlayerParryingCollider);
	}
	else
	{
		auto iter = m_Monsters.find(pObject);
		if (iter != m_Monsters.end())
		{
			MSG_BOX("Monster Already Exist! : Collision_Manager");
			return E_FAIL;
		}

		m_Monsters.emplace(pObject, pHitCollider);
		Safe_AddRef(pObject);
		Safe_AddRef(pHitCollider);
	}

	return S_OK;
}


HRESULT CCollision_Manager::Register_CollisionCulling(CGameObject* pObject, class CCollider* pCollider)
{
	if (not pObject)
	{
		return E_FAIL;
	}

	auto iter = m_Objects.find(pObject);
	if (iter != m_Objects.end())
	{
		MSG_BOX("Objects Already Exist! : Collision_Manager");
		return E_FAIL;
	}
	m_Objects.emplace(pObject, pCollider);
	Safe_AddRef(pObject);
	Safe_AddRef(pCollider);

	return S_OK;
}

void CCollision_Manager::Delete_CollisionObject(CGameObject* pObject, _bool IsPlayer)
{
	if (IsPlayer)
	{
		Safe_Release(m_pPlayer);
		Safe_Release(m_pPlayerHitCollider);

		m_pPlayer = nullptr;
		m_pPlayerHitCollider = nullptr;
	}
	else
	{
		auto iter = m_Monsters.find(pObject);
		if (iter != m_Monsters.end())
		{
			Safe_Release(const_cast<CGameObject*>(iter->first));
			Safe_Release(iter->second);
			m_Monsters.erase(iter);
		}
	}
}

void CCollision_Manager::Attack_Monster(CCollider* pCollider, _uint iDamage, _uint iDamageType)
{
	for (auto& Monster : m_Monsters)
	{
		if (Monster.second->Intersect(pCollider))
		{
			Monster.first->Set_Damage(iDamage, iDamageType);
		}
	}
}

_bool CCollision_Manager::CheckCollision_Monster(CCollider* pCollider)
{
	for (auto& Monster : m_Monsters)
	{
		if (Monster.second->Intersect(pCollider))
		{
			return true;
		}
	}
	
	return false;
}

_bool CCollision_Manager::CheckCollision_Parrying(CCollider* pCollider)
{
	if (pCollider->Intersect(m_pPlayerParryingCollider))
	{
		if ((_bool)m_pPlayer->Get_HP())
		{
		m_pPlayer->Set_Damage(0, 1);
			return true;
		}

		return false;
	}

	return false;
}

_bool CCollision_Manager::Attack_Player(CCollider* pCollider, _uint iDamage, _uint iDamageType)
{
	if (not pCollider or pCollider->Intersect(m_pPlayerHitCollider))
	{
		m_pPlayer->Set_Damage(iDamage, iDamageType);
		return true;
	}

	return false;
}

_bool CCollision_Manager::CheckCollision_Player(CCollider* pCollider)
{
	if (pCollider->Intersect(m_pPlayerHitCollider))
	{
		return true;
	}

	return false;
}

_bool CCollision_Manager::CheckCollision_Culling(CCollider* pCollider)
{
	if (not m_pObjectCollider)
	{
		return false;
	}
	if (pCollider->Intersect(m_pObjectCollider))
	{
		return true;
	}

	return false;
}

class CCollider* CCollision_Manager::Get_Nearest_MonsterCollider()
{

	CCollider* NearestMonsterCollider = nullptr;
	CGameObject* NearestMonsterObject = nullptr;
	_float minDistance = 300.f;

	for (auto& Monster : m_Monsters)
	{
		if (Monster.second->Intersect(m_pPlayerAttRangeCollider))
		{
			_vec4 monsterPos = _vec4(Monster.second->Get_ColliderPos(),1.f);
			_float distance = _vec4::Distance(_vec4(m_pPlayerHitCollider->Get_ColliderPos(), 1.f), monsterPos);
			if (distance < minDistance)
			{
				minDistance = distance;
				NearestMonsterCollider = Monster.second;
				NearestMonsterObject = Monster.first;
			}
		}
	}
	
	if (NearestMonsterObject)
	{
		NearestMonsterObject->Set_Damage(0, 11);
	}
	return NearestMonsterCollider;
}

CollideFace CCollision_Manager::Get_CollideFace(CCollider* pAABBCollider, CCollider* pShereCollider)
{
	_vec3 vNormal{};
	CollideFace eCollideFace = C_END;
	if (pAABBCollider->Get_ColliderType() != ColliderType::AABB || pShereCollider->Get_ColliderType() != ColliderType::Sphere)
	{
		return eCollideFace;
	}

	_vec3 vNormals[C_END]{};
	vNormals[C_TOP] = _vec3(0.f, 1.f, 0.f);
	vNormals[C_BOTTOM] = _vec3(0.f, -1.f, 0.f);
	vNormals[C_FRONT] = _vec3(0.f, 0.f, 1.f);
	vNormals[C_BACK] = _vec3(0.f, 0.f, -1.f);
	vNormals[C_LEFT] = _vec3(-1.f, 0.f, 0.f);
	vNormals[C_RIGHT] = _vec3(1.f, 0.f, 0.f);

	_vec3 vPlaneDot[C_END]{};
	vPlaneDot[C_TOP] = _vec3(pAABBCollider->Get_ColliderPos().x, pAABBCollider->Get_ColliderPos().y + pAABBCollider->Get_Extents().y, pAABBCollider->Get_ColliderPos().z);
	vPlaneDot[C_BOTTOM] = _vec3(pAABBCollider->Get_ColliderPos().x, pAABBCollider->Get_ColliderPos().y - pAABBCollider->Get_Extents().y, pAABBCollider->Get_ColliderPos().z);
	vPlaneDot[C_FRONT] = _vec3(pAABBCollider->Get_ColliderPos().x, pAABBCollider->Get_ColliderPos().y, pAABBCollider->Get_ColliderPos().z + pAABBCollider->Get_Extents().z);
	vPlaneDot[C_BACK] = _vec3(pAABBCollider->Get_ColliderPos().x, pAABBCollider->Get_ColliderPos().y, pAABBCollider->Get_ColliderPos().z - pAABBCollider->Get_Extents().z);
	vPlaneDot[C_LEFT] = _vec3(pAABBCollider->Get_ColliderPos().x - pAABBCollider->Get_Extents().x, pAABBCollider->Get_ColliderPos().y, pAABBCollider->Get_ColliderPos().z);
	vPlaneDot[C_RIGHT] = _vec3(pAABBCollider->Get_ColliderPos().x + pAABBCollider->Get_Extents().x, pAABBCollider->Get_ColliderPos().y, pAABBCollider->Get_ColliderPos().z);

	for (size_t i = 0; i < C_END; i++)
	{
		_vec3 vLine = pShereCollider->Get_ColliderPos() - pAABBCollider->Get_ColliderPos();
		_float t = XMVectorGetX(XMVector3Dot(vNormals[i], vPlaneDot[i] - pAABBCollider->Get_ColliderPos())) / XMVectorGetX(XMVector3Dot(vNormals[i], vLine));
		if (t >= 0 && t <= 1)
		{
			eCollideFace = (CollideFace)i;
			break;
		}
	}

	if (eCollideFace == C_END)
	{
		_vec3 vDir = pShereCollider->Get_ColliderPos() - pAABBCollider->Get_ColliderPos();
		vDir.Normalize();

		_float fMaxForProduct{ -1.f };
		for (size_t i = 0; i < C_END; i++)
		{
			_float fDotProduct = XMVectorGetX(XMVector3Dot(vNormals[i], vDir));
			if (fDotProduct > fMaxForProduct)
			{
				fMaxForProduct = fDotProduct;
				eCollideFace = (CollideFace)i;
			}
		}
	}

	return eCollideFace;
	/*
	*     XMVECTOR V = XMLoadFloat3(&P2) - XMLoadFloat3(&P1);

    // Calculate the scalar value t
    float t = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&N), XMLoadFloat3(&P0) - XMLoadFloat3(&P1))) / XMVectorGetX(XMVector3Dot(XMLoadFloat3(&N), V));
	*/
	/*
	
	_vec3 vSphereCenter = pShereCollider->Get_ColliderPos();
	_vec3 vAABBMin = pAABBCollider->Get_ColliderPos() - pAABBCollider->Get_Extents();
	_vec3 vAABBMax = pAABBCollider->Get_ColliderPos() + pAABBCollider->Get_Extents();


	float dist_min_x = abs(vSphereCenter.x - vAABBMin.x);
	float dist_max_x = abs(vSphereCenter.x - vAABBMax.x);
	float dist_min_z = abs(vSphereCenter.z - vAABBMin.z);
	float dist_max_z = abs(vSphereCenter.z - vAABBMax.z);

	vector<float> vecDists = { dist_min_x, dist_max_x, dist_min_z, dist_max_z };
	float fMinDist = *min_element(vecDists.begin(), vecDists.end());

	if (fMinDist == dist_min_x)
	{
		eCollideFace = C_BACK;
	}
	else if (fMinDist == dist_max_x)
	{
		eCollideFace = C_FRONT;
	}
	else if (fMinDist == dist_min_z)
	{
		eCollideFace = C_LEFT;
	}
	else if (fMinDist == dist_max_z)
	{
		eCollideFace = C_RIGHT;
	}

	return eCollideFace;*/
}
CCollision_Manager* CCollision_Manager::Create()
{
	CCollision_Manager* pInstance = new CCollision_Manager();

	return pInstance;
}

void CCollision_Manager::Free()
{
	Safe_Release(m_pPlayer);
	Safe_Release(m_pPlayerHitCollider);
	Safe_Release(m_pPlayerParryingCollider);
	Safe_Release(m_pPlayerAttRangeCollider);

	for (auto& Monster : m_Monsters)
	{
		Safe_Release(Monster.second);
		Safe_Release(const_cast<CGameObject*>(Monster.first));
	}

	for (auto& Obejcts : m_Objects)
	{
		Safe_Release(Obejcts.second);
		Safe_Release(const_cast<CGameObject*>(Obejcts.first));
	}

	m_Monsters.clear();
	m_Objects.clear();
}
