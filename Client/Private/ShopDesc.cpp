#include "ShopDesc.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "TextButton.h"
#include "ItemInfo.h"
#include "Item.h"

CShopDesc::CShopDesc(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CShopDesc::CShopDesc(const CShopDesc& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CShopDesc::Init_Prototype()
{
	return S_OK;
}

HRESULT CShopDesc::Init(void* pArg)
{

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}


	m_fSizeX = 300.f;
	m_fSizeY = 90.f;

	m_fX = ((SHOPITEM_DESC*)pArg)->vPosition.x;
	m_fY = ((SHOPITEM_DESC*)pArg)->vPosition.y;

	m_fDepth = ((SHOPITEM_DESC*)pArg)->fDepth;
	m_strItemName = ((SHOPITEM_DESC*)pArg)->strItemName;
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	

	m_rcRect = {
		  (LONG)(m_fX - m_fSizeX * 0.5f),
		  (LONG)(m_fY - m_fSizeY * 0.5f),
		  (LONG)(m_fX + m_fSizeX * 0.5f),
		  (LONG)(m_fY + m_fSizeY * 0.5f)
	};

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CShopDesc::Tick(_float fTimeDelta)
{
	m_rcRect = {
	  (LONG)(m_fX - m_fSizeX * 0.5f),
	  (LONG)(m_fY - m_fSizeY * 0.5f),
	  (LONG)(m_fX + m_fSizeX * 0.5f),
	  (LONG)(m_fY + m_fSizeY * 0.5f)
	};
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	/*
	
	if (m_bCanInteract)
	{
		if (PtInRect(&m_rcRect, ptMouse) && m_pGameInstance->Mouse_Down(DIM_RBUTTON, InputChannel::UI))
		{
			if (m_pGameInstance->Get_LayerSize(LEVEL_STATIC, TEXT("Layer_ItemInfo")) == 0)
			{
				CItemInfo::ITEMINFO_DESC InfoDesc = {};
				InfoDesc.eItemDesc = m_eItemDesc;
				InfoDesc.fDepth = m_fDepth - 0.05f;
				if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_ItemInfo"), TEXT("Prototype_GameObject_ItemInfo"), &InfoDesc)))
				{
					return;
				}
			}

		}
	}

	*/
	m_pItem->Tick(fTimeDelta);

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
}

void CShopDesc::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
	m_pItem->Late_Tick(fTimeDelta);

}

HRESULT CShopDesc::Render()
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

	_vec2 vStartPos = _vec2(m_fX - m_fSizeX / 2.f + 90.f, m_fY - 30.f);
	m_pGameInstance->Render_Text(L"Font_Malang", m_strItemName, _vec2(vStartPos.x + 1.f, vStartPos.y), 0.4f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", m_strItemName, _vec2(vStartPos.x, +vStartPos.y + 1.f), 0.4f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", m_strItemName, _vec2(vStartPos.x, +vStartPos.y), 0.4f, _vec4(1.f, 1.f, 1.f, 1.f), 0.f, true);
	
	vStartPos = _vec2(m_fX - m_fSizeX / 2.f + 90.f + 25.f, m_fY + 13.f);
	wstring strCost = to_wstring(m_pItem->Get_ItemDesc().iPurchase);
	m_pGameInstance->Render_Text(L"Font_Malang", strCost, _vec2(vStartPos.x + 1.f, vStartPos.y), 0.4f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", strCost, _vec2(vStartPos.x, +vStartPos.y + 1.f), 0.4f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", strCost, _vec2(vStartPos.x, +vStartPos.y), 0.4f, _vec4(1.f, 1.f, 1.f, 1.f), 0.f, true);


	return S_OK;
}

const ITEM& CShopDesc::Get_ItemDesc() const
{
	return m_pItem->Get_ItemDesc();
}
void CShopDesc::Set_Position(_vec2 vPos)
{
	m_fX = vPos.x;
	m_fY = vPos.y;	
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	m_rcRect = {
	  (LONG)(m_fX - m_fSizeX * 0.5f),
	  (LONG)(m_fY - m_fSizeY * 0.5f),
	  (LONG)(m_fX + m_fSizeX * 0.5f),
	  (LONG)(m_fY + m_fSizeY * 0.5f)
	};

}

HRESULT CShopDesc::Add_Parts()
{
	
	CItem::ITEM_DESC ItemDesc = {};
	ItemDesc.fDepth = m_fDepth - 0.01f;
	ItemDesc.eItemDesc = CUI_Manager::Get_Instance()->Find_Item(m_strItemName);
	ItemDesc.iNum = 1;
	ItemDesc.vPosition = _vec2(m_fX - 100.f, m_fY);
	ItemDesc.vSize = _vec2(70.f, 70.f);
	ItemDesc.bCanInteract = false;

	m_pItem = (CItem*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Item"), &ItemDesc);

	if (not m_pItem)
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CShopDesc::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_ShopDesc"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CShopDesc::Bind_ShaderResources()
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

CShopDesc* CShopDesc::Create(_dev pDevice, _context pContext)
{
	CShopDesc* pInstance = new CShopDesc(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CItemBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CShopDesc::Clone(void* pArg)
{
	CShopDesc* pInstance = new CShopDesc(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CItemBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShopDesc::Free()
{
	__super::Free();

	Safe_Release(m_pItem);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
