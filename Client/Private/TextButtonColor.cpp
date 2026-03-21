#include "TextButtonColor.h"
#include "GameInstance.h"
#include "UI_Manager.h"

CTextButtonColor::CTextButtonColor(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CTextButtonColor::CTextButtonColor(const CTextButtonColor& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CTextButtonColor::Init_Prototype()
{
	return S_OK;
}

HRESULT CTextButtonColor::Init(void* pArg)
{


	m_fDepth = ((TEXTBUTTON_DESC*)pArg)->fDepth;
	m_fAlpha = ((TEXTBUTTON_DESC*)pArg)->fAlpha;

	m_vSize = ((TEXTBUTTON_DESC*)pArg)->vSize;
	m_vPosition = ((TEXTBUTTON_DESC*)pArg)->vPosition;
	m_vColor = ((TEXTBUTTON_DESC*)pArg)->vColor;

	m_fFontSize = ((TEXTBUTTON_DESC*)pArg)->fFontSize;
	m_vTextPosition = ((TEXTBUTTON_DESC*)pArg)->vTextPosition;
	m_strText = ((TEXTBUTTON_DESC*)pArg)->strText;
	m_vTextColor = ((TEXTBUTTON_DESC*)pArg)->vTextColor;
	m_eLevel = ((TEXTBUTTON_DESC*)pArg)->eLevelID;

	m_strTexture = ((TEXTBUTTON_DESC*)pArg)->strTexture;
	m_strTexture2= ((TEXTBUTTON_DESC*)pArg)->strTexture2;


	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}


	m_fSizeX = m_vSize.x;
	m_fSizeY = m_vSize.y;

	m_fX = m_vPosition.x;
	m_fY = m_vPosition.y;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	m_rcRect = {
		  (LONG)(m_vPosition.x - m_fSizeX * 0.5f),
		  (LONG)(m_vPosition.y - m_fSizeY * 0.5f),
		  (LONG)(m_vPosition.x + m_fSizeX * 0.5f),
		  (LONG)(m_vPosition.y + m_fSizeY * 0.5f)
	};
	m_rcInitialRect = m_rcRect;
	return S_OK;
}

void CTextButtonColor::Tick(_float fTimeDelta)
{
	if (CUI_Manager::Get_Instance()->Get_TimeStop())
	{
		fTimeDelta /= m_pGameInstance->Get_TimeRatio();
	}

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
}

void CTextButtonColor::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);

}

HRESULT CTextButtonColor::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}


	if (FAILED(m_pShaderCom->Begin(m_ePass)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}
	if (m_strText != TEXT(""))
	{
		m_vTextColor.w = m_fAlpha;
		m_pGameInstance->Render_Text(L"Font_Malang", m_strText, _vec2(m_fX + m_vTextPosition.x + 1.f, m_fY + m_vTextPosition.y), m_fFontSize, _vec4(0.f, 0.f, 0.f, 1.f));
		m_pGameInstance->Render_Text(L"Font_Malang", m_strText, _vec2(m_fX + m_vTextPosition.x, m_fY + m_vTextPosition.y), m_fFontSize, _vec4(0.f, 0.f, 0.f, 1.f));
		m_pGameInstance->Render_Text(L"Font_Malang", m_strText, _vec2(m_fX + m_vTextPosition.x, m_fY + m_vTextPosition.y + 1.f), m_fFontSize, _vec4(0.f, 0.f, 0.f, 1.f));
		m_pGameInstance->Render_Text(L"Font_Malang", m_strText, _vec2(m_fX + m_vTextPosition.x, m_fY + m_vTextPosition.y/*  - 0.5f*/), m_fFontSize, _vec4(0.f, 0.f, 0.f, 1.f));
		m_pGameInstance->Render_Text(L"Font_Malang", m_strText, _vec2(m_fX + m_vTextPosition.x, m_fY + m_vTextPosition.y), m_fFontSize, m_vTextColor);

	}

	return S_OK;
}

HRESULT CTextButtonColor::Add_Components()
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

	if (m_strTexture != TEXT(""))
	{
		if (FAILED(__super::Add_Component(m_eLevel, m_strTexture, TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		{
			return E_FAIL;
		}
	}
	if (m_strTexture2 != TEXT(""))
	{
		if (FAILED(__super::Add_Component(m_eLevel, m_strTexture2, TEXT("Com_Texture1"), reinterpret_cast<CComponent**>(&m_pMaskTextureCom))))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CTextButtonColor::Bind_ShaderResources()
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

	if (m_ePass == VTPass_Mask_Color)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_vec4))))
		{
			return E_FAIL;
		}
		if (m_strTexture != TEXT(""))
		{
			if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
			{
				return E_FAIL;
			}
		}
	}	
	else if (m_ePass == VTPass_Move)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fx", &m_fMoveX, sizeof(_float))))
		{
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fy", &m_fMoveY, sizeof(_float))))
		{
			return E_FAIL;
		}

		if (m_strTexture != TEXT(""))
		{
			if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
			{
				return E_FAIL;
			}
		}
	}
	if (m_ePass == VTPass_UI_Alpha)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		{
			return E_FAIL;
		}
		if (m_strTexture != TEXT(""))
		{
			if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
			{
				return E_FAIL;
			}
		}
	}
	else if (m_ePass == VTPass_UI_Color_Alpha)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		{
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_vec4))))
		{
			return E_FAIL;
		}
		if (m_strTexture != TEXT(""))
		{
			if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
			{
				return E_FAIL;
			}
		}
	}
	else  if (m_ePass == VTPass_Mask_ColorAlpha)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_vec4))))
		{
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		{
			return E_FAIL;
		}
		if (m_strTexture != TEXT(""))
		{
			if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
			{
				return E_FAIL;
			}
		}
	}
	else  if (m_ePass == VTPass_Mask_Texture || m_ePass == VTPass_Inv_Mask_Texture)
	{
		/*
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		{
			return E_FAIL;
		}
		*/
		if (m_strTexture != TEXT(""))
		{
			if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
			{
				return E_FAIL;
			}
		}
		if (m_strTexture2 != TEXT(""))
		{
			if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
			{
				return E_FAIL;
			}
		}
	}
	else  if (m_ePass == VTPass_Bright)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fx", &m_fMoveX, sizeof(_float))))
		{
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fy", &m_fMoveY, sizeof(_float))))
		{
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fBrightFactor", &m_fFactor, sizeof(_float))))
		{
			return E_FAIL;
		}
		if (m_strTexture != TEXT(""))
		{
			if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
			{
				return E_FAIL;
			}
		}
		if (m_strTexture2 != TEXT(""))
		{
			if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
			{
				return E_FAIL;
			}
		}
	}
	else  if (m_ePass == VTPass_MaskColorMove)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fx", &m_fMoveX, sizeof(_float))))
		{
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fy", &m_fMoveY, sizeof(_float))))
		{
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_vec4))))
		{
			return E_FAIL;
		}
		if (m_strTexture != TEXT(""))
		{
			if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
			{
				return E_FAIL;
			}
		}
	}
	else  if (m_ePass == VTPass_Background_Mask)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fx", &m_fMoveX, sizeof(_float))))
		{
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fy", &m_fMoveY, sizeof(_float))))
		{
			return E_FAIL;
		}
		if (m_strTexture != TEXT(""))
		{
			if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
			{
				return E_FAIL;
			}
		}
	}
	else if (m_ePass == VTPass_HPNoMask)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fHpRatio", &m_fAlpha, sizeof(_float))))
		{
			return E_FAIL;
		}
		if (m_strTexture != TEXT(""))
		{
			if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
			{
				return E_FAIL;
			}
		}
	}
	else if (m_ePass == VTPass_SpriteMaskTexture)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vNumSprite", &m_vNumSprite, sizeof(_int2))))
		{
			return E_FAIL;
		}
		int iIndex = static_cast<_int>(m_fIndex);
		if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &iIndex, sizeof(_int))))
		{
			return E_FAIL;
		}

		if (m_strTexture != TEXT(""))
		{
			if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
			{
				return E_FAIL;
			}
		}
		if (m_strTexture2 != TEXT(""))
		{
			if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
			{
				return E_FAIL;
			}
		}
	}
	else if (m_ePass == VTPass_SpriteMaskColor)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vNumSprite", &m_vNumSprite, sizeof(_int2))))
		{
			return E_FAIL;
		}
		int iIndex = static_cast<_int>(m_fIndex);
		if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &iIndex, sizeof(_int))))
		{
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &iIndex, sizeof(_int))))
		{
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_vec4))))
		{
			return E_FAIL;
		}
		if (m_strTexture != TEXT(""))
		{
			if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
			{
				return E_FAIL;
			}
		}
	}
	else if (m_ePass == VTPass_SpriteMaskColor_Dissolve)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vNumSprite", &m_vNumSprite, sizeof(_int2))))
		{
			return E_FAIL;
		}
		int iIndex = static_cast<_int>(m_fIndex);
		if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &iIndex, sizeof(_int))))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_vec4))))
		{
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveRatio", &m_fFactor, sizeof(_float))))
		{
			return E_FAIL;
		}
		if (m_strTexture != TEXT(""))
		{
			if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
			{
				return E_FAIL;
			}
		}
		if (m_strTexture2 != TEXT(""))
		{
			if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture")))
			{
				return E_FAIL;
			}
		}
	}
	else
	{
		if (m_strTexture != TEXT(""))
		{
			if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
			{
				return E_FAIL;
			}
		}
	}

	return S_OK;
}

void CTextButtonColor::Set_Size(_float fSizeX, _float fSizeY, _float fFontSize)
{
	m_fSizeX = fSizeX; 
	m_fSizeY = fSizeY; 
	if (m_fFontSize != -1.f)
		m_fFontSize = fFontSize;
	m_rcRect = {
  (LONG)(m_fX - m_fSizeX * 0.5f),
  (LONG)(m_fY - m_fSizeY * 0.5f),
  (LONG)(m_fX + m_fSizeX * 0.5f),
  (LONG)(m_fY + m_fSizeY * 0.5f)
	};
}

void CTextButtonColor::Set_Position(_vec2 vPos)
{
	m_fX = vPos.x;
	m_fY = vPos.y;
	m_rcRect = {
  (LONG)(m_fX - m_fSizeX * 0.5f),
  (LONG)(m_fY - m_fSizeY * 0.5f),
  (LONG)(m_fX + m_fSizeX * 0.5f),
  (LONG)(m_fY + m_fSizeY * 0.5f)
	};
}

void CTextButtonColor::Rotate_Button(_vec4 vAxis, _float fAngle)
{
	m_pTransformCom->Rotation(vAxis, fAngle);
}

CTextButtonColor* CTextButtonColor::Create(_dev pDevice, _context pContext)
{
	CTextButtonColor* pInstance = new CTextButtonColor(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CTextButtonColor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTextButtonColor::Clone(void* pArg)
{
	CTextButtonColor* pInstance = new CTextButtonColor(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CTextButtonColor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTextButtonColor::Free()
{
	__super::Free();

	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
