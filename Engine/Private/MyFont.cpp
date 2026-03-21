#include "MyFont.h"

CMyFont::CMyFont(_dev pDevice, _context pContext)
    : m_pDevice(pDevice)
    , m_pContext(pContext)
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CMyFont::Init(const wstring& strFilePath)
{
    m_pBatch = new SpriteBatch(m_pContext);
    if (!m_pBatch)
    {
        return E_FAIL;
    }

    m_pFont = new SpriteFont(m_pDevice, strFilePath.c_str());
    if (!m_pFont)
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CMyFont::Render(const wstring& strText, const _float2& vPosition, _float fScale, _fvector vColor, _float fRotation, _bool isFront)
{
    if (!m_pFont || !m_pBatch)
    {
        return E_FAIL;
    }

    m_pContext->GSSetShader(nullptr, nullptr, 0);

    m_pBatch->Begin();
    
    _float2 vOrigin{};
    if (!isFront)
    {
        _vector vTextSize = m_pFont->MeasureString(strText.c_str());
        XMStoreFloat2(&vOrigin, vTextSize / 2.f);
    }
    else
    {
        vOrigin = _float2(0.f, 0.f);
    }

    m_pFont->DrawString(m_pBatch, strText.c_str(), vPosition, vColor, fRotation, vOrigin, fScale);

    m_pBatch->End();

    return S_OK;
}

CMyFont* CMyFont::Create(_dev pDevice, _context pContext, const wstring& strFilePath)
{
    CMyFont* pInstance = new CMyFont(pDevice, pContext);

    if (FAILED(pInstance->Init(strFilePath)))
    {
        MSG_BOX("Failed to Create : CMyFont");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMyFont::Free()
{
    Safe_Delete(m_pBatch);
    Safe_Delete(m_pFont);

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
