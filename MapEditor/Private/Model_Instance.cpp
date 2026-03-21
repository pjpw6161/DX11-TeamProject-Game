#include "Model_Instance.h"
static _int iID = 1;

CModel_Instance::CModel_Instance(_dev pDevice, _context pContext)
	: CBlendObject(pDevice, pContext)
{
}

CModel_Instance::CModel_Instance(const CModel_Instance& rhs)
	: CBlendObject(rhs)
	//, m_pImGui_Manager(CImGui_Manager::Get_Instance())
{
	//Safe_AddRef(m_pImGui_Manager);
	m_iID = iID++;
}


HRESULT CModel_Instance::Init_Prototype()
{
	return S_OK;
}

HRESULT CModel_Instance::Init(void* pArg)
{
	m_Info = *(MapInfo*)pArg;

	_vec3 MapPos = _vec3(m_Info.vPos.x, m_Info.vPos.y, m_Info.vPos.z);
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (m_Info.ppMap)
	{
		*m_Info.ppMap = this;
		m_Info.ppMap = nullptr;
	}


	m_iShaderPass = StaticPass_AlphaTestMeshes;

	m_pTransformCom->Set_Position(MapPos);

	return S_OK;
}

void CModel_Instance::Tick(_float fTimeDelta)
{

}

void CModel_Instance::Late_Tick(_float fTimeDelta)
{

	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend, this);

}

HRESULT CModel_Instance::Render()
{

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
			return E_FAIL;
		}

		_bool HasNorTex{};
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
		{
			HasNorTex = false;
		}
		else
		{
			HasNorTex = true;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_bSelected", &m_isSelected, sizeof _bool)))
		{
			return E_FAIL;
		}


		if (FAILED(m_pShaderCom->Begin(m_iOutLineShaderPass)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(m_iShaderPass)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i)))
		{
			return E_FAIL;
		}
	}


	return S_OK;
}

HRESULT CModel_Instance::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	m_iShaderPass = StaticPass_Default;
	m_iOutLineShaderPass = StaticPass_OutLine;

	if (FAILED(Add_Component(LEVEL_STATIC), TEXT("Com_VB")))

		if (FAILED(__super::Add_Component(LEVEL_STATIC, m_Info.Prototype, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		{
			return E_FAIL;
		}

	return S_OK;
}

HRESULT CModel_Instance::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(TransformType::View))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(TransformType::Proj))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof _float4)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &m_pGameInstance->Get_CameraNF().y, sizeof _float)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShaderCom->Bind_RawValue("g_iID", &m_iID, sizeof(_int))))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CModel_Instance::Select(const _bool& isSelected)
{
	m_isSelected = isSelected;
}

void CModel_Instance::Mode(const _bool& isMode)
{
	m_isMode = isMode;
}

HRESULT CModel_Instance::Create_HightMap(vector<VTXSTATICMESH> VerticesPos)
{
	vector<_float> vHight;
	for (const auto& Value : VerticesPos) {
		vHight.push_back(Value.vPosition.y);
	}
	auto minHeight = *min_element(vHight.begin(), vHight.end());
	auto maxHeight = *max_element(vHight.begin(), vHight.end());

	ID3D11Texture2D* pTexture2D = nullptr;
	D3D11_TEXTURE2D_DESC	TextureDesc = {};

	TextureDesc.Width = 128;
	TextureDesc.Height = 128;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	vector<_uint> colors(TextureDesc.Width * TextureDesc.Height);

	ID3D11Texture2D* pTexture = nullptr;

	for (int y = 0; y < TextureDesc.Height; ++y) {
		for (int x = 0; x < TextureDesc.Width; ++x) {
			int vertexIndex = y * TextureDesc.Width + x;

			// 버텍스 배열의 크기를 넘어서지 않도록 검사합니다.
			if (vertexIndex < vHight.size()) {
				_float fValue = (vHight[vertexIndex] - minHeight) / (maxHeight - minHeight);
				_uint colorValue = static_cast<_uint>(lerp(0, 255, fValue));

				// ARGB 포맷으로 변환하여 텍스처의 해당 위치에 색상값을 저장합니다.
				int textureIndex = y * TextureDesc.Width + x;
				colors[textureIndex] = (colorValue << 24) | (colorValue << 16) | (colorValue << 8) | colorValue;
			}
		}
	}


	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = colors.data();
	initData.SysMemPitch = TextureDesc.Width * 4;
	initData.SysMemSlicePitch = 0;


	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, &initData, &pTexture)))
		return E_FAIL;


	if (FAILED(SaveWICTextureToFile(m_pContext, pTexture, GUID_ContainerFormatPng, L"../Bin/Data/HightMap.png")))
		return E_FAIL;

	Safe_Release(pTexture2D);
	Safe_Release(pTexture);

	return S_OK;
}

_float CModel_Instance::lerp(_float a, _float b, _float f) {
	return a + f * (b - a);
}

CModel_Instance* CModel_Instance::Create(_dev pDevice, _context pContext)
{
	CModel_Instance* pInstance = new CModel_Instance(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CModel_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CModel_Instance::Clone(void* pArg)
{
	CModel_Instance* pInstance = new CModel_Instance(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CModel_Instance");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel_Instance::Free()
{
	__super::Free();

	//Safe_Release(m_pImGui_Manager);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
}
