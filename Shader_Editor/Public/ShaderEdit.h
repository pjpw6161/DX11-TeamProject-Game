#pragma once

#include "Base.h"
#include "Shader_Define.h"

BEGIN(Engine)
class CGameInstance;
class CRenderer;
END

BEGIN(ShaderEdit)

class CShaderEdit final :
    public CBase
{
private:
    CShaderEdit();
    virtual ~CShaderEdit() = default;

public:
    HRESULT Init();
    void Tick(_float fTimeDelta);
    HRESULT Render();

private:
    _dev m_pDevice = nullptr;
    _context m_pContext = nullptr;

private:
    CGameInstance* m_pGameInstance = nullptr;
    CRenderer* m_pRenderer = nullptr;
    class CImGui_Mgr* m_pImgui = nullptr;
    _float m_fTimeAcc = 0.f;
    _uint m_iFrameCount = 0;

private:
    HRESULT Open_Level();
    HRESULT Ready_Prototype_Component();

public:
    static CShaderEdit* Create();
    // CBase을(를) 통해 상속됨
    virtual void Free() override;
};

END