#pragma once

#include "Shader_Define.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CTransform;
class CRenderer;
END

BEGIN(ShaderEdit)

class CImGui_Mgr final :
    public CBase
{
    DECLARE_SINGLETON(CImGui_Mgr)

private:
    CImGui_Mgr();
    virtual ~CImGui_Mgr() = default;

public:
    HRESULT Init(_dev pDevice, _context pContext, class CGameInstance* pGameInstance, CRenderer* pRenderer);

    void Frame();

    void Editing(_float fTimeDelta);

    void Render();

private:
    _dev m_pDevice = nullptr;
    _context m_pContext = nullptr;
    CRenderer* m_pRenderer = nullptr;

private:
    CGameInstance* m_pGameInstance = nullptr;


private:
    // SSAO
    SSAO_DESC m_SSAO;

    _float m_fSSAOBlurPower = 0.f;

    // Effect
    _float m_fEffectBlurPower = 0.f;

    // HDR
    HDR_DESC m_HDR;

    _float m_fHDRBlurPower = 0.f;

public:
    // CBase을(를) 통해 상속됨
    virtual void Free() override;
};

END