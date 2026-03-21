#include "ShadowMap.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "GameInstance.h"

CShadowMap::CShadowMap(_dev pDevice, _context pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CShadowMap::Init(_uint2 iSize, const _float4& vColor)
{
	D3D11_TEXTURE2D_DESC TexDesc{};
	TexDesc.Width = iSize.x * 2;
	TexDesc.Height = iSize.x * 2;
	TexDesc.MipLevels = 1;
	TexDesc.ArraySize = 3;
	TexDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	TexDesc.SampleDesc.Count = 1;
	TexDesc.SampleDesc.Quality = 0;
	TexDesc.Usage = D3D11_USAGE_DEFAULT;
	TexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	TexDesc.CPUAccessFlags = 0;
	TexDesc.MiscFlags = 0;


	if (FAILED(m_pDevice->CreateTexture2D(&TexDesc, 0, &m_pTex)))
		return E_FAIL;

	D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc{};
	DSVDesc.Flags = 0;
	DSVDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	DSVDesc.Texture2DArray.ArraySize = TexDesc.ArraySize;

	if (FAILED(m_pDevice->CreateDepthStencilView(m_pTex, &DSVDesc, &m_pDSV)))
		return E_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
	SRVDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	SRVDesc.Texture2DArray.MostDetailedMip = 0;
	SRVDesc.Texture2DArray.MipLevels = TexDesc.MipLevels;
	SRVDesc.Texture2DArray.FirstArraySlice = 0;
	SRVDesc.Texture2DArray.ArraySize = TexDesc.ArraySize;
	
	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTex, &SRVDesc, &m_pSRV)))
		return E_FAIL;

	m_ViewPort.TopLeftX = 0.f;
	m_ViewPort.TopLeftY = 0.f;
	m_ViewPort.Width = static_cast<_float>(iSize.x * 2);
	m_ViewPort.Height = static_cast<_float>(iSize.x * 2);
	m_ViewPort.MinDepth = 0.f;
	m_ViewPort.MaxDepth = 1.f;

	return S_OK;
}

HRESULT CShadowMap::Begin_ShadowMap()
{
	ID3D11ShaderResourceView* pSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};

	m_pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV);

	m_pContext->OMGetRenderTargets(1, &m_pBackBufferRTV, &m_pMainDepthStencilView);

	m_pContext->ClearDepthStencilView(m_pDSV, D3D11_CLEAR_DEPTH, 1.f, 0);

	ID3D11RenderTargetView* RTV[1] = { nullptr };

	m_pContext->OMSetRenderTargets(1, RTV, m_pDSV);

	_uint iNumViewPorts{ 1 };

	m_pContext->RSGetViewports(&iNumViewPorts, &m_OldViewportDesc);

	m_pContext->RSSetViewports(iNumViewPorts, &m_ViewPort);

	return S_OK;
}

HRESULT CShadowMap::End_ShadowMap()
{
	m_pContext->OMSetRenderTargets(1, &m_pBackBufferRTV, m_pMainDepthStencilView);

	Safe_Release(m_pBackBufferRTV);
	Safe_Release(m_pMainDepthStencilView);

	_uint iNumViewPorts{ 1 };

	m_pContext->RSSetViewports(iNumViewPorts, &m_OldViewportDesc);

	return S_OK;
}

HRESULT CShadowMap::Bind_SRV(CShader* pShader, const _char* pVariableName)
{
	if (FAILED(pShader->Bind_ShaderResourceView(pVariableName, m_pSRV)))
		return E_FAIL;

	return S_OK;
}

#ifdef _DEBUG
HRESULT CShadowMap::Ready_Debug(_float2 vPos, _float2 vSize)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());



	return S_OK;
}

HRESULT CShadowMap::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	for (_uint i = 0; i < 3; i++)
	{
		_uint iNumViewPorts{ 1 };

		D3D11_VIEWPORT ViewportDesc{};

		m_pContext->RSGetViewports(&iNumViewPorts, &ViewportDesc);

		m_WorldMatrix.m[0][0] = 100.f;
		m_WorldMatrix.m[1][1] = 100.f;
		m_WorldMatrix.m[3][0] = ((ViewportDesc.Width - 350.f) - 100.f * i) - ViewportDesc.Width * 0.5f;
		m_WorldMatrix.m[3][1] = -50.f + ViewportDesc.Height * 0.5f;

		if (FAILED(pShader->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
		{
			return E_FAIL;
		}

		if (FAILED(pShader->Bind_RawValue("g_DebugArrayIndex", &i, sizeof(_uint))))
			return E_FAIL;

		if (FAILED(pShader->Bind_ShaderResourceView("g_DebugArrayTexture", m_pSRV)))
		{
			return E_FAIL;
		}

		if (FAILED(pShader->Begin(DefPass_DebugArray)))
		{
			return E_FAIL;
		}

		if (FAILED(pVIBuffer->Render()))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}
#endif // _DEBUG

CShadowMap* CShadowMap::Create(_dev pDevice, _context pContext, _uint2 iSize, const _float4& vColor)
{
	CShadowMap* pInstance = new CShadowMap(pDevice, pContext);

	if (FAILED(pInstance->Init(iSize, vColor)))
	{
		MSG_BOX("Failed to Create : CShadowMap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShadowMap::Free()
{
	Safe_Release(m_pTex);
	Safe_Release(m_pSRV);
	Safe_Release(m_pDSV);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
