#include "OrthographicObject.h"

COrthographicObject::COrthographicObject(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

COrthographicObject::COrthographicObject(const COrthographicObject& rhs)
	: CGameObject(rhs)
{
}

const _float& COrthographicObject::Get_Depth() const
{
	return m_fDepth;
}

HRESULT COrthographicObject::Init_Prototype()
{
	return S_OK;
}

HRESULT COrthographicObject::Init(void* pArg)
{
	return S_OK;
}

void COrthographicObject::Tick(_float fTimeDelta)
{
}

void COrthographicObject::Late_Tick(_float fTimeDelta)
{
}

HRESULT COrthographicObject::Render()
{
	return S_OK;
}

void COrthographicObject::Apply_Orthographic(_uint iWinSizeX, _uint iWinSizeY)
{
	m_pTransformCom->Set_Scale(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Set_State(State::Pos, XMVectorSet(m_fX - iWinSizeX * 0.5f, -m_fY + iWinSizeY * 0.5f, 0.f, 1.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(static_cast<_float>(iWinSizeX), static_cast<_float>(iWinSizeY), 0.f, 1.1f));
}

const _vec2 COrthographicObject::Convert_To_2D(CTransform* pTransform) const
{

	_mat	ProjMatrix = m_pGameInstance->Get_Transform(TransformType::Proj);
	_mat	ViewMatrix = m_pGameInstance->Get_Transform(TransformType::View);
	_mat	WorldMatrix = XMMatrixIdentity();

	_vec4 vPos= XMVector3Project(pTransform->Get_State(State::Pos), 0, 0, 1280, 720, 0, 1, ProjMatrix, ViewMatrix, XMLoadFloat4x4(&WorldMatrix));

	if (vPos.z > 1.f)
	{
		vPos.x = -100.f;
		vPos.y = -100.f;
	}
	_vec2 vPosition2D = _vec2(vPos.x, vPos.y);
	return vPosition2D;
}

void COrthographicObject::Free()
{
	__super::Free();
}
