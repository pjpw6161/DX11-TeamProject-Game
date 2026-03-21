#include "Compute_RenderTarget.h"

CCompute_RenderTarget::CCompute_RenderTarget(_dev pDevice, _context pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCompute_RenderTarget::Init(_uint2 iSize, DXGI_FORMAT ePixelFormat)
{
	m_iSize = iSize;

	D3D11_TEXTURE2D_DESC TexDesc{};

	TexDesc.Width = m_iSize.x;
	TexDesc.Height = m_iSize.y;
	TexDesc.MipLevels = 1;
	TexDesc.ArraySize = 1;
	TexDesc.Format = ePixelFormat;
	TexDesc.SampleDesc.Count = 1;
	TexDesc.Usage = D3D11_USAGE_DEFAULT;
	TexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	TexDesc.CPUAccessFlags = 0;
	TexDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TexDesc, nullptr, &m_pTexture)))
		return E_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
	SRVDesc.Format = TexDesc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MostDetailedMip = 0;
	SRVDesc.Texture2D.MipLevels = 1;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture, &SRVDesc, &m_pSRV)))
		return E_FAIL;

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc{};

	UAVDesc.Format = TexDesc.Format;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	UAVDesc.Texture2D.MipSlice = 0;

	if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pTexture, &UAVDesc, &m_pUAV)))
		return E_FAIL;

	return S_OK;
}

CCompute_RenderTarget* CCompute_RenderTarget::Create(_dev pDevice, _context pContext, _uint2 iSize, DXGI_FORMAT ePixelFormat)
{
	CCompute_RenderTarget* pInstance = new CCompute_RenderTarget(pDevice, pContext);

	if (FAILED(pInstance->Init(iSize, ePixelFormat)))
	{
		MSG_BOX("Failed to Create : CCompute_RenderTarget");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCompute_RenderTarget::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pTexture);
	Safe_Release(m_pSRV);
	Safe_Release(m_pUAV);
}
