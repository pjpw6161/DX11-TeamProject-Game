#include "BlendObject.h"

CBlendObject::CBlendObject(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CBlendObject::CBlendObject(const CBlendObject& rhs)
	: CGameObject(rhs)
{
}

const _float& CBlendObject::Get_CamDistance() const
{
	return m_fCamDistance;
}

HRESULT CBlendObject::Init_Prototype()
{
	return S_OK;
}

HRESULT CBlendObject::Init(void* pArg)
{
	return S_OK;
}

void CBlendObject::Tick(_float fTimeDelta)
{
}

void CBlendObject::Late_Tick(_float fTimeDelta)
{
}

HRESULT CBlendObject::Render()
{
	return S_OK;
}

void CBlendObject::Compute_CamDistance()
{
	m_fCamDistance = (m_pTransformCom->Get_State(State::Pos) - m_pGameInstance->Get_CameraPos()).Length();
}

void CBlendObject::Free()
{
	__super::Free();
}
