#include "NameTag.h"
#include "GameInstance.h"

CNameTag::CNameTag(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CNameTag::CNameTag(const CNameTag& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CNameTag::Init_Prototype()
{
	return S_OK;
}

HRESULT CNameTag::Init(void* pArg)
{

	m_fDepth = (_float)D_NAMETAG / (_float)D_END;
	m_eLevel = ((NAMETAG_DESC*)pArg)->eLevelID;
	m_fFontSize = ((NAMETAG_DESC*)pArg)->fFontSize;
	m_vTextPosition = ((NAMETAG_DESC*)pArg)->vTextPosition;
	m_vColor = ((NAMETAG_DESC*)pArg)->vColor;
	m_pParentTransform = ((NAMETAG_DESC*)pArg)->pParentTransform;
	Safe_AddRef(m_pParentTransform);
	m_strNameTag = ((NAMETAG_DESC*)pArg)->strNameTag;


	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CNameTag::Tick(_float fTimeDelta)
{
	if (m_isCoord)
	{
		m_pTransformCom->Set_State(State::Pos, m_vPos + m_vTextPosition);
	}
	else
	{
		//DirectX::XMFLOAT3 = {x=149.119095 y=-6.72868681 z=121.260971 }
		m_pTransformCom->Set_State(State::Pos, m_pParentTransform->Get_State(State::Pos) + m_vTextPosition);
	}

}

void CNameTag::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CNameTag::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	_vec2 v2DPos = __super::Convert_To_2D(m_pTransformCom);
	m_pGameInstance->Render_Text(L"Font_Malang", m_strNameTag, _vec2(v2DPos.x + 1.f, v2DPos.y), m_fFontSize, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", m_strNameTag, _vec2(v2DPos.x - 1.f, v2DPos.y), m_fFontSize, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", m_strNameTag, _vec2(v2DPos.x, v2DPos.y + 1.f), m_fFontSize, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", m_strNameTag, _vec2(v2DPos.x, v2DPos.y - 1.f), m_fFontSize, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", m_strNameTag, v2DPos, m_fFontSize, m_vColor);

	return S_OK;
}

HRESULT CNameTag::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CNameTag::Bind_ShaderResources()
{
	return S_OK;
}

CNameTag* CNameTag::Create(_dev pDevice, _context pContext)
{
	CNameTag* pInstance = new CNameTag(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CNameTag");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNameTag::Clone(void* pArg)
{
	CNameTag* pInstance = new CNameTag(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CNameTag");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNameTag::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pParentTransform);
}
