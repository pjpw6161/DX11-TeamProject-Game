#include "Effect_Dummy.h"

_int CEffect_Dummy::m_iLightID = {};

CEffect_Dummy::CEffect_Dummy(_dev pDevice, _context pContext)
	: CBlendObject(pDevice, pContext)
{
}

CEffect_Dummy::CEffect_Dummy(const CEffect_Dummy& rhs)
	: CBlendObject(rhs)
{
}

HRESULT CEffect_Dummy::Init_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Dummy::Init(void* pArg)
{
	if (not pArg)
	{
		MSG_BOX("No Argument!");
		return E_FAIL;
	}

	m_Effect = *reinterpret_cast<EffectInfo*>(pArg);

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_pTransformCom->Set_Scale(m_Effect.vSize);
	m_vScaleAcc = m_Effect.vSize;

	if (m_Effect.strUnDissolveTexture.size())
	{
		m_fUnDissolveRatio = 1.f;
	}

	m_vUV = m_Effect.vUVInit;

	if (m_Effect.hasLight)
	{
		m_strLightTag = L"Light_Effect_" + to_wstring(m_iLightID++);
		m_pGameInstance->Add_Light(m_pGameInstance->Get_CurrentLevelIndex(), m_strLightTag, m_Effect.Light_Desc);
	}

	m_fAlpha = m_Effect.fAlphaInit;

	if (not m_Effect.bSkipBloom)
	{
		m_shouldRenderBlur = true;
	}

	return S_OK;
}

void CEffect_Dummy::Tick(_float fTimeDelta)
{
	if (m_Effect.strUnDissolveTexture.size() and m_fUnDissolveRatio > 0.f)
	{
		m_fUnDissolveRatio -= fTimeDelta / m_Effect.fUnDissolveDuration;
		if (m_fUnDissolveRatio < 0.f)
		{
			m_fUnDissolveRatio = 0.f;
		}
	}

	if (m_fUnDissolveRatio <= 0.f and m_Effect.fLifeTime >= 0.f and m_fTimer > m_Effect.fLifeTime)
	{
		if (m_Effect.hasLight)
		{
			m_pGameInstance->Delete_Light(LEVEL_STATIC, m_strLightTag);
			m_Effect.hasLight = false;
		}

		if (m_Effect.strDissolveTexture.size())
		{
			m_fDissolveRatio += fTimeDelta / m_Effect.fDissolveDuration;
			if (m_fDissolveRatio > 1.f)
			{
				m_isDead = true;
			}
		}
		else
		{
			m_isDead = true;
		}
	}

	if (m_Effect.isSprite)
	{
		if (m_Effect.isFixedIndex)
		{
			m_iSpriteIndex = m_Effect.iFixedSpriteIndex;
		}
		else
		{
			m_iSpriteIndex = static_cast<_int>(m_fSpriteTimer);
			m_fSpriteTimer += (fTimeDelta * m_Effect.vNumSprites.x * m_Effect.vNumSprites.y) / m_Effect.fSpriteDuration;
			if (m_iSpriteIndex >= m_Effect.vNumSprites.x * m_Effect.vNumSprites.y)
			{
				m_iSpriteIndex = 0;
				m_fSpriteTimer = {};
			}
		}
	}

	m_fTimer += fTimeDelta;
	m_vUV += m_Effect.vUVDelta * fTimeDelta;
	m_fAlpha += m_Effect.fAlphaDelta * fTimeDelta;

	if (m_Effect.isUVLoop and
		(m_vUV.x < -1.f or m_vUV.x > 2.f or
		 m_vUV.y < -1.f or m_vUV.y > 2.f))
	{
		m_vUV = m_Effect.vUVInit;
	}

	if (m_Effect.pMatrix)
	{
		m_OffsetMatrix = *m_Effect.pMatrix;
	}

	m_pTransformCom->Set_Matrix(m_OffsetMatrix);

	switch (m_Effect.iType)
	{
	case Effect_Type::ET_PARTICLE:
	case Effect_Type::ET_PARTI_DISTORTION:
		m_pParticle->Update(fTimeDelta, m_pTransformCom->Get_World_Matrix(), m_Effect.iNumInstances, m_Effect.bApplyGravity, m_Effect.vGravityDir, m_Effect.fPartiAppearRatio, m_Effect.fPartiDissolveRatio);
		//m_WorldMatrix = m_pTransformCom->Get_World_Matrix();
		break;
	case Effect_Type::ET_RECT:
	{
		if (m_Effect.isBillboard)
		{
			m_pTransformCom->LookAway(m_pGameInstance->Get_CameraPos());
		}
		//m_pTransformCom->Rotation(m_pTransformCom->Get_State(State::Look), m_Effect.fRectRotationAngle);

		m_pTransformCom->Set_Scale(m_vScaleAcc);
		m_vScaleAcc += m_Effect.vSizeDelta * fTimeDelta;

		m_WorldMatrix = m_pTransformCom->Get_World_Matrix();
		_vec4 vPos = m_WorldMatrix.Position();
		m_WorldMatrix.Position(_vec4(0.f, 0.f, 0.f, 1.f));
		m_WorldMatrix *= _mat::CreateFromAxisAngle(_vec3(m_pTransformCom->Get_State(State::Look)), XMConvertToRadians(m_Effect.fRectRotationAngle));
		m_WorldMatrix.Position(vPos);

		_vec4 vRight = m_WorldMatrix.Right().Get_Normalized();
		_vec4 vUp = m_WorldMatrix.Up().Get_Normalized();
		_vec4 vLook = m_WorldMatrix.Look().Get_Normalized();

		vPos += vRight * m_Effect.vPosOffset.x;
		vPos += vUp * m_Effect.vPosOffset.y;
		vPos += vLook * m_Effect.vPosOffset.z;

		m_WorldMatrix.Position(vPos);
		break;
	}
	case Effect_Type::ET_MESH:
	case Effect_Type::ET_DISTORTION:
	{
		if (m_Effect.isBillboard)
		{
			m_pTransformCom->LookAway(m_pGameInstance->Get_CameraPos());
			m_pTransformCom->Set_Matrix(_mat::CreateRotationX(XMConvertToRadians(m_Effect.vBillboardRotation.x)) * m_pTransformCom->Get_World_Matrix());
			m_pTransformCom->Set_Matrix(_mat::CreateRotationY(XMConvertToRadians(m_Effect.vBillboardRotation.y)) * m_pTransformCom->Get_World_Matrix());
			m_pTransformCom->Set_Matrix(_mat::CreateRotationZ(XMConvertToRadians(m_Effect.vBillboardRotation.z)) * m_pTransformCom->Get_World_Matrix());
		}

		m_pTransformCom->Set_Scale(m_vScaleAcc);
		m_vScaleAcc += m_Effect.vSizeDelta * fTimeDelta;

		m_WorldMatrix = m_pTransformCom->Get_World_Matrix();

		_vec4 vPos = m_WorldMatrix.Position();;

		_vec4 vRight = m_WorldMatrix.Right().Get_Normalized();
		_vec4 vUp = m_WorldMatrix.Up().Get_Normalized();
		_vec4 vLook = m_WorldMatrix.Look().Get_Normalized();

		vPos += vRight * m_Effect.vPosOffset.x;
		vPos += vUp * m_Effect.vPosOffset.y;
		vPos += vLook * m_Effect.vPosOffset.z;

		m_WorldMatrix.Position(vPos);

		break;
	}
	}

	if (m_Effect.hasLight)
	{
		LIGHT_DESC* pLightInfo = m_pGameInstance->Get_LightDesc(m_pGameInstance->Get_CurrentLevelIndex(), m_strLightTag);
		pLightInfo->vPosition = m_WorldMatrix.Position();
	}
}

void CEffect_Dummy::Late_Tick(_float fTimeDelta)
{
	__super::Compute_CamDistance();
	if (m_Effect.iType == ET_DISTORTION or m_Effect.iType == ET_PARTI_DISTORTION)
	{
		m_pRendererCom->Add_RenderGroup(RG_Distortion, this);
	}
	else
	{
		m_pRendererCom->Add_RenderGroup(RG_Blend, this);
	}
}

HRESULT CEffect_Dummy::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(m_Effect.iPassIndex)))
	{
		return E_FAIL;
	}

	HRESULT hr{};
	switch (m_Effect.iType)
	{
	case Effect_Type::ET_PARTICLE:
	case Effect_Type::ET_PARTI_DISTORTION:
		hr = m_pParticle->Render();
		break;
	case Effect_Type::ET_RECT:
		hr = m_pRect->Render();
		break;
	case Effect_Type::ET_MESH:
	case Effect_Type::ET_DISTORTION:
		for (size_t i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
		{
			if (FAILED(m_pModelCom->Render(i)))
			{
				return E_FAIL;
			}
		}
		break;
	}

	return hr;
}

HRESULT CEffect_Dummy::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	switch (m_Effect.iType)
	{
	case Effect_Type::ET_PARTICLE:
	case Effect_Type::ET_PARTI_DISTORTION:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Instancing_Point"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pParticle), &m_Effect.PartiDesc)))
		{
			return E_FAIL;
		}
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex_Instancing"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		{
			return E_FAIL;
		}
		break;
	case Effect_Type::ET_RECT:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pRect))))
		{
			return E_FAIL;
		}
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex_Effect"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		{
			return E_FAIL;
		}
		break;
	case Effect_Type::ET_MESH:
	case Effect_Type::ET_DISTORTION:
		wstring PrototypeTag = L"Prototype_Model_Effect_";

		_tchar strUnicode[MAX_PATH]{};
		MultiByteToWideChar(CP_ACP, 0, &m_Effect.strModel[0], static_cast<_int>(m_Effect.strModel.size()), strUnicode, static_cast<_int>(m_Effect.strModel.size()));
		PrototypeTag += strUnicode;

		if (FAILED(__super::Add_Component(LEVEL_STATIC, PrototypeTag, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		{
			return E_FAIL;
		}
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh_Effect"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		{
			return E_FAIL;
		}
		break;
	}

	if (m_Effect.strMaskTexture.size())
	{
		wstring PrototypeTag = L"Prototype_Component_Texture_Effect_" + m_Effect.strMaskTexture;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, PrototypeTag, TEXT("Com_MaskTexture"), (CComponent**)&m_pMaskTextureCom)))
		{
			return E_FAIL;
		}
	}

	if (m_Effect.strDissolveTexture.size())
	{
		wstring PrototypeTag = L"Prototype_Component_Texture_Effect_" + m_Effect.strDissolveTexture;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, PrototypeTag, TEXT("Com_DissolveTexture"), (CComponent**)&m_pDissolveTextureCom)))
		{
			return E_FAIL;
		}
	}

	if (m_Effect.strUnDissolveTexture.size())
	{
		wstring PrototypeTag = L"Prototype_Component_Texture_Effect_" + m_Effect.strUnDissolveTexture;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, PrototypeTag, TEXT("Com_UnDissolveTexture"), (CComponent**)&m_pUnDissolveTextureCom)))
		{
			return E_FAIL;
		}
	}

	if (m_Effect.strDiffuseTexture.size())
	{
		wstring PrototypeTag = L"Prototype_Component_Texture_Effect_" + m_Effect.strDiffuseTexture;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, PrototypeTag, TEXT("Com_DiffTexture"), (CComponent**)&m_pDiffTextureCom)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CEffect_Dummy::Bind_ShaderResources()
{
	if (m_Effect.strMaskTexture.size())
	{
		if (FAILED(m_pMaskTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture")))
		{
			return E_FAIL;
		}

		if (m_Effect.iType == ET_MESH or m_Effect.iType == ET_DISTORTION)
		{
			if (FAILED(m_pShaderCom->Bind_RawValue("g_vUVTransform", &m_vUV, sizeof m_vUV)))
			{
				return E_FAIL;
			}
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof m_fAlpha)))
		{
			return E_FAIL;
		}
	}

	if (m_fDissolveRatio >= 0.f and m_Effect.strUnDissolveTexture.size())
	{
		if (FAILED(m_pUnDissolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture")))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveRatio", &m_fUnDissolveRatio, sizeof m_fUnDissolveRatio)))
		{
			return E_FAIL;
		}
	}

	if (m_fUnDissolveRatio <= 0.f and m_Effect.strDissolveTexture.size())
	{
		if (FAILED(m_pDissolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture")))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveRatio", &m_fDissolveRatio, sizeof m_fDissolveRatio)))
		{
			return E_FAIL;
		}
	}

	if (m_Effect.strDiffuseTexture.size())
	{
		string strVariableName = "g_Texture";
		if (m_Effect.iType == ET_MESH)
		{
			strVariableName = "g_DiffuseTexture";
		}
		else if (m_Effect.iType == ET_DISTORTION or m_Effect.iType == ET_PARTI_DISTORTION)
		{
			strVariableName = "g_DistortionTexture";
		}
		if (FAILED(m_pDiffTextureCom->Bind_ShaderResource(m_pShaderCom, strVariableName.c_str())))
		{
			return E_FAIL;
		}
	}
	else
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_Effect.vColor, sizeof _vec4)))
		{
			return E_FAIL;
		}
	}

	if (m_Effect.iType == ET_PARTICLE or m_Effect.iType == ET_PARTI_DISTORTION)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof _vec4)))
		{
			return E_FAIL;
		}
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(TransformType::View))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(TransformType::Proj))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
	{
		return E_FAIL;
	}

	if (m_Effect.isSprite)
	{
		if (FAILED(m_pShaderCom->Bind_RawValue("g_iIndex", &m_iSpriteIndex, sizeof m_iSpriteIndex)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vNumSprite", &m_Effect.vNumSprites, sizeof m_Effect.vNumSprites)))
		{
			return E_FAIL;
		}
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_isBlur", &m_shouldRenderBlur, sizeof m_shouldRenderBlur)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_CamNF", &m_pGameInstance->Get_CameraNF(), sizeof(_float2))))
	{
		return E_FAIL;
	}

	return S_OK;
}

CEffect_Dummy* CEffect_Dummy::Create(_dev pDevice, _context pContext)
{
	CEffect_Dummy* pInstance = new CEffect_Dummy(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CEffect_Dummy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect_Dummy::Clone(void* pArg)
{
	CEffect_Dummy* pInstance = new CEffect_Dummy(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CEffect_Dummy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect_Dummy::Free()
{
	if (m_Effect.hasLight)
	{
		m_pGameInstance->Delete_Light(m_pGameInstance->Get_CurrentLevelIndex(), m_strLightTag);
	}

	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pUnDissolveTextureCom);
	Safe_Release(m_pDissolveTextureCom);
	Safe_Release(m_pDiffTextureCom);
	Safe_Release(m_pMaskTextureCom);
	Safe_Release(m_pParticle);
	Safe_Release(m_pRect);
}
