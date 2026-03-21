#include "FadeBox.h"
#include "GameInstance.h"
#include "UI_Manager.h"

CFadeBox::CFadeBox(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CFadeBox::CFadeBox(const CFadeBox& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CFadeBox::Init_Prototype()
{
	return S_OK;
}

HRESULT CFadeBox::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;

	m_fX = g_iWinSizeX >> 1;
	m_fY = g_iWinSizeY >> 1;

	m_fDepth = (_float)D_FADE / (_float)D_END;

	m_Desc = *reinterpret_cast<FADE_DESC*>(pArg);

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	return S_OK;
}

void CFadeBox::Tick(_float fTimeDelta)
{
	//if (CUI_Manager::Get_Instance()->Get_TimeStop())
	{
		fTimeDelta /= m_pGameInstance->Get_TimeRatio();
	}

	if (m_Desc.fIn_Duration > 0.f)
	{
		m_fAlpha = Lerp(0.f, m_Desc.fMaxAlpha, m_fTime / m_Desc.fIn_Duration);
		if (m_fAlpha >= m_Desc.fMaxAlpha)
		{
			m_fAlpha = m_Desc.fMaxAlpha;
			m_Desc.fIn_Duration = {};
			m_fTime = {};
		}
	}
	else if (m_Desc.fMid_Duration > 0.f)
	{
		if (m_fTime > m_Desc.fMid_Duration)
		{
			m_Desc.fMid_Duration = {};
			m_fTime = {};
		}
	}
	else if (not m_Desc.isInfiniteLoop)
	{
		m_fAlpha = Lerp(0.f, m_Desc.fMaxAlpha, 1.f - m_fTime / m_Desc.fOut_Duration);
		if (m_fAlpha <= 0.f)
		{
			Kill();
		}
	}

	if (m_Desc.phasFadeCompleted)
	{
		if (m_fAlpha >= m_Desc.fMaxAlpha)
		{
			*m_Desc.phasFadeCompleted = true;
		}
	}

	m_fTime += fTimeDelta;
}

void CFadeBox::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CFadeBox::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_UI_Color_Alpha)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CFadeBox::Add_Components()
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

	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_FadeBox"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	//{
	//	return E_FAIL;
	//}

	return S_OK;
}

HRESULT CFadeBox::Bind_ShaderResources()
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

	//if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
	//{
	//	return E_FAIL;
	//}
	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof m_fAlpha)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_Desc.vColor, sizeof m_Desc.vColor)))
	{
		return E_FAIL;
	}

	return S_OK;
}

CFadeBox* CFadeBox::Create(_dev pDevice, _context pContext)
{
	CFadeBox* pInstance = new CFadeBox(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CFadeBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CFadeBox::Clone(void* pArg)
{
	CFadeBox* pInstance = new CFadeBox(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CFadeBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFadeBox::Free()
{
	__super::Free();

	//Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
