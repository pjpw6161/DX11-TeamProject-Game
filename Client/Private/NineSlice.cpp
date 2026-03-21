#include "NineSlice.h"
#include "GameInstance.h"

CNineSlice::CNineSlice(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CNineSlice::CNineSlice(const CNineSlice& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CNineSlice::Init_Prototype()
{
	return S_OK;
}

HRESULT CNineSlice::Init(void* pArg)
{

	m_fDepth = ((SLICE_DESC*)pArg)->fDepth;
	m_vSize = ((SLICE_DESC*)pArg)->vSize;
	m_vPosition = ((SLICE_DESC*)pArg)->vPosition;

	m_fFontSize = ((SLICE_DESC*)pArg)->fFontSize;
	m_vTextPosition = ((SLICE_DESC*)pArg)->vTextPosition;
	m_strText = ((SLICE_DESC*)pArg)->strText;
	m_vTextColor = ((SLICE_DESC*)pArg)->vTextColor;
	m_vTextBorderColor = ((SLICE_DESC*)pArg)->vTextBorderColor;
	m_eLevel = ((SLICE_DESC*)pArg)->eLevelID;

	m_strTexture = ((SLICE_DESC*)pArg)->strTexture;

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

void CNineSlice::Tick(_float fTimeDelta)
{

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
}

void CNineSlice::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);

}

HRESULT CNineSlice::Render()
{
	if (m_strTexture != TEXT(""))
	{
		if (FAILED(Render_Texture()))
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

HRESULT CNineSlice::Add_Components()
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

HRESULT CNineSlice::Bind_ShaderResources()
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
		_int2 vNumSprite = { 3, 3 };
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vNumSprite", &vNumSprite, sizeof(_int2))))
		{
			return E_FAIL;
		}
	}


	return S_OK;
}



void CNineSlice::Set_Position(_vec2 vPos)
{
	m_vPosition = vPos;
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

void CNineSlice::Set_Size(_float fSizeX, _float fSizeY, _float fFontSize)
{
	m_vSize = _vec2(fSizeX, fSizeY);
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

HRESULT CNineSlice::Render_Texture()
{
	_float2 vTexPos = m_vPosition;
	_float fSmallSpriteSize = 6.f;

	_uint iIndex = 4;

	m_fSizeX = m_vSize.x - fSmallSpriteSize * 2.f;
	m_fSizeY = m_vSize.y - fSmallSpriteSize * 2.f;
	m_fX = vTexPos.x;
	m_fY = vTexPos.y;
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &iIndex, sizeof(_int))))
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


	iIndex = 0;
	m_fX = vTexPos.x - m_vSize.x / 2.f - fSmallSpriteSize / 2.f;
	m_fY = vTexPos.y - m_vSize.y / 2.f - fSmallSpriteSize / 2.f;
	m_fSizeX = fSmallSpriteSize;
	m_fSizeY = fSmallSpriteSize;
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &iIndex, sizeof(_int))))
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

	iIndex = 2;
	m_fX = vTexPos.x + m_vSize.x / 2.f + fSmallSpriteSize / 2.f;
	m_fY = vTexPos.y - m_vSize.y / 2.f - fSmallSpriteSize / 2.f;
	m_fSizeX = fSmallSpriteSize;
	m_fSizeY = fSmallSpriteSize;
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &iIndex, sizeof(_int))))
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

	iIndex = 6;
	m_fX = vTexPos.x - m_vSize.x / 2.f - fSmallSpriteSize / 2.f;
	m_fY = vTexPos.y + m_vSize.y / 2.f + fSmallSpriteSize / 2.f;
	m_fSizeX = fSmallSpriteSize;
	m_fSizeY = fSmallSpriteSize;
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &iIndex, sizeof(_int))))
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

	iIndex = 8;

	m_fX = vTexPos.x + m_vSize.x / 2.f + fSmallSpriteSize / 2.f;
	m_fY = vTexPos.y + m_vSize.y / 2.f + fSmallSpriteSize / 2.f;
	m_fSizeX = fSmallSpriteSize;
	m_fSizeY = fSmallSpriteSize;
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &iIndex, sizeof(_int))))
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

	iIndex = 1;

	m_fX = vTexPos.x;
	m_fY = vTexPos.y - m_vSize.y / 2.f - fSmallSpriteSize / 2.f;
	m_fSizeX = m_vSize.x;
	m_fSizeY = fSmallSpriteSize;
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &iIndex, sizeof(_int))))
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

	iIndex = 7;

	m_fX = vTexPos.x;
	m_fY = vTexPos.y + m_vSize.y / 2.f + fSmallSpriteSize / 2.f;
	m_fSizeX = m_vSize.x;
	m_fSizeY = fSmallSpriteSize;
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &iIndex, sizeof(_int))))
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

	iIndex = 3;

	m_fX = vTexPos.x - m_vSize.x / 2.f - fSmallSpriteSize / 2.f;
	m_fY = vTexPos.y;
	m_fSizeX = fSmallSpriteSize;
	m_fSizeY = m_vSize.y;
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &iIndex, sizeof(_int))))
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

	iIndex = 5;

	m_fX = vTexPos.x + m_vSize.x / 2.f + fSmallSpriteSize / 2.f;
	m_fY = vTexPos.y;
	m_fSizeX = fSmallSpriteSize;
	m_fSizeY = m_vSize.y;
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &iIndex, sizeof(_int))))
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

	return S_OK;
}

CNineSlice* CNineSlice::Create(_dev pDevice, _context pContext)
{
	CNineSlice* pInstance = new CNineSlice(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CNineSlice");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNineSlice::Clone(void* pArg)
{
	CNineSlice* pInstance = new CNineSlice(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CNineSlice");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNineSlice::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
