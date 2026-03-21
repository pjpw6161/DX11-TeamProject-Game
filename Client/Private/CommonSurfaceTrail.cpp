#include "CommonSurfaceTrail.h"

CCommonSurfaceTrail::CCommonSurfaceTrail(_dev pDevice, _context pContext)
	: CBlendObject(pDevice, pContext)
{
}

CCommonSurfaceTrail::CCommonSurfaceTrail(const CCommonSurfaceTrail& rhs)
	: CBlendObject(rhs)
{
}

void CCommonSurfaceTrail::On()
{
	m_bNoRender = false;
	m_fDissolveRatio = 0.f;
}

void CCommonSurfaceTrail::Set_Color(const _color vColor)
{
	m_Info.vColor = vColor;
}

HRESULT CCommonSurfaceTrail::Init_Prototype()
{
	return S_OK;
}

HRESULT CCommonSurfaceTrail::Init(void* pArg)
{
	if (not pArg)
	{
		MSG_BOX("No Argument!");
		return E_FAIL;
	}

	m_Info = *reinterpret_cast<SURFACETRAIL_DESC*>(pArg);

	if (m_Info.iNumVertices > 50)
	{
		MSG_BOX("버텍스 개수는 50을 초과할 수 없습니다.");
	}

	if (m_Info.strMaskTextureTag.empty())
	{
		m_Info.strMaskTextureTag = L"FX_B_Gradient003_Tex";
	}

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_TopPosArray = new _vec3[m_Info.iNumVertices];
	m_BottomPosArray = new _vec3[m_Info.iNumVertices];
	m_AlphaArray = new _float[m_Info.iNumVertices];

	m_shouldRenderBlur = true;

	return S_OK;
}

void CCommonSurfaceTrail::Tick(_vec3 vTopPos, _vec3 vBottomPos)
{
	if (m_bNoRender and m_fDissolveRatio < 1.f)
	{
		return;
	}

	if (m_TopPosList.size() >= m_Info.iNumVertices)
	{
		m_TopPosList.pop_back();
	}
	m_TopPosList.push_front(vTopPos);

	if (m_BottomPosList.size() >= m_Info.iNumVertices)
	{
		m_BottomPosList.pop_back();
	}
	m_BottomPosList.push_front(vBottomPos);

	m_pTransformCom->Set_State(State::Pos, XMVectorSetW(XMLoadFloat3(&vTopPos), 1.f));
}

void CCommonSurfaceTrail::Late_Tick(_float fTimeDelta)
{
	if (m_fDissolveRatio >= 1.f)
	{
		return;
	}

	if (m_bNoRender)
	{
		m_fDissolveRatio += fTimeDelta * 4.f;
	}

	for (size_t i = 0; i < m_Info.iNumVertices; i++)
	{
		m_AlphaArray[i] = Saturate((1.f - static_cast<_float>(i) / m_Info.iNumVertices) - m_fDissolveRatio);
	}

	_uint iIndex{};
	for (auto& vPos : m_TopPosList)
	{
		m_TopPosArray[iIndex++] = vPos;
	}

	iIndex = {};
	for (auto& vPos : m_BottomPosList)
	{
		m_BottomPosArray[iIndex++] = vPos;
	}

	m_pTrailBufferCom->Update(m_Info.iNumVertices, m_TopPosArray, m_BottomPosArray, m_AlphaArray);

	__super::Compute_CamDistance();
	if (m_Info.iPassIndex == 2)
	{
		m_pRendererCom->Add_RenderGroup(RG_Distortion, this);
	}
	else
	{
		m_pRendererCom->Add_RenderGroup(RG_Blend, this);
	}
	m_bNoRender = true;
}

HRESULT CCommonSurfaceTrail::Render()
{
	if (m_pGameInstance->Get_CurrentLevelIndex() == LEVEL_LOADING)
	{
		return S_OK;
	}

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(m_Info.iPassIndex)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTrailBufferCom->Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCommonSurfaceTrail::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Trail_Surface_50"), TEXT("Com_TrailBuffer"), reinterpret_cast<CComponent**>(&m_pTrailBufferCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex_Trail_Surface"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	wstring PrototypeTag = L"Prototype_Component_Texture_Effect_" + m_Info.strMaskTextureTag;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, PrototypeTag, TEXT("Com_MaskTexture"), (CComponent**)&m_pMaskTextureCom)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CCommonSurfaceTrail::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(TransformType::View))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(TransformType::Proj))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_Info.vColor, sizeof(m_Info.vColor))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
	{
		return E_FAIL;
	}
	
	if (FAILED(m_pShaderCom->Bind_RawValue("g_isBlur", &m_shouldRenderBlur, sizeof _bool)))
	{
		return E_FAIL;
	}


	return S_OK;
}

CCommonSurfaceTrail* CCommonSurfaceTrail::Create(_dev pDevice, _context pContext)
{
	CCommonSurfaceTrail* pInstnace = new CCommonSurfaceTrail(pDevice, pContext);

	if (FAILED(pInstnace->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CCommonSurfaceTrail");
		Safe_Release(pInstnace);
	}

	return pInstnace;
}

CGameObject* CCommonSurfaceTrail::Clone(void* pArg)
{
	CCommonSurfaceTrail* pInstance = new CCommonSurfaceTrail(*this);

	if (FAILED(pInstance->Init((pArg))))
	{
		MSG_BOX("Failed to Clone : CCommonSurfaceTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCommonSurfaceTrail::Free()
{
	__super::Free();

	Safe_Delete_Array(m_TopPosArray);
	Safe_Delete_Array(m_BottomPosArray);
	Safe_Delete_Array(m_AlphaArray);

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTrailBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pMaskTextureCom);
}
