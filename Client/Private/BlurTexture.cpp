#include "BlurTexture.h"
#include "GameInstance.h"

CBlurTexture::CBlurTexture(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CBlurTexture::CBlurTexture(const CBlurTexture& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CBlurTexture::Init_Prototype()
{
	return S_OK;
}

HRESULT CBlurTexture::Init(void* pArg)
{

	m_fDepth = ((BLURTEX_DESC*)pArg)->fDepth;
	m_vSize = ((BLURTEX_DESC*)pArg)->vSize;
	m_vPosition = ((BLURTEX_DESC*)pArg)->vPosition;
	m_vColor = ((BLURTEX_DESC*)pArg)->vColor;

	m_fFontSize = ((BLURTEX_DESC*)pArg)->fFontSize;
	m_vTextPosition = ((BLURTEX_DESC*)pArg)->vTextPosition;
	m_strText = ((BLURTEX_DESC*)pArg)->strText;
	m_vTextColor = ((BLURTEX_DESC*)pArg)->vTextColor;
	m_eLevel = ((BLURTEX_DESC*)pArg)->eLevelID;

	m_strTexture = ((BLURTEX_DESC*)pArg)->strTexture;

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

	return S_OK;
}

void CBlurTexture::Tick(_float fTimeDelta)
{

}

void CBlurTexture::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CBlurTexture::Render()
{
	if (m_strTexture != TEXT(""))
	{
		if (FAILED(Bind_ShaderResources()))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(VTPass_BLUR)))
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
		m_pGameInstance->Render_Text(L"Font_UI", m_strText, _vec2(m_vPosition.x + m_vTextPosition.x + 0.1f, m_vPosition.y + m_vTextPosition.y), m_fFontSize, m_vTextColor);
		m_pGameInstance->Render_Text(L"Font_UI", m_strText, _vec2(m_vPosition.x + m_vTextPosition.x - 0.1f, m_vPosition.y + m_vTextPosition.y), m_fFontSize, m_vTextColor);
		m_pGameInstance->Render_Text(L"Font_UI", m_strText, _vec2(m_vPosition.x + m_vTextPosition.x, m_vPosition.y + m_vTextPosition.y + 0.1f), m_fFontSize, m_vTextColor);
		m_pGameInstance->Render_Text(L"Font_UI", m_strText, _vec2(m_vPosition.x + m_vTextPosition.x, m_vPosition.y + m_vTextPosition.y - 0.1f), m_fFontSize, m_vTextColor);
		m_pGameInstance->Render_Text(L"Font_UI", m_strText, _vec2(m_vPosition.x + m_vTextPosition.x, m_vPosition.y + m_vTextPosition.y), m_fFontSize, m_vTextColor);
	}

	return S_OK;
}

HRESULT CBlurTexture::Add_Components()
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

HRESULT CBlurTexture::Bind_ShaderResources()
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
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_vec4))))
		{
			return E_FAIL;
		}		
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fAmount", &m_fAmount, sizeof(_float))))
		{
			return E_FAIL;
		}
	}


	return S_OK;
}

void CBlurTexture::Set_Position(_vec2 vPos)
{
	m_fX = vPos.x;
	m_fY = vPos.y;
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
}

void CBlurTexture::Set_Size(_float fSizeX, _float fSizeY, _float fFontSize)
{
	m_fSizeX = fSizeX;
	m_fSizeY = fSizeY;
	if (m_fFontSize != -1.f)
		m_fFontSize = fFontSize;
}

CBlurTexture* CBlurTexture::Create(_dev pDevice, _context pContext)
{
	CBlurTexture* pInstance = new CBlurTexture(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CBlurTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBlurTexture::Clone(void* pArg)
{
	CBlurTexture* pInstance = new CBlurTexture(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CBlurTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlurTexture::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
