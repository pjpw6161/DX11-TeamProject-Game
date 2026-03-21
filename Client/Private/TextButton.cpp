#include "TextButton.h"
#include "GameInstance.h"

CTextButton::CTextButton(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CTextButton::CTextButton(const CTextButton& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CTextButton::Init_Prototype()
{
	return S_OK;
}

HRESULT CTextButton::Init(void* pArg)
{

	m_fDepth = ((TEXTBUTTON_DESC*)pArg)->fDepth;
	m_vSize = ((TEXTBUTTON_DESC*)pArg)->vSize;
	m_vPosition = ((TEXTBUTTON_DESC*)pArg)->vPosition;

	m_fFontSize = ((TEXTBUTTON_DESC*)pArg)->fFontSize;
	m_vTextPosition = ((TEXTBUTTON_DESC*)pArg)->vTextPosition;
	m_strText = ((TEXTBUTTON_DESC*)pArg)->strText;
	m_vTextColor = ((TEXTBUTTON_DESC*)pArg)->vTextColor;
	m_vTextBorderColor = ((TEXTBUTTON_DESC*)pArg)->vTextBorderColor;
	m_eLevel = ((TEXTBUTTON_DESC*)pArg)->eLevelID;

	m_strTexture = ((TEXTBUTTON_DESC*)pArg)->strTexture;

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

void CTextButton::Tick(_float fTimeDelta)
{

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
}

void CTextButton::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);

}

HRESULT CTextButton::Render()
{
	if (m_strTexture != TEXT(""))
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
	}
	

	if (m_strText != TEXT(""))
	{
		m_pGameInstance->Render_Text(L"Font_Malang", m_strText, _vec2(m_fX + m_vTextPosition.x + 1.f, m_fY + m_vTextPosition.y), m_fFontSize, m_vTextBorderColor);
		m_pGameInstance->Render_Text(L"Font_Malang", m_strText, _vec2(m_fX + m_vTextPosition.x, m_fY + m_vTextPosition.y), m_fFontSize, m_vTextBorderColor);
		m_pGameInstance->Render_Text(L"Font_Malang", m_strText, _vec2(m_fX + m_vTextPosition.x, m_fY + m_vTextPosition.y + 1.f), m_fFontSize, m_vTextBorderColor);
		m_pGameInstance->Render_Text(L"Font_Malang", m_strText, _vec2(m_fX + m_vTextPosition.x, m_fY + m_vTextPosition.y), m_fFontSize, m_vTextBorderColor);
		m_pGameInstance->Render_Text(L"Font_Malang", m_strText, _vec2(m_fX + m_vTextPosition.x, m_fY + m_vTextPosition.y), m_fFontSize, m_vTextColor);
	}

	return S_OK;
}

HRESULT CTextButton::Add_Components()
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

	return S_OK;
}

HRESULT CTextButton::Bind_ShaderResources()
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
	if (m_strTexture != TEXT(""))
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		{
			return E_FAIL;
		}
	}


	return S_OK;
}



void CTextButton::Set_Position(_vec2 vPos)
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

void CTextButton::Set_Size(_float fSizeX, _float fSizeY, _float fFontSize)
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

CTextButton* CTextButton::Create(_dev pDevice, _context pContext)
{
	CTextButton* pInstance = new CTextButton(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CTextButton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTextButton::Clone(void* pArg)
{
	CTextButton* pInstance = new CTextButton(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CTextButton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTextButton::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
