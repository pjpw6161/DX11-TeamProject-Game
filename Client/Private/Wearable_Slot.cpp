#include "Wearable_Slot.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "UI_Manager.h"

CWearable_Slot::CWearable_Slot(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CWearable_Slot::CWearable_Slot(const CWearable_Slot& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CWearable_Slot::Init_Prototype()
{
	return S_OK;
}

HRESULT CWearable_Slot::Init(void* pArg)
{
	m_fSizeX = ((WEARABLESLOT*)pArg)->vSize.x;
	m_fSizeY = ((WEARABLESLOT*)pArg)->vSize.y;
	m_fX = ((WEARABLESLOT*)pArg)->vPosition.x;
	m_fY = ((WEARABLESLOT*)pArg)->vPosition.y;

	m_fDepth = ((WEARABLESLOT*)pArg)->fDepth;
	m_eType = ((WEARABLESLOT*)pArg)->eType;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}	
	if (FAILED(Add_Slots()))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CWearable_Slot::Tick(_float fTimeDelta)
{

	m_rcRect = {
		  (LONG)(m_fX - m_fSizeX * 0.5f),
		  (LONG)(m_fY - m_fSizeY * 0.5f),
		  (LONG)(m_fX + m_fSizeX * 0.5f),
		  (LONG)(m_fY + m_fSizeY * 0.5f)
	};

}

void CWearable_Slot::Late_Tick(_float fTimeDelta)
{
	if (m_isFull)
	{
		if (m_pSelected != nullptr)
		{
			m_pSelected->Late_Tick(fTimeDelta);
		}
		if (m_pItem != nullptr)
		{
			m_pItem->Late_Tick(fTimeDelta);
		}
	}
	else
	{
		if (m_pSlotType != nullptr)
		{
			m_pSlotType->Late_Tick(fTimeDelta);
		}
	}
	_float fX = m_fX;
	_float fY = m_fY;
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CWearable_Slot::Render()
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

HRESULT CWearable_Slot::Set_WearableItem(ITEM eItemDesc)
{
	if (m_pItem != nullptr)
	{
		Safe_Release(m_pItem);
	}

	CItem::ITEM_DESC ItemDesc= {};
	ItemDesc.vSize = _float2(m_fSizeX, m_fSizeY);
	ItemDesc.vPosition = _float2(m_fX, m_fY);
	ItemDesc.bCanInteract = false;
	ItemDesc.haveBG = true;
	ItemDesc.eItemDesc = eItemDesc;
	ItemDesc.fDepth = m_fDepth - 0.1f;
	m_pItem = (CItem*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Item"), &ItemDesc);
	if (nullptr == m_pItem)
		return E_FAIL;

	m_isFull = true;
	if (m_eType == W_PET)
	{
		if (m_pGameInstance->Get_LayerSize(LEVEL_STATIC, TEXT("Layer_Pet")) != 0)
		{
			CUI_Manager::Get_Instance()->Set_Pet(true);
		}
		if (eItemDesc.strName == TEXT("고양이"))
		{
			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Pet"), TEXT("Prototype_GameObject_Pet_Cat"))))
			{
				return E_FAIL;
			}
		}
		else if (eItemDesc.strName == TEXT("드래곤"))
		{
			if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Pet"), TEXT("Prototype_GameObject_Pet_Dragon"))))
			{
				return E_FAIL;
			}
		}
		return S_OK;
	}

	_int iIndex = eItemDesc.iPartIndex;
	if (m_ePartType == PT_BOW)
	{

		CUI_Manager::Get_Instance()->Set_WeaponType(WP_BOW);
	}
	else if (m_ePartType == PT_SWORD)
	{
		CUI_Manager::Get_Instance()->Set_WeaponType(WP_SWORD);
		iIndex += 6;
	}


	CUI_Manager::Get_Instance()->Set_CustomPart(m_ePartType, iIndex, eItemDesc.iStatus);

	return S_OK;
}

const ITEM& CWearable_Slot::Get_ItemDesc() const
{
	return m_pItem->Get_ItemDesc();
}
void CWearable_Slot::Delete_Item()
{
	_int iResetIdx = -1;

	if (m_pItem != nullptr)
	{
		if (m_eType == W_PET)
		{
			if (m_pGameInstance->Get_LayerSize(LEVEL_STATIC, TEXT("Layer_Pet")) != 0)
			{
				CUI_Manager::Get_Instance()->Set_Pet(true);
			}
			m_isFull = false;
			Safe_Release(m_pItem);
			return;
		}

		ITEM eItem = Get_ItemDesc();
		if (m_ePartType == PT_BOW)
		{
			CUI_Manager::Get_Instance()->Set_WeaponType(WP_BOW);
			iResetIdx = (_uint)BOW_UNEQUIP;
		}
		else if (m_ePartType == PT_SWORD)
		{
			CUI_Manager::Get_Instance()->Set_WeaponType(WP_SWORD);
			iResetIdx = (_uint)SWORD_UNEQUIP;
		}

		Safe_Release(m_pItem);
	}

	m_isFull = false;

	if (m_ePartType == PT_BODY)
	{
		iResetIdx = 0;
	}

	CUI_Manager::Get_Instance()->Set_CustomPart(m_ePartType, iResetIdx, 0);
}
HRESULT CWearable_Slot::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_customslot"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CWearable_Slot::Bind_ShaderResources()
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

HRESULT CWearable_Slot::Add_Slots()
{
	CTextButton::TEXTBUTTON_DESC Button = {};
	Button.eLevelID = LEVEL_STATIC;
	Button.strText = TEXT("");
	Button.vPosition = _vec2(m_fX, m_fY);
	Button.vSize = _vec2(m_fSizeX, m_fSizeY);
	Button.fDepth = m_fDepth - 0.02f;

	switch (m_eType)
	{
	case W_TOP:
		Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_inven_top");
		m_ePartType = PT_HELMET;
		break;
	case W_CHEST:
		Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Inven_Chest");
		m_ePartType = PT_BODY;
		break;
	case W_HAND:
		Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Inven_Hand");
		//m_ePartType = PT_;
		break;
	case W_FOOT:
		Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Inven_Foot");
		break;
	case W_SWORD:
		Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Inven_Equip");
		m_ePartType = PT_SWORD;
		break;
	case W_BOW:
		Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Inven_Equip");
		m_ePartType = PT_BOW;
		break;
	case W_PET:
		Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Inven_Pet");
		break;
	}
	m_pSlotType = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);
	if (not m_pSlotType)
	{
		return E_FAIL;
	}

	Button.fDepth = m_fDepth - 0.01f;
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_customslotBG");
	m_pSelected = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);
	if (not m_pSelected)
	{
		return E_FAIL;
	}
	return S_OK;
}

CWearable_Slot* CWearable_Slot::Create(_dev pDevice, _context pContext)
{
	CWearable_Slot* pInstance = new CWearable_Slot(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CWearable_Slot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWearable_Slot::Clone(void* pArg)
{
	CWearable_Slot* pInstance = new CWearable_Slot(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CWearable_Slot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWearable_Slot::Free()
{
	__super::Free();

	Safe_Release(m_pItem);
	Safe_Release(m_pSlotType);
	Safe_Release(m_pSelected);

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);

}
