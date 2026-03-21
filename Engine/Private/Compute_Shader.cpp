#include "Compute_Shader.h"

constexpr ID3D11ShaderResourceView* NULL_SRV[8] = {};
constexpr ID3D11UnorderedAccessView* NULL_UAV[8] = {};
constexpr _uint NO_OFFSET = -1;

CCompute_Shader::CCompute_Shader(_dev pDevice, _context pContext)
	: CComponent(pDevice, pContext)
{
}

CCompute_Shader::CCompute_Shader(const CCompute_Shader& rhs)
	: CComponent(rhs)
	, m_pBuffer(rhs.m_pBuffer)
	, m_pBlob(rhs.m_pBlob)
	, m_pShader(rhs.m_pShader)
{
	Safe_AddRef(m_pBuffer);
	Safe_AddRef(m_pBlob);
	Safe_AddRef(m_pShader);
}

HRESULT CCompute_Shader::Init_Prototype(const wstring& strShaderFilePath, const string& strEntryPoint, _uint iDataSize)
{
	if (0 < iDataSize) {

		D3D11_BUFFER_DESC BDesc{};

		BDesc.ByteWidth = iDataSize;
		BDesc.Usage = D3D11_USAGE_DYNAMIC;
		BDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		BDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		if (FAILED(m_pDevice->CreateBuffer(&BDesc, nullptr, &m_pBuffer)))
			return E_FAIL;
	}

	ID3DBlob* ErrorBlob = nullptr;

	if (FAILED(D3DCompileFromFile(strShaderFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, strEntryPoint.c_str(), "cs_5_0", 0, 0, &m_pBlob, &ErrorBlob))) {
		if (nullptr != ErrorBlob) {
			OutputDebugStringA((_char*)ErrorBlob->GetBufferPointer());
			Safe_Release(ErrorBlob);
		}
		MSG_BOX("컴퓨트 쉐이더 생성 실패 ㅅㄱ");
		return E_FAIL;
	}

	if (FAILED(m_pDevice->CreateComputeShader(m_pBlob->GetBufferPointer(), m_pBlob->GetBufferSize(), nullptr, &m_pShader)))
		return E_FAIL;

	/*D3D11_QUERY_DESC QueryDesc{};
	QueryDesc.Query = D3D11_QUERY_EVENT;

	if (FAILED(m_pDevice->CreateQuery(&QueryDesc, &m_pQuery)))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CCompute_Shader::Init(void* pArg)
{
	return S_OK;
}

HRESULT CCompute_Shader::Set_Shader()
{
	//ID3D11ShaderResourceView* pSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};

	//m_pContext->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, pSRV);

	m_pContext->CSSetShader(m_pShader, nullptr, 0);

	return S_OK;
}

HRESULT CCompute_Shader::Begin(_uint3 ThreadGroupCount)
{
	m_pContext->Dispatch(ThreadGroupCount.x, ThreadGroupCount.y, ThreadGroupCount.z);

	//m_pContext->Flush();

	//m_pContext->End(m_pQuery);

	//while (S_OK != m_pContext->GetData(m_pQuery, nullptr, 0, 0)) {
	//	int a = 0;
	//}
	
	m_pContext->CSSetShaderResources(m_iSlot.x, m_iNumViews, NULL_SRV);
	m_pContext->CSSetUnorderedAccessViews(m_iSlot.y, 1, NULL_UAV, &NO_OFFSET);


	//m_pContext->CSSetShader(nullptr, nullptr, 0);

	return S_OK;
}

HRESULT CCompute_Shader::Change_Value(void* pArg, _uint iDataSize, _uint iSlot)
{
	D3D11_MAPPED_SUBRESOURCE MSR{};

	if (FAILED(m_pContext->Map(m_pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MSR)))
		return E_FAIL;

	memcpy(MSR.pData, pArg, iDataSize);

	m_pContext->Unmap(m_pBuffer, 0);

	m_pContext->CSSetConstantBuffers(iSlot, 1, &m_pBuffer);

	return S_OK;
}

HRESULT CCompute_Shader::Bind_ShaderResourceView(ID3D11ShaderResourceView* pSRV, ID3D11UnorderedAccessView* pUAV, _uint2 iSlot)
{
	m_iSlot = iSlot;

	m_pContext->CSSetShaderResources(m_iSlot.x, 1, &pSRV);

	m_pContext->CSSetUnorderedAccessViews(m_iSlot.y, 1, &pUAV, &NO_OFFSET);

	return S_OK;
}

HRESULT CCompute_Shader::Bind_SRVs(ID3D11ShaderResourceView** pSRVs, _uint iSlot, _uint iNumViews)
{
	m_iSlot.x = iSlot;

	m_pContext->CSSetShaderResources(iSlot, iNumViews, pSRVs);

	return S_OK;
}

HRESULT CCompute_Shader::Bind_UAV(ID3D11UnorderedAccessView* pUAV, _uint iSlot)
{
	m_iSlot.y = iSlot;

	m_pContext->CSSetUnorderedAccessViews(iSlot, 1, &pUAV, &NO_OFFSET);

	return S_OK;
}

HRESULT CCompute_Shader::Begin_MultiSRV(_uint3 ThreadGroupCount)
{
	m_pContext->Dispatch(ThreadGroupCount.x, ThreadGroupCount.y, ThreadGroupCount.z);

	m_pContext->CSSetShaderResources(m_iSlot.x, m_iNumViews, NULL_SRV);
	m_pContext->CSSetUnorderedAccessViews(m_iSlot.y, 1, NULL_UAV, &NO_OFFSET);
	return S_OK;
}

CCompute_Shader* CCompute_Shader::Create(_dev pDevice, _context pContext, const wstring& strShaderFilePath, const string& strEntryPoint, _uint iDataSize)
{
	CCompute_Shader* pInstance = new CCompute_Shader(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(strShaderFilePath, strEntryPoint, iDataSize)))
	{
		MSG_BOX("Failed to Create : CCompute_Shader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CCompute_Shader::Clone(void* pArg)
{
	CCompute_Shader* pInstance = new CCompute_Shader(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CCompute_Shader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCompute_Shader::Free()
{
	__super::Free();

	Safe_Release(m_pBuffer);
	Safe_Release(m_pBlob);
	Safe_Release(m_pShader);
	//Safe_Release(m_pQuery);
}