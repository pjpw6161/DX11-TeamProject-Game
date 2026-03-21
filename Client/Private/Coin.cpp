#include "Coin.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "UI_Manager.h"

CCoin::CCoin(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CCoin::CCoin(const CCoin& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CCoin::Init_Prototype()
{
	return S_OK;
}

HRESULT CCoin::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = 25.f;
	m_fSizeY = 25.f;

	m_fX = 700.f;
	m_fY = 30.f;

	m_fDepth = (_float)D_SCREEN / (_float)D_END;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	


	return S_OK;
}

void CCoin::Tick(_float fTimeDelta)
{
	m_iCoin = CUI_Manager::Get_Instance()->Get_Coin();
}

void CCoin::Late_Tick(_float fTimeDelta)
{
	if (CUI_Manager::Get_Instance()->Showing_FullScreenUI() || CUI_Manager::Get_Instance()->Is_BossFight())
	{
		return;
	}

	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CCoin::Render()
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
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iCoin), _vec2(vPos.x - 0.2f, vPos.y), 0.4f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iCoin), _vec2(vPos.x + 0.2f, vPos.y), 0.4f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iCoin), _vec2(vPos.x, vPos.y + 0.2f), 0.4f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iCoin), _vec2(vPos.x, vPos.y - 0.2f), 0.4f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_iCoin), _vec2(vPos.x, vPos.y), 0.4f, _vec4(1.f, 1.f, 1.f, 1.f), 0.f, true);


	return S_OK;
}

HRESULT CCoin::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_coin"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CCoin::Bind_ShaderResources()
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

CCoin* CCoin::Create(_dev pDevice, _context pContext)
{
	CCoin* pInstance = new CCoin(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CCoin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCoin::Clone(void* pArg)
{
	CCoin* pInstance = new CCoin(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CCoin");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCoin::Free()
{
	__super::Free();


	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
