#include "Font_Manager.h"
#include "MyFont.h"

CFont_Manager::CFont_Manager(_dev pDevice, _context pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CFont_Manager::Init()
{
	return S_OK;
}

HRESULT CFont_Manager::Add_Font(const wstring& strFontTag, const wstring& strFilePath)
{
	if (Find_Font(strFontTag))
	{
		MSG_BOX("Font Tag Already Exist!");
		return E_FAIL;
	}

	CMyFont* pFont = CMyFont::Create(m_pDevice, m_pContext, strFilePath);
	if (!pFont)
	{
		return E_FAIL;
	}

	m_Fonts.emplace(strFontTag, pFont);

	return S_OK;
}

HRESULT CFont_Manager::Render(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _float fScale, _fvector vColor, _float fRotation, _bool isFront)
{
	CMyFont* pFont = Find_Font(strFontTag);
	if (!pFont)
	{
		return E_FAIL;
	}

	return pFont->Render(strText, vPosition, fScale, vColor, fRotation, isFront);
}

_vec2 CFont_Manager::Get_TextSize(const wstring& strFontTag, const wstring& strText)
{
	CMyFont* pFont = Find_Font(strFontTag);
	if (!pFont)
	{
		return _vec2(0.f, 0.f);
	}
	return pFont->Get_TextSize(strText);
}

CMyFont* CFont_Manager::Find_Font(const wstring& strFontTag)
{
	auto iter = m_Fonts.find(strFontTag);

	if (iter == m_Fonts.end())
	{
		return nullptr;
	}

	return iter->second;
}

CFont_Manager* CFont_Manager::Create(_dev pDevice, _context pContext)
{
	CFont_Manager* pInstance = new CFont_Manager(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Create : CFont_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFont_Manager::Free()
{
	for (auto& Pair : m_Fonts)
	{
		Safe_Release(Pair.second);
	}
	m_Fonts.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
