#include "Diamond.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "UI_Manager.h"

CDiamond::CDiamond(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CDiamond::CDiamond(const CDiamond& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CDiamond::Init_Prototype()
{
	return S_OK;
}

HRESULT CDiamond::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}
	m_fSizeX = 30.f;
	m_fSizeY = 30.f;

	m_fX = 580.f;
	m_fY = 30.f;

	m_fDepth = (_float)D_SCREEN / (_float)D_END;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);


	return S_OK;
}

void CDiamond::Tick(_float fTimeDelta)
{
	m_iDiamond = CUI_Manager::Get_Instance()->Get_Diamond();
}

void CDiamond::Late_Tick(_float fTimeDelta)
{
	if (CUI_Manager::Get_Instance()->Showing_FullScreenUI() || CUI_Manager::Get_Instance()->Is_BossFight())
	{
		return;
	}

	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CDiamond::Render()
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

	_vec2 vPos = _vec2(m_fX + 20.f, m_fY - 10.f);
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iDiamond), _vec2(vPos.x - 0.2f, vPos.y), 0.4f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iDiamond), _vec2(vPos.x + 0.2f, vPos.y), 0.4f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iDiamond), _vec2(vPos.x, vPos.y + 0.2f), 0.4f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iDiamond), _vec2(vPos.x, vPos.y - 0.2f), 0.4f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iDiamond), _vec2(vPos.x, vPos.y), 0.4f, _vec4(1.f, 1.f, 1.f, 1.f), 0.f, true);


	return S_OK;
}

HRESULT CDiamond::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_Diamond"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDiamond::Bind_ShaderResources()
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

CDiamond* CDiamond::Create(_dev pDevice, _context pContext)
{
	CDiamond* pInstance = new CDiamond(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CDiamond");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDiamond::Clone(void* pArg)
{
	CDiamond* pInstance = new CDiamond(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CDiamond");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDiamond::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
