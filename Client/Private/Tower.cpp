#include "Tower.h"
#include "GameInstance.h"
#include "TextButtonColor.h"

CTower::CTower(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CTower::CTower(const CTower& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CTower::Init_Prototype()
{
	return S_OK;
}

HRESULT CTower::Init(void* pArg)
{

	m_vDefaultPoint = ((TOWER_DESC*)pArg)->vDefaultPoint;
	m_isBossRoom = ((TOWER_DESC*)pArg)->isBossRoom;
	m_vPosition = ((TOWER_DESC*)pArg)->vPosition;
	m_vInitialPosition = m_vPosition;

	m_fSizeX = 312.f;
	m_fSizeY = 137.f;

	m_fX = (*m_vDefaultPoint).x;
	m_fY = (*m_vDefaultPoint).y + m_vPosition.y;

	m_fDepth = (_float)D_WINDOW / (_float)D_END - 0.05f;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}
	return S_OK;
}

void CTower::Tick(_float fTimeDelta)
{
	m_fX = (*m_vDefaultPoint).x;
	m_fY = (*m_vDefaultPoint).y + m_vPosition.y;


	m_rcRect = {
		(LONG)(m_fX - m_fSizeX * 0.5f),
		(LONG)(m_fY - m_fSizeY * 0.5f),
		(LONG)(m_fX + m_fSizeX * 0.5f),
		(LONG)(m_fY + m_fSizeY * 0.5f),
	};


	if (!m_isCleared)
	{
		m_pDoor->Set_Position(_vec2(m_fX, m_fY));
		m_pDoor->Tick(fTimeDelta);
	}
	if (m_isSelected)
	{
		m_pSelectMask->Set_Position(_vec2(m_fX, m_fY));
		m_pSelectMask->Tick(fTimeDelta);
	}
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
}

void CTower::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
	if (!m_isCleared)
	{
		m_pDoor->Late_Tick(fTimeDelta);
	}
	if (m_isSelected)
	{
		m_pSelectMask->Late_Tick(fTimeDelta);
	}

}

HRESULT CTower::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_UI)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}

const _vec2 CTower::Get_Position()
{
	return _vec2(m_fX, m_fY);
}

void CTower::Select_Object(_bool isSelect)
{
	m_isSelected = isSelect;
}

HRESULT CTower::Add_Parts()
{
	CTextButtonColor::TEXTBUTTON_DESC ColButtonDesc = {};
	ColButtonDesc.eLevelID = LEVEL_STATIC;
	ColButtonDesc.fDepth = m_fDepth - 0.01f;
	ColButtonDesc.fAlpha = 1.f;
	ColButtonDesc.strText = TEXT("");
	ColButtonDesc.vSize = _vec2(m_fSizeX, m_fSizeY);
	ColButtonDesc.vPosition = _vec2(m_fX, m_fY);
	if (m_isBossRoom)
	{
		ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_InfinitytowerBossdoor");
	}
	else
	{
		ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Infinitytowerdoor");
	}
	m_pDoor = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pDoor)
	{
		return E_FAIL;
	}
	dynamic_cast<CTextButtonColor*>(m_pDoor)->Set_Pass(VTPass_UI);

	ColButtonDesc.fAlpha = 0.8f;
	ColButtonDesc.vColor = _vec4(1.f, 0.8f, 0.3f, 1.f);
	ColButtonDesc.vSize = _vec2(m_fSizeX+ 200.f, m_fSizeY + 120.f);
	if (m_isBossRoom)
	{
		ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_InfinityTowerFloor_00");
	}
	else
	{
		ColButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_InfinityTowerFloor_01");
	}
	m_pSelectMask = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &ColButtonDesc);
	if (not m_pSelectMask)
	{
		return E_FAIL;
	}
	dynamic_cast<CTextButtonColor*>(m_pSelectMask)->Set_Pass(VTPass_Mask_ColorAlpha);


	return S_OK;
}

HRESULT CTower::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	{
		return E_FAIL;
	}

	if (m_isBossRoom)
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_Infinitytowerboss"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		{
			return E_FAIL;
		}
	}
	else
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_Infinitytower"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		{
			return E_FAIL;
		}
	}


	return S_OK;
}

HRESULT CTower::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_ViewMatrix))
		|| FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
	{
		return E_FAIL;
	}

	return S_OK;
}

CTower* CTower::Create(_dev pDevice, _context pContext)
{
	CTower* pInstance = new CTower(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CTower");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTower::Clone(void* pArg)
{
	CTower* pInstance = new CTower(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CTower");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTower::Free()
{
	__super::Free();

	Safe_Release(m_pDoor);
	Safe_Release(m_pSelectMask);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
