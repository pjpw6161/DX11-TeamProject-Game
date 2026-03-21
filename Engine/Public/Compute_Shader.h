#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCompute_Shader final :
    public CComponent
{
private:
    CCompute_Shader(_dev pDevice, _context pContext);
    CCompute_Shader(const CCompute_Shader& rhs);
    virtual ~CCompute_Shader() = default;

public:
    virtual HRESULT Init_Prototype(const wstring& strShaderFilePath, const string& strEntryPoint, _uint iDataSize);
    virtual HRESULT Init(void* pArg);

public:
    HRESULT Set_Shader();

    // 시작하기
    HRESULT Begin(_uint3 ThreadGroupCount);

    HRESULT Change_Value(void* pArg, _uint iDataSize, _uint iSlot = 0);

    // 입력할 텍스처
    HRESULT Bind_ShaderResourceView(ID3D11ShaderResourceView* pSRV, ID3D11UnorderedAccessView* pUAV, _uint2 iSlot);

    // 입력할 텍스처 여러개
    HRESULT Bind_SRVs(ID3D11ShaderResourceView** pSRVs, _uint iSlot, _uint iNumViews);

    HRESULT Bind_UAV(ID3D11UnorderedAccessView* pUAV, _uint iSlot);

    // 여러개 텍스처 시작하기
    HRESULT Begin_MultiSRV(_uint3 ThreadGroupCount);
private:
    ID3D11Buffer* m_pBuffer = nullptr;

    ID3DBlob* m_pBlob = nullptr;

    ID3D11ComputeShader* m_pShader = nullptr;

    ID3D11Query* m_pQuery = nullptr;

    _uint2 m_iSlot{};

    _uint m_iNumViews = 0;
public:
    static CCompute_Shader* Create(_dev pDevice, _context pContext, const wstring& strShaderFilePath, const string& strEntryPoint, _uint iDataSize = 0);
    // CComponent을(를) 통해 상속됨
    virtual CComponent* Clone(void* pArg = nullptr) override;
    virtual void Free() override;
};

END