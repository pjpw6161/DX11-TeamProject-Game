#pragma once
#include "Base.h"

BEGIN(Engine)

class CShadowMap :
    public CBase
{
private:
    CShadowMap(_dev pDevice, _context pContext);
    virtual ~CShadowMap() = default;

public:
    HRESULT Init(_uint2 iSize, const _float4& vColor);
    HRESULT Begin_ShadowMap();
    HRESULT End_ShadowMap();
    HRESULT Bind_SRV(class CShader* pShader, const _char* pVariableName);

#ifdef _DEBUG
    HRESULT Ready_Debug(_float2 vPos, _float2 vSize);
    HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif // _DEBUG

private:
    _dev m_pDevice = nullptr;
    _context m_pContext = nullptr;

    ID3D11Texture2D* m_pTex = nullptr;
    ID3D11ShaderResourceView* m_pSRV = nullptr;
    ID3D11DepthStencilView* m_pDSV = nullptr;

    _float4 m_vClearColor{};

    D3D11_VIEWPORT m_ViewPort{};

    
#ifdef _DEBUG
    _float44 m_WorldMatrix{};
#endif // _DEBUG

private:
    D3D11_VIEWPORT m_OldViewportDesc{};

    ID3D11RenderTargetView* m_pBackBufferRTV{ nullptr };
    ID3D11DepthStencilView* m_pMainDepthStencilView{ nullptr };

public:
    static CShadowMap* Create(_dev pDevice, _context pContext, _uint2 iSize, const _float4& vColor);
    // CBase을(를) 통해 상속됨
    virtual void Free() override;
};

END