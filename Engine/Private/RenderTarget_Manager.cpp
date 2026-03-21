#include "RenderTarget_Manager.h"
#include "RenderTarget.h"

CRenderTarget_Manager::CRenderTarget_Manager(_dev pDevice, _context pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CRenderTarget_Manager::Init()
{


	return S_OK;
}

HRESULT CRenderTarget_Manager::Add_RenderTarget(const wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vColor, const _bool isMultiSample)
{
	if (Find_RenderTarget(strTargetTag))
	{
		MSG_BOX("Target Tag Already Exist!");
		return E_FAIL;
	}

	CRenderTarget* pRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, iWidth, iHeight, ePixelFormat, vColor, isMultiSample);
	if (not pRenderTarget)
	{
		return E_FAIL;
	}

	m_RenderTargets.emplace(strTargetTag, pRenderTarget);

	return S_OK;
}

HRESULT CRenderTarget_Manager::Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (not pRenderTarget)
	{
		MSG_BOX("Can't Find RenderTarget!");
		return E_FAIL;
	}

	list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);

	if (pMRTList)
	{
		pMRTList->push_back(pRenderTarget);
	}
	else
	{
		list<CRenderTarget*> MRTList{};
		MRTList.push_back(pRenderTarget);

		m_MRTs.emplace(strMRTTag, MRTList);
	}

	Safe_AddRef(pRenderTarget);

	return S_OK;
}

HRESULT CRenderTarget_Manager::Begin_MRT(const wstring& strMRTTag, ID3D11DepthStencilView* pDepthStencillView, const _bool shouldClear)
{
	ID3D11ShaderResourceView* pSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};

	m_pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV);

	list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);

	if (not pMRTList)
	{
		MSG_BOX("Can't Find MRT List!");
		return E_FAIL;
	}

	m_pContext->OMGetRenderTargets(1, &m_pBackBufferRTV, &m_pMainDepthStencilView);

	ID3D11DepthStencilView* pDSV{ m_pMainDepthStencilView };

	if (pDepthStencillView)
	{
		pDSV = pDepthStencillView;

		m_pContext->ClearDepthStencilView(pDepthStencillView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	}

	ID3D11RenderTargetView* pRTVs[8]{};

	_uint iNumViews{};

	for (auto& pRenderTarget : *pMRTList)
	{
		if (shouldClear)
		{
			pRenderTarget->Clear();
		}
		pRTVs[iNumViews++] = pRenderTarget->Get_RenderTargetView();
	}

	m_pContext->OMSetRenderTargets(iNumViews, pRTVs, pDSV);

	return S_OK;
}

HRESULT CRenderTarget_Manager::End_MRT()
{
	m_pContext->OMSetRenderTargets(1, &m_pBackBufferRTV, m_pMainDepthStencilView);

	Safe_Release(m_pBackBufferRTV);
	Safe_Release(m_pMainDepthStencilView);

	return S_OK;
}

HRESULT CRenderTarget_Manager::Bind_ShaderResourceView(CShader* pShader, const _char* pVariableName, const wstring& strTargetTag)
{
	CRenderTarget* pTarget = Find_RenderTarget(strTargetTag);
	if (not pTarget)
	{
		MSG_BOX("Can't Find RenderTarget!");
		return E_FAIL;
	}

	return pTarget->Bind_ShaderResourceView(pShader, pVariableName);
}

HRESULT CRenderTarget_Manager::Copy_Texture(const wstring& strTargetTag, ID3D11Resource* pTexture)
{
	CRenderTarget* pTarget = Find_RenderTarget(strTargetTag);
	if (not pTarget)
	{
		MSG_BOX("Can't Find RenderTarget!");
		return E_FAIL;
	}

	return pTarget->Copy_Texture(pTexture);
}

ID3D11Texture2D* CRenderTarget_Manager::Get_Texture2D(const wstring& strTargetTag)
{
	CRenderTarget* pTarget = Find_RenderTarget(strTargetTag);
	if (not pTarget)
	{
		MSG_BOX("Can't Find RenderTarget!");
		return nullptr;
	}
	return pTarget->Get_Texture2D();
}

#ifdef _DEBUG
HRESULT CRenderTarget_Manager::Ready_Debug(const wstring& strTargetTag, _float2 vPos, _float2 vSize)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (not pRenderTarget)
	{
		return E_FAIL;
	}

	return pRenderTarget->Ready_Debug(vPos, vSize);
}

HRESULT CRenderTarget_Manager::Render_Debug(const wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);
	if (not pMRTList)
	{
		return E_FAIL;
	}

	for (auto& pTarget : *pMRTList)
	{
		if (FAILED(pTarget->Render(pShader, pVIBuffer)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}
#endif // _DEBUG

ID3D11ShaderResourceView* CRenderTarget_Manager::Get_SRV(const wstring& strTargetTag)
{
	CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
	if (nullptr == pRenderTarget)
		return nullptr;

	return pRenderTarget->Get_SRV();
}

CRenderTarget* CRenderTarget_Manager::Find_RenderTarget(const wstring& strTargetTag)
{
	auto iter = m_RenderTargets.find(strTargetTag);
	if (iter == m_RenderTargets.end())
	{
		return nullptr;
	}

	return iter->second;
}

list<class CRenderTarget*>* CRenderTarget_Manager::Find_MRT(const wstring& strMRTTag)
{
	auto iter = m_MRTs.find(strMRTTag);
	if (iter == m_MRTs.end())
	{
		return nullptr;
	}

	return &iter->second;
}

CRenderTarget_Manager* CRenderTarget_Manager::Create(_dev pDevice, _context pContext)
{
	CRenderTarget_Manager* pInstance = new CRenderTarget_Manager(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Create : CRenderTarget_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderTarget_Manager::Free()
{
	for (auto& pair : m_MRTs)
	{
		for (auto& pRenderTargets : pair.second)
		{
			Safe_Release(pRenderTargets);
		}
		pair.second.clear();
	}
	m_MRTs.clear();

	for (auto& pair : m_RenderTargets)
	{
		Safe_Release(pair.second);
	}
	m_RenderTargets.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
