#include "Item.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "TextButtonColor.h"
#include "ItemInfo.h"

CItem::CItem(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CItem::CItem(const CItem& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CItem::Init_Prototype()
{
	return S_OK;
}

HRESULT CItem::Init(void* pArg)
{

	m_eItemDesc = ((ITEM_DESC*)pArg)->eItemDesc;
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_isScreen = ((ITEM_DESC*)pArg)->isScreen;
	m_bCanInteract = ((ITEM_DESC*)pArg)->bCanInteract;
	m_haveBG = ((ITEM_DESC*)pArg)->haveBG;

	m_iNum = ((ITEM_DESC*)pArg)->iNum;

	m_fSizeX = ((ITEM_DESC*)pArg)->vSize.x;
	m_fSizeY = ((ITEM_DESC*)pArg)->vSize.y;

	m_fX = ((ITEM_DESC*)pArg)->vPosition.x;
	m_fY = ((ITEM_DESC*)pArg)->vPosition.y;

	m_fDepth = ((ITEM_DESC*)pArg)->fDepth;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	

	m_rcRect = {
		  (LONG)(m_fX - m_fSizeX * 0.5f),
		  (LONG)(m_fY - m_fSizeY * 0.5f),
		  (LONG)(m_fX + m_fSizeX * 0.5f),
		  (LONG)(m_fY + m_fSizeY * 0.5f)
	};

	if (m_haveBG)
	{
		CTextButtonColor::TEXTBUTTON_DESC TextButton = {};
		TextButton.eLevelID = LEVEL_STATIC;
		TextButton.fDepth = m_fDepth + 0.005f;
		TextButton.fAlpha = 0.8f;
		TextButton.strText = TEXT("");
		TextButton.vPosition = _vec2(m_fX, m_fY);
		TextButton.vSize = _vec2(m_fSizeX, m_fSizeY);
		TextButton.vTextPosition = _vec2(0.f, 0.f);
		ITEM_TIER eTier = (ITEM_TIER)m_eItemDesc.iItemTier;
		switch (eTier)
		{
		case TIER_COMMON:
			TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_invenslot");
			break;
		case TIER_UNCOMMON:
			TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_invenslot1");
			break;
		case TIER_RARE:
			TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_invenslot2");
			break;
		case TIER_UNIQUE:
			TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_invenslot3");
			break;
		case TIER_LEGENDARY:
			TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_invenslot4");
			break;

		}

		m_pBackGround = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);
		if (not m_pBackGround)
		{
			return E_FAIL;
		}
		m_pBackGround->Set_Pass(VTPass_UI);
	}

	CTextButton::TEXTBUTTON_DESC ButtonDesc = {};
	ButtonDesc.eLevelID = LEVEL_STATIC;
	ButtonDesc.fDepth = m_fDepth - 0.01f;
	ButtonDesc.fFontSize = 0.45f;
	ButtonDesc.strText = TEXT("");
	ButtonDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Border");
	ButtonDesc.vPosition = _vec2(-50.f, -50.f);
	ButtonDesc.vSize = _vec2(m_fSizeX, m_fSizeY);
	ButtonDesc.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	ButtonDesc.vTextPosition = _vec2(20.f, 12.f);
	m_pBorder = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &ButtonDesc);
	if (not m_pBorder)
	{
		return E_FAIL;
	}
	return S_OK;
}

void CItem::Tick(_float fTimeDelta)
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

	if (m_bBorder)
	{
		if (m_pBorder != nullptr)
		{
			m_pBorder->Set_Position(_vec2(m_fX, m_fY));
			m_pBorder->Tick(fTimeDelta);
		}
	}
	if (m_pBackGround != nullptr)
	{
		m_pBackGround->Set_Position(_vec2(m_fX, m_fY));
		m_pBackGround->Tick(fTimeDelta);
	}
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
}

void CItem::Late_Tick(_float fTimeDelta)
{
	if (m_bBorder)
	{
		if (m_pBorder != nullptr)
		{
			m_pBorder->Late_Tick(fTimeDelta);
		}
	}
	if (m_pBackGround != nullptr)
	{
		m_pBackGround->Late_Tick(fTimeDelta);
	}
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);

}

HRESULT CItem::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_Sprite)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}
	if (m_iNum > 1)
	{
		_vec2 vStartPos = _vec2(m_fX + 14.f, m_fY + 20.f);
		if (m_isScreen)
		{
			vStartPos.x -= 5.f;
			vStartPos.y -= 8.f;
		}
		m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iNum), _vec2(vStartPos.x + 1.f, vStartPos.y), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f));
		m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iNum), _vec2(vStartPos.x, +vStartPos.y + 1.f), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f));
		m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iNum), _vec2(vStartPos.x, +vStartPos.y), 0.3f, _vec4(1.f, 1.f, 1.f, 1.f));
	}
	return S_OK;
}

void CItem::Set_Position(_vec2 vPos)
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

HRESULT CItem::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_eItemDesc.strTexture, TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CItem::Bind_ShaderResources()
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

	_int2 vNumSprite = { 8, 8 };
	if (FAILED(m_pShaderCom->Bind_RawValue("g_vNumSprite", &vNumSprite, sizeof(_int2))))
	{
		return E_FAIL;
	}
	_uint iIndex = m_eItemDesc.iIndex;
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &iIndex, sizeof(_int))))
	{
		return E_FAIL;
	}
	return S_OK;
}

CItem* CItem::Create(_dev pDevice, _context pContext)
{
	CItem* pInstance = new CItem(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItem::Clone(void* pArg)
{
	CItem* pInstance = new CItem(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CItem");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItem::Free()
{
	__super::Free();

	Safe_Release(m_pBorder);
	Safe_Release(m_pBackGround);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
