#include "Texture.h"
#include "Shader.h"

CTexture::CTexture(_dev pDevice, _context pContext)
	: CComponent(pDevice, pContext)
{

}

CTexture::CTexture(const CTexture& rhs)
	: CComponent(rhs)
	, m_iNumTextures(rhs.m_iNumTextures)
	, m_SRVs(rhs.m_SRVs)
{
	for (auto& pView : m_SRVs)
	{
		Safe_AddRef(pView);
	}
}

HRESULT CTexture::Init_Prototype(const wstring& strTextureFilePath, _uint iNumTextures)
{
	m_iNumTextures = iNumTextures;
	m_SRVs.reserve(m_iNumTextures);

	_tchar szTextureFilePath[MAX_PATH] = L"";

	for (size_t i = 0; i < m_iNumTextures; i++)
	{
		ID3D11ShaderResourceView* pView = nullptr;

		wsprintf(szTextureFilePath, strTextureFilePath.c_str(), i);

		_tchar szEXT[MAX_PATH] = L"";

		_wsplitpath_s(szTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);

		HRESULT hr{};

		if (!lstrcmp(szEXT, L".dds"))
		{
			hr = CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pView);
		}
		else if (!lstrcmp(szEXT, L".tga"))
		{
			MSG_BOX(".tga?");
		}
		else
		{
			hr = CreateWICTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pView);
		}
		if (FAILED(hr))
		{
			return E_FAIL;
		}

		m_SRVs.push_back(pView);
	}

	return S_OK;
}

HRESULT CTexture::Init(void* pArg)
{
	return S_OK;
}

ID3D11ShaderResourceView* CTexture::Get_SRV(_uint iTextureIndex)
{
	return m_SRVs[iTextureIndex];
}

HRESULT CTexture::Bind_ShaderResource(CShader* pShader, const _char* pVariableName, _uint iTextureIndex)
{
	if (iTextureIndex >= m_iNumTextures)
	{
		return E_FAIL;
	}

	return pShader->Bind_ShaderResourceView(pVariableName, m_SRVs[iTextureIndex]);
}

HRESULT CTexture::Bind_ShaderResourceArray(CShader* pShader, const _char* pVariableName)
{
	return pShader->Bind_ShaderResourceViewArray(pVariableName, &m_SRVs.front(), m_iNumTextures);
}

CTexture* CTexture::Create(_dev pDevice, _context pContext, const wstring& strTextureFilePath, _uint iNumTextures)
{
	CTexture* pInstance = new CTexture(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(strTextureFilePath, iNumTextures)))
	{
		MSG_BOX("Failed to Create : CTexture");
	}

	return pInstance;
}

CComponent* CTexture::Clone(void* pArg)
{
	CTexture* pInstance = new CTexture(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	__super::Free();

	for (auto& pView : m_SRVs)
	{
		Safe_Release(pView);
	}
	m_SRVs.clear();
}
