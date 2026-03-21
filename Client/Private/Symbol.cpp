#include "Symbol.h"
#include "GameInstance.h"
#include "UI_Manager.h"

CSymbol::CSymbol(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CSymbol::CSymbol(const CSymbol& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CSymbol::Init_Prototype()
{
	return S_OK;
}

HRESULT CSymbol::Init(void* pArg)
{



	m_fX = g_iWinSizeX >> 1;
	m_fY = (g_iWinSizeY >> 1) - 100.f;

	m_fDepth = (_float)D_SCREEN / (_float)D_END;


	m_eType = ((SYMBOL_DESC*)pArg)->eType;



	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	return S_OK;
}

void CSymbol::Tick(_float fTimeDelta)
{
	m_fY = (g_iWinSizeY >> 1) - 200.f;
	if (CUI_Manager::Get_Instance()->Get_TimeStop())
	{
		fTimeDelta /= m_pGameInstance->Get_TimeRatio();
	}

	m_fTime += fTimeDelta;

	if (!m_bDissapear)
	{
		m_fAlpha += fTimeDelta * 2.f * m_fDir;
	}

	if (m_fAlpha >= 1.f)
	{
		m_bDissapear = true;
		if (m_fTime >= 2.f)
		{
			m_bDissapear = false;
			m_fDir = -1.f;
		}

	}
	if (m_fAlpha < 0.f)
	{
		m_isDead = true;
	}

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
}

void CSymbol::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CSymbol::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_UI_Alpha)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}



	return S_OK;
}

HRESULT CSymbol::Add_Components()
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
	wstring strTexture{};
	switch (m_eType)
	{
	case VILLAGE:
		strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Symbol0");	
		m_fSizeX = 514.f;
		m_fSizeY = 64.f;
		break;
	case FIELDEAST:
		strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Symbol1");
		m_fSizeX = 514.f;
		m_fSizeY = 64.f;
		break;
	case FIELDSOUTH:
		strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Symbol2");
		m_fSizeX = 514.f;
		m_fSizeY = 64.f;
		break;
	case GROAR:
		strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_ImageText_Chapter1_01");
		m_fSizeX = 256.f;
		m_fSizeY = 128.f;
		break;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, strTexture, TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSymbol::Bind_ShaderResources()
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
	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
	{
		return E_FAIL;
	}

	return S_OK;
}

CSymbol* CSymbol::Create(_dev pDevice, _context pContext)
{
	CSymbol* pInstance = new CSymbol(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CSymbol");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSymbol::Clone(void* pArg)
{
	CSymbol* pInstance = new CSymbol(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSymbol");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSymbol::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
