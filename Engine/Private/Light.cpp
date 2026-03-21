#include "Light.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

CLight::CLight()
{
}

LIGHT_DESC* CLight::Get_LightDesc()
{
	return &m_LightDesc;
}

void CLight::Dim(_float fDuration)
{
	m_shouldDim = true;
	m_fDimmerDuration = fDuration;
}

const _bool& CLight::Is_Dead()
{
	return m_isDead;
}

HRESULT CLight::Init(const LIGHT_DESC& LightDeesc)
{
	m_LightDesc = LightDeesc;

	return S_OK;
}

void CLight::Tick(_float fTimeDelta)
{
	if (m_shouldDim)
	{
		_float fAlpha = m_fDimmerTimer / m_fDimmerDuration;
		m_LightDesc.vDiffuse = _vec4::Lerp(vOriginDiffuse, _vec4(), fAlpha);
		m_LightDesc.vAmbient = _vec4::Lerp(vOriginAmbient, _vec4(), fAlpha);
		m_LightDesc.vSpecular = _vec4::Lerp(vOriginSpecular, _vec4(), fAlpha);
		m_fDimmerTimer += fTimeDelta;
		if (fAlpha >= 1.f)
		{
			m_isDead = true;
		}
	}
	else
	{
		vOriginDiffuse = m_LightDesc.vDiffuse;
		vOriginAmbient = m_LightDesc.vAmbient;
		vOriginSpecular = m_LightDesc.vSpecular;
	}
}

HRESULT CLight::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	_uint iPassIndex{};

	if (m_LightDesc.eType == LIGHT_DESC::Directional)
	{
		if (FAILED(pShader->Bind_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof _float4)))
		{
			return E_FAIL;
		}
		iPassIndex = DefPass_Light_Directional;
	}
	else if (m_LightDesc.eType == LIGHT_DESC::Point)
	{
		iPassIndex = DefPass_Light_Point;

		if (FAILED(pShader->Bind_RawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof _float4)))
		{
			return E_FAIL;
		}
		if (FAILED(pShader->Bind_RawValue("g_vLightAtt", &m_LightDesc.vAttenuation, sizeof _float4)))
		{
			return E_FAIL;
		}
	}

	if (FAILED(pShader->Bind_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof _float4)))
	{
		return E_FAIL;
	}
	if (FAILED(pShader->Bind_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof _float4)))
	{
		return E_FAIL;
	}
	if (FAILED(pShader->Bind_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof _float4)))
	{
		return E_FAIL;
	}

	if (FAILED(pShader->Begin(iPassIndex)))
	{
		return E_FAIL;
	}

	if (FAILED(pVIBuffer->Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLight::Bind_ViewProjMatrix(CShader* pShader, const _char* pViewVariableName, const _char* pProjVariableName)
{
	_float44		ViewMatrix, ProjMatrix;

	_float fLightFar{ 500.f };

	_vector vAt = XMLoadFloat4(&m_LightDesc.vPosition);
	_vector vPos = vAt - XMVector3Normalize(XMLoadFloat4(&m_LightDesc.vDirection)) * 50.f;

	//vAt = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	//vPos = XMVectorSet(20.f, 100.f, 0.f, 1.f);

	XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(vPos, vAt, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
	//XMStoreFloat4x4(&ProjMatrix, XMMatrixOrthographicLH(30.f, 30.f, 0.f, fLightFar));
	XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(60.0f), (_float)1280.f / 720.0f, 0.1f, fLightFar));

	if (FAILED(pShader->Bind_Matrix(pViewVariableName, ViewMatrix)))
		return E_FAIL;
	if (FAILED(pShader->Bind_Matrix(pProjVariableName, ProjMatrix)))
		return E_FAIL;
	if (FAILED(pShader->Bind_RawValue("g_fLightFar", &fLightFar, sizeof _float)))
		return E_FAIL;

	//if (FAILED(pShader->Bind_Matrix(pViewVariableName, ViewMatrix)))
	//{
	//	return E_FAIL;
	//}
	//if (FAILED(pShader->Bind_Matrix(pProjVariableName, ProjMatrix)))
	//{
	//	return E_FAIL;
	//}

	return S_OK;
}

CLight* CLight::Create(const LIGHT_DESC& LightDesc)
{
	CLight* pInstance = new CLight();

	if (FAILED(pInstance->Init(LightDesc)))
	{
		MSG_BOX("Failed to Create : CLight");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
}
