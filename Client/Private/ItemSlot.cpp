#include "ItemSlot.h"
#include "GameInstance.h"
#include "TextButton.h"

CItemSlot::CItemSlot(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CItemSlot::CItemSlot(const CItemSlot& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CItemSlot::Init_Prototype()
{
	return S_OK;
}

HRESULT CItemSlot::Init(void* pArg)
{
	m_eSlotMode = ((ITEMSLOT_DESC*)pArg)->eSlotMode;
	switch (m_eSlotMode)
	{
	case ITSLOT_SCREEN:
		m_strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Slot");
		break;
	case ITSLOT_INVEN:
		m_strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_invenslot");
		break;
	}
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = ((ITEMSLOT_DESC*)pArg)->vSize.x;
	m_fSizeY = ((ITEMSLOT_DESC*)pArg)->vSize.y;

	m_fX = ((ITEMSLOT_DESC*)pArg)->vPosition.x;
	m_fY = ((ITEMSLOT_DESC*)pArg)->vPosition.y;

	m_fDepth = ((ITEMSLOT_DESC*)pArg)->fDepth;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	
	m_rcRect = {
		  (LONG)(m_fX - m_fSizeX * 0.5f),
		  (LONG)(m_fY - m_fSizeY * 0.5f),
		  (LONG)(m_fX + m_fSizeX * 0.5f),
		  (LONG)(m_fY + m_fSizeY * 0.5f)
	};

	return S_OK;
}

void CItemSlot::Tick(_float fTimeDelta)
{

}

void CItemSlot::Late_Tick(_float fTimeDelta)
{
	if (m_pItem != nullptr)
	{
		m_pItem->Late_Tick(fTimeDelta);
	}
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CItemSlot::Render()
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

HRESULT CItemSlot::Set_Item(CItem* pItem, _int* iNum)
{
	if (pItem == nullptr)
	{
		return E_FAIL;
	}
	if (m_pItem != nullptr)
	{
		Safe_Release(m_pItem);
	}
	_uint iItemNum = pItem->Get_ItemNum();
	if (iItemNum > m_iMaxNum)
	{
		*iNum = m_iMaxNum;
		iItemNum = m_iMaxNum;
	}
	else
	{
		*iNum = 0;
	}
	CItem::ITEM_DESC ItemDesc = {};
	ItemDesc.bCanInteract = false;
	ItemDesc.eItemDesc = pItem->Get_ItemDesc();
	ItemDesc.fDepth = m_fDepth - 0.1f;
	ItemDesc.iNum = iItemNum;
	ItemDesc.vPosition = _vec2(m_fX, m_fY);
	if (m_eSlotMode == ITSLOT_SCREEN)
	{
		ItemDesc.vSize = _vec2(m_fSizeX - 20.f, m_fSizeY - 20.f);
		ItemDesc.isScreen = true;
	}
	else
	{
		ItemDesc.vSize = _vec2(m_fSizeX, m_fSizeY);
		ItemDesc.isScreen = false;
	}

	m_pItem = (CItem*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Item"), &ItemDesc);

	m_isFull = true;
	return S_OK;
}



void CItemSlot::Use_Item()
{
	if (m_pItem == nullptr)
	{
		return;
	}
	m_pItem->Set_ItemNum(-1);
	if (m_pItem->Get_ItemNum() <= 0)
	{
		Delete_Item();
	}
}

void CItemSlot::Delete_Item()
{
	Safe_Release(m_pItem);
	m_isFull = false;
}

void CItemSlot::Set_FullSlot(CItem* pItem, _int* iNum)
{
	if (m_pItem == nullptr)
	{
		return;
	}
	_uint iCurItemNum = m_pItem->Get_ItemNum();
	if (iCurItemNum >= m_iMaxNum)
	{
		*iNum = -1; //꽉차서 못들어감
	}
	else if (iCurItemNum + pItem->Get_ItemNum() > m_iMaxNum)
	{
		*iNum = m_iMaxNum - iCurItemNum;
	}
	else
	{
		*iNum = 0;
	}
	return;
}

const wstring CItemSlot::Get_ItemName()
{
	wstring strName = TEXT("");
	if (m_pItem == nullptr)
	{
		return strName;
	}
	return m_pItem->Get_ItemDesc().strName;
}

const ITEM_USAGE CItemSlot::Get_ItemUsage()
{
	if (m_pItem == nullptr)
	{
		return USAGE_END;
	}
	return m_pItem->Get_ItemDesc().eItemUsage;
}

const CItem* CItemSlot::Get_ItemObject()
{
	if (m_pItem == nullptr)
	{
		return nullptr;
	}

	return m_pItem;
}
HRESULT CItemSlot::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_strTexture, TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CItemSlot::Bind_ShaderResources()
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

CItemSlot* CItemSlot::Create(_dev pDevice, _context pContext)
{
	CItemSlot* pInstance = new CItemSlot(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CItemBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItemSlot::Clone(void* pArg)
{
	CItemSlot* pInstance = new CItemSlot(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CItemBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItemSlot::Free()
{
	__super::Free();

	Safe_Release(m_pItem);
	Safe_Release(m_pItemTex);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
