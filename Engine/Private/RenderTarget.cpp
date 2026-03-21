#include "RenderTarget.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

CRenderTarget::CRenderTarget(_dev pDevice, _context pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

ID3D11RenderTargetView* CRenderTarget::Get_RenderTargetView()
{
	return m_pRenderTargetView;
}

ID3D11Texture2D* CRenderTarget::Get_Texture2D()
{
	return m_pTexture;
}

HRESULT CRenderTarget::Init(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vColor, const _bool isMultiSample)
{
	m_vClearColor = vColor;

	D3D11_TEXTURE2D_DESC Desc{};
	Desc.Width = iWidth;
	Desc.Height = iHeight;
	Desc.MipLevels = 1;
	Desc.ArraySize = 1;
	Desc.Format = ePixelFormat;

	//if (isMultiSample)
	//{
	//	_uint NumQualityLevels{};
	//	m_pDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM, 4, &NumQualityLevels);

	//	Desc.SampleDesc.Quality = NumQualityLevels - 1;
	//	Desc.SampleDesc.Count = 4;
	//}
	//else
	{
		Desc.SampleDesc.Quality = 0;
		Desc.SampleDesc.Count = 1;
	}

	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	Desc.CPUAccessFlags = 0;
	Desc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&Desc, nullptr, &m_pTexture)))
	{
		return E_FAIL;
	}

	//D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	//ZeroMemory(&rtvDesc, sizeof(rtvDesc));
	//rtvDesc.Format = Desc.Format;
	//rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;

	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture, nullptr, &m_pRenderTargetView)))
	{
		return E_FAIL;
	}

	//D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	//ZeroMemory(&srvDesc, sizeof(srvDesc));
	//srvDesc.Format = Desc.Format;
	//srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture, nullptr, &m_pShaderResourceView)))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CRenderTarget::Clear()
{
	m_pContext->ClearRenderTargetView(m_pRenderTargetView, reinterpret_cast<_float*>(&m_vClearColor));
}

HRESULT CRenderTarget::Bind_ShaderResourceView(CShader* pShader, const _char* pVariableName)
{
	if (FAILED(pShader->Bind_ShaderResourceView(pVariableName, m_pShaderResourceView)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderTarget::Copy_Texture(ID3D11Resource* pTexture)
{
	m_pContext->CopyResource(m_pTexture, pTexture);

	Safe_Release(pTexture);

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderTarget::Ready_Debug(_float2 vPos, _float2 vSize)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	_uint iNumViewPorts{ 1 };

	D3D11_VIEWPORT ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewPorts, &ViewportDesc);

	m_WorldMatrix.m[0][0] = vSize.x;
	m_WorldMatrix.m[1][1] = vSize.y;
	m_WorldMatrix.m[3][0] = vPos.x - ViewportDesc.Width * 0.5f;
	m_WorldMatrix.m[3][1] = -vPos.y + ViewportDesc.Height * 0.5f;

	return S_OK;
}

HRESULT CRenderTarget::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
	{
		return E_FAIL;
	}

	if (FAILED(pShader->Bind_ShaderResourceView("g_DebugTexture", m_pShaderResourceView)))
	{
		return E_FAIL;
	}

	if (FAILED(pShader->Begin(DefPass_Debug)))
	{
		return E_FAIL;
	}

	if (FAILED(pVIBuffer->Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}
#endif // _DEBUG

CRenderTarget* CRenderTarget::Create(_dev pDevice, _context pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vColor, const _bool isMultiSample)
{
	CRenderTarget* pInstance = new CRenderTarget(pDevice, pContext);

	if (FAILED(pInstance->Init(iWidth, iHeight, ePixelFormat, vColor, isMultiSample)))
	{
		MSG_BOX("Failed to Create : CRenderTarget");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CRenderTarget::Free()
{
	//SaveDDSTextureToFile(m_pContext, m_pTexture, TEXT("../Bin/Diffuse.dds"));

	Safe_Release(m_pRenderTargetView);
	Safe_Release(m_pShaderResourceView);

	Safe_Release(m_pTexture);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
