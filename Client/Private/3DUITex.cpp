#include "3DUITex.h"
#include "GameInstance.h"
#include "UI_Manager.h"

C3DUITex::C3DUITex(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

C3DUITex::C3DUITex(const C3DUITex& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT C3DUITex::Init_Prototype()
{
	return S_OK;
}

HRESULT C3DUITex::Init(void* pArg)
{



	m_eLevel = ((UITEX_DESC*)pArg)->eLevelID;
	m_strTexture = ((UITEX_DESC*)pArg)->strTexture;
	m_vPosition = ((UITEX_DESC*)pArg)->vPosition;
	m_pParentTransform = ((UITEX_DESC*)pArg)->pParentTransform;
	Safe_AddRef(m_pParentTransform);

	m_fSizeX = ((UITEX_DESC*)pArg)->vSize.x;
	m_fSizeY = ((UITEX_DESC*)pArg)->vSize.y;
	
	m_fFontSize = ((UITEX_DESC*)pArg)->fFontSize;
	m_vTextPosition = ((UITEX_DESC*)pArg)->vTextPosition;
	m_strText = ((UITEX_DESC*)pArg)->strText;
	m_vTextColor = ((UITEX_DESC*)pArg)->vTextColor;
	
	m_fDepth = (_float)D_NAMETAG / (_float)D_END + 0.1f;

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);


	return S_OK;
}

void C3DUITex::Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_State(State::Pos, m_pParentTransform->Get_State(State::Pos) + m_vPosition);	
	m_vTextPos = __super::Convert_To_2D(m_pTransformCom);
	m_fX = m_vTextPos.x;
	m_fY = m_vTextPos.y;
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);


	m_rcRect = {
		  (LONG)(m_vTextPos.x - m_fSizeX * 0.5f),
		  (LONG)(m_vTextPos.y - m_fSizeY * 0.5f),
		  (LONG)(m_vTextPos.x + m_fSizeX * 0.5f),
		  (LONG)(m_vTextPos.y + m_fSizeY * 0.5f)
	};
}

void C3DUITex::Late_Tick(_float fTimeDelta)
{
	/*
	
	if (CUI_Manager::Get_Instance()->Showing_FullScreenUI())
	{
		return;
	}*/
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT C3DUITex::Render()
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
		m_pGameInstance->Render_Text(L"Font_Malang", m_strText, _vec2(m_vTextPos.x + m_vTextPosition.x + 1.f, m_vTextPos.y + m_vTextPosition.y), m_fFontSize, _vec4(0.f, 0.f, 0.f, 1.f));
		m_pGameInstance->Render_Text(L"Font_Malang", m_strText, _vec2(m_vTextPos.x + m_vTextPosition.x, m_vTextPos.y + m_vTextPosition.y), m_fFontSize, _vec4(0.f, 0.f, 0.f, 1.f));
		m_pGameInstance->Render_Text(L"Font_Malang", m_strText, _vec2(m_vTextPos.x + m_vTextPosition.x, m_vTextPos.y + m_vTextPosition.y + 1.f), m_fFontSize, _vec4(0.f, 0.f, 0.f, 1.f));
		m_pGameInstance->Render_Text(L"Font_Malang", m_strText, _vec2(m_vTextPos.x + m_vTextPosition.x, m_vTextPos.y + m_vTextPosition.y/*  - 0.5f*/), m_fFontSize, _vec4(0.f, 0.f, 0.f, 1.f));
		m_pGameInstance->Render_Text(L"Font_Malang", m_strText, _vec2(m_vTextPos.x + m_vTextPosition.x, m_vTextPos.y + m_vTextPosition.y), m_fFontSize, m_vTextColor);

	}

	return S_OK;
}

void C3DUITex::Set_Size(_float fSizeX, _float fSizeY)
{
	m_fSizeX = fSizeX;
	m_fSizeY = fSizeY;
}

void C3DUITex::Set_Position(_vec3 vPos)
{
	m_vPosition = vPos;
}

HRESULT C3DUITex::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_strTexture, TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT C3DUITex::Bind_ShaderResources()
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

	if (m_ePass != VTPass_UI)
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
		{
			return E_FAIL;
		}
		_vec4 vColor = _vec4(1.f, 0.5f, 0.f, 1.f);

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &vColor, sizeof _vec4)))
		{
			return E_FAIL;
		}
	}
	else
	{
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		{
			return E_FAIL;
		}
	}
	return S_OK;
}

C3DUITex* C3DUITex::Create(_dev pDevice, _context pContext)
{
	C3DUITex* pInstance = new C3DUITex(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : C3DUITex");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* C3DUITex::Clone(void* pArg)
{
	C3DUITex* pInstance = new C3DUITex(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : C3DUITex");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void C3DUITex::Free()
{
	__super::Free();

	Safe_Release(m_pParentTransform);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
