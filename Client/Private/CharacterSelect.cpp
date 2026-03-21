#include "CharacterSelect.h"
#include "GameInstance.h"

CCharacterSelect::CCharacterSelect(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CCharacterSelect::CCharacterSelect(const CCharacterSelect& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CCharacterSelect::Init_Prototype()
{
	return S_OK;
}

HRESULT CCharacterSelect::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = 280.f;
	m_fSizeY = 20.f;

	m_fX = g_iWinSizeX >> 1;
	m_fY = 80.f;

	m_fDepth = 0.5f;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	return S_OK;
}

void CCharacterSelect::Tick(_float fTimeDelta)
{
	if (m_isActive)
	{
		switch (m_eState)
		{
		case NONALPHA:
		{
			m_fAlpha += fTimeDelta * 2.f;
			if (m_fAlpha >= 1.f)
			{
				m_fAlpha = 1.f;
				m_isActive = false;
			}
		}
		break;
		case ALPHA:
		{
			m_fAlpha -= fTimeDelta * 2.f;
			if (m_fAlpha <= 0.f)
			{
				m_fAlpha = 0.f;
				m_isActive = false;
			}
		}
		break;
		default:
			break;
		}
	}
}

void CCharacterSelect::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CCharacterSelect::Render()
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

HRESULT CCharacterSelect::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_SELECT, TEXT("Prototype_Component_Texture_UI_Select_Bg_PlayerSelectTitle"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCharacterSelect::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_ViewMatrix))
		|| FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
	{
		return E_FAIL;
	}
	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
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

void CCharacterSelect::Set_Active_Alpha(STATE eState)
{
	if (eState == NONALPHA)
	{
		m_fAlpha = 0.f;
		m_isActive = true;
	}
	else if (eState == ALPHA)
	{
		m_fAlpha = 1.f;
		m_isActive = true;
	}
	m_eState = eState;
}

CCharacterSelect* CCharacterSelect::Create(_dev pDevice, _context pContext)
{
	CCharacterSelect* pInstance = new CCharacterSelect(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CCharacterSelect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCharacterSelect::Clone(void* pArg)
{
	CCharacterSelect* pInstance = new CCharacterSelect(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CCharacterSelect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCharacterSelect::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
