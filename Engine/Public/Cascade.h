#pragma once
#include "Base.h"

BEGIN(Engine)

class CCascade final :
    public CBase
{
private:
    CCascade(_dev pDevice, _context pContext);
    virtual ~CCascade() = default;

public:
    HRESULT Init(_uint iIndex);
    HRESULT Update(_vec3* FrustumCorners, _float* Cascade);

    _mat Get_ViewMat() const {
        return m_ViewMat;
    }

    _mat Get_ProjMat() const {
        return m_ProjMat;
    }

private:
    _uint m_iIndex = 0;
    _vec4 m_ShadowCamPos;
    _vec3 m_FrustumCorners[8]{};

    _mat m_ProjMat{};
    _mat m_ViewMat{};

    _dev m_pDevice = nullptr;
    _context m_pContext = nullptr;

    class CGameInstance* m_pGameInstance = nullptr;

public:
    static CCascade* Create(_dev pDevice, _context pContext, _uint iIndex);
    // CBase을(를) 통해 상속됨
    virtual void Free() override;
};

END