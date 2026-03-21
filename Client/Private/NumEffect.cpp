#include "NumEffect.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "3DUITex.h"

CNumEffect::CNumEffect(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CNumEffect::CNumEffect(const CNumEffect& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CNumEffect::Init_Prototype()
{
	return S_OK;
}

HRESULT CNumEffect::Init(void* pArg)
{

	m_bOrth = ((NUMEFFECT_DESC*)pArg)->bOrth;
	m_fDepth = (_float)D_NAMETAG / (_float)D_END;

	m_iDamage = ((NUMEFFECT_DESC*)pArg)->iDamage;
	m_vTextPosition = ((NUMEFFECT_DESC*)pArg)->vTextPosition;
	//m_vColor = ((NAMETAG_DESC*)pArg)->vColor;HITEFFECT_DESC
	m_pParentTransform = ((NUMEFFECT_DESC*)pArg)->pParentTransform;
	if (!m_bOrth)
	{
		Safe_AddRef(m_pParentTransform);
	}


	m_fSizeX = 300.f;
	m_fSizeY = 300.f;
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}	

	if (!m_bOrth)
	{
		C3DUITex::UITEX_DESC TexDesc = {};
		TexDesc.eLevelID = LEVEL_STATIC;
		TexDesc.pParentTransform = m_pParentTransform;
		TexDesc.strTexture = TEXT("Prototype_Component_Texture_Effect_FX_A_Shine003_Tex");
		TexDesc.vPosition = _vec3(m_vTextPosition.x, m_vTextPosition.y, -0.01f);
		TexDesc.vSize = _vec2(40.f, 40.f);

		m_pEffect = (C3DUITex*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_3DUITex"), &TexDesc);
		if (not m_pEffect)
		{
			return E_FAIL;
		}
		m_pEffect->Set_Pass(VTPass_Mask_Color);

	}


	if (m_iDamage >= 5.f)
	{
		m_vColor = _vec4(0.94f, 0.34f, 0.1f, 1.f);
	}
	else if (m_iDamage >= 10.f)
	{
		m_vColor = _vec4(1.f, 0.8f, 0.f, 1.f);
	}
	else
	{
		m_vColor.w = 0.f;
	}
	
	return S_OK;
}

void CNumEffect::Tick(_float fTimeDelta)
{
	if (m_fSizeX <= 100.f)
	{
		m_fTime += fTimeDelta;
		if (m_fTime >= 0.2f && m_fAlpha <= 0.f)
		{
			m_isDead = true;
		}
		if (m_fTime >= 0.5f)
		{

			m_fAlpha -= 2.f * fTimeDelta;
			m_vTextPosition.y += 2.f * fTimeDelta;
		}
		else
		{
			m_vTextPosition.y += 0.2f * fTimeDelta;
		}
	}
	else
	{
		m_fSizeX -= 300.f * fTimeDelta;
		m_fSizeY -= 300.f * fTimeDelta;
		m_fAlpha = Lerp(0.f, 1.f, (300.f - m_fSizeX) / 300.f);
	}

	if (!m_bOrth)
	{
		if (m_pEffect->Get_Size().y >= 140.f)
		{
			m_isEffect = false;
		}
		if (m_fAlpha >= 0.2f && m_isEffect)
		{
			m_pEffect->Set_Size(m_pEffect->Get_Size().x + 12.f, m_pEffect->Get_Size().y + 8.f);
		}
		m_pEffect->Tick(fTimeDelta);
	}

}

void CNumEffect::Late_Tick(_float fTimeDelta)
{
	if (!m_bOrth)
	{
		if (m_fAlpha >= 0.2f && m_isEffect)
		{
			m_pEffect->Late_Tick(fTimeDelta);
		}
	}

	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CNumEffect::Render()
{
	if (!m_bOrth)
	{
		m_pTransformCom->Set_State(State::Pos, m_pParentTransform->Get_State(State::Pos) + m_vTextPosition);
		_vec2 v2DPos = __super::Convert_To_2D(m_pTransformCom);
		m_fX = v2DPos.x;
		m_fY = v2DPos.y;
	}
	else
	{
		m_fX = m_vTextPosition.x;
		m_fY = m_vTextPosition.y;
	}

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iDamage % 10)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_LerpColorNAlpha)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}

	// 10자리 수.
	if (m_iDamage > 9)
	{
		if (!m_bOrth)
		{
			m_pTransformCom->Set_State(State::Pos, m_pParentTransform->Get_State(State::Pos) + m_vTextPosition);
			_vec2 v2DPos = __super::Convert_To_2D(m_pTransformCom);
			m_fX = v2DPos.x - 15.f;
			m_fY = v2DPos.y;
		}
		else
		{
			m_fX = m_vTextPosition.x - 20.f;
			m_fY = m_vTextPosition.y;
		}

		__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

		if (FAILED(Bind_ShaderResources()))
		{
			return E_FAIL;
		}
		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", (m_iDamage % 100) / 10)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(VTPass_LerpColorNAlpha)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pVIBufferCom->Render()))
		{
			return E_FAIL;
		}

		// 100자리 수.
		if (m_iDamage > 99)
		{
			if (!m_bOrth)
			{
				m_pTransformCom->Set_State(State::Pos, m_pParentTransform->Get_State(State::Pos) + m_vTextPosition);
				_vec2 v2DPos = __super::Convert_To_2D(m_pTransformCom);
				m_fX = v2DPos.x - 30.f;
				m_fY = v2DPos.y;
			}
			else
			{
				m_fX = m_vTextPosition.x - 40.f;
				m_fY = m_vTextPosition.y;
			}
			__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

			if (FAILED(Bind_ShaderResources()))
			{
				return E_FAIL;
			}
			if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iDamage / 100)))
			{
				return E_FAIL;
			}
			if (FAILED(m_pShaderCom->Begin(VTPass_LerpColorNAlpha)))
			{
				return E_FAIL;
			}

			if (FAILED(m_pVIBufferCom->Render()))
			{
				return E_FAIL;
			}

		}
	}

	return S_OK;
}

HRESULT CNumEffect::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_NumDamage"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CNumEffect::Bind_ShaderResources()
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


	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof _vec4)))
	{
		return E_FAIL;
	}	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof _float)))
	{
		return E_FAIL;
	}
	return S_OK;
}

CNumEffect* CNumEffect::Create(_dev pDevice, _context pContext)
{
	CNumEffect* pInstance = new CNumEffect(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CNumEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNumEffect::Clone(void* pArg)
{
	CNumEffect* pInstance = new CNumEffect(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CNumEffect");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNumEffect::Free()
{
	__super::Free();

	Safe_Release(m_pEffect);
	Safe_Release(m_pParentTransform);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
