#include "Terrain.h"
#include "GameInstance.h"
CTerrain::CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CTerrain::CTerrain(const CTerrain& rhs)
	:CGameObject(rhs)
{
}

HRESULT CTerrain::Init_Prototype()
{
	return S_OK;
}

HRESULT CTerrain::Init(void* pArg)
{
	if (!pArg)
	{
		MSG_BOX("No Argument!");
		return E_FAIL;
	}

	//m_vTerrainSize = *reinterpret_cast<_uint2*>(pArg);
	m_TerrainInfo = *(TERRAIN_INFO*)pArg;
	
	if (FAILED(Add_Component()))
		return E_FAIL;
	
	return S_OK;
}

void CTerrain::Tick(_float fTimeDelta)
{

}

void CTerrain::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);
}

HRESULT CTerrain::Render()
{

	// Begin을 호출하기 전에 셰이더에 넘겨줘야 한다.
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	// Pass가 하나만 있으므로 index는 0
	if(FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

// 컴포넌트 추가 함수
HRESULT CTerrain::Add_Component()
{
	// For.Com_Renderer
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		return E_FAIL;
	}

	// For.Com_Shader
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		return E_FAIL;
	}
	// For.Com_VIBuffer
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, &m_TerrainInfo)))
	{
		return E_FAIL;
	}

	// For.Com_Texture
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom[TYPE_DIFFUSE])))
		return E_FAIL;



	return S_OK;
}

HRESULT CTerrain::Bind_ShaderResources()
{
	_float44 pWorldMatrix = m_pTransformCom->Get_World_Matrix();
	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", pWorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(TransformType::View))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(TransformType::Proj))))
		return E_FAIL;

	if (FAILED(m_pTextureCom[TYPE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_CamNF", &m_pGameInstance->Get_CameraNF(), sizeof(_float2))))
		return E_FAIL;

	const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(LEVEL_STATIC, TEXT("Light_Main"));
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}


CTerrain* CTerrain::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	// 원형 객체 생성 -> Initialize_Prototype 을 호출한다
	CTerrain* pInstance = new CTerrain(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTerrain::Clone(void* pArg)
{
	// 실사용 객체를 생성한다. -> 복사 생성자 호출

	CTerrain* pInstance = new CTerrain(*this);
	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTerrain");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain::Free()
{
	__super::Free();

	for (size_t i = 0; i < TYPE_END; i++)
		Safe_Release(m_pTextureCom[i]);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
