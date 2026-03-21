#pragma once
#include "Base.h"

BEGIN(Engine)

class CCascade_Manager final :
    public CBase
{
private:
    CCascade_Manager(_dev pDevice, _context pContext);
    virtual ~CCascade_Manager() = default;

public:
    HRESULT Init();
    HRESULT Update_Cascade();

    CASCADE_DESC Get_CascadeDesc() {
        return m_CascadeDesc;
    }

private:
    vector<class CCascade*> m_pCascades;

   //{ 0.f, 0.2f, 0.4f, 1.f };
    CASCADE_DESC m_CascadeDesc;
    _dev m_pDevice = nullptr;
    _context m_pContext = nullptr;

    class CGameInstance* m_pGameInstance = nullptr;

public:
    static CCascade_Manager* Create(_dev pDevice, _context pContext);
    // CBase을(를) 통해 상속됨
    virtual void Free() override;
};

END