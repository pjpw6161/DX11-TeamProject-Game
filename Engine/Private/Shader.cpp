#include "Shader.h"

CShader::CShader(_dev pDevice, _context pContext)
    : CComponent(pDevice, pContext)
{
}

CShader::CShader(const CShader& rhs)
    : CComponent(rhs)
    , m_pEffect(rhs.m_pEffect)
    , m_iNumPasses(rhs.m_iNumPasses)
    , m_InputLayouts(rhs.m_InputLayouts)
{
    Safe_AddRef(m_pEffect);

    for (auto& pInputLayout : m_InputLayouts)
    {
        Safe_AddRef(pInputLayout);
    }
}

HRESULT CShader::Init_Prototype(const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
    DWORD iHlslFlag{};

#ifdef _DEBUG
    iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else  
    iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif // _DEBUG

    if (FAILED(D3DX11CompileEffectFromFile(strShaderFilePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice, &m_pEffect, nullptr)))
    {
        return E_FAIL;
    }

    ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
    if (!pTechnique)
    {
        return E_FAIL;
    }

    D3DX11_TECHNIQUE_DESC TechniqueDesc{};
    pTechnique->GetDesc(&TechniqueDesc);

    m_iNumPasses = TechniqueDesc.Passes;

    for (uint32_t i = 0; i < m_iNumPasses; i++)
    {
        ID3D11InputLayout* pInputLayout{ nullptr };

        ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(i);
        if (!pPass)
        {
            return E_FAIL;
        }

        D3DX11_PASS_DESC PassDesc{};
        pPass->GetDesc(&PassDesc);

        if (FAILED(m_pDevice->CreateInputLayout(pElements, iNumElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout)))
        {
            return E_FAIL;
        }

        m_InputLayouts.push_back(pInputLayout);
    }

    return S_OK;
}

HRESULT CShader::Init(void* pArg)
{
    return S_OK;
}

HRESULT CShader::Begin(_uint iPassIndex)
{
    ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);

    ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex(iPassIndex);
    if (!pPass)
    {
        return E_FAIL;
    }

    m_pContext->IASetInputLayout(m_InputLayouts[iPassIndex]);

    if (FAILED(pPass->Apply(0, m_pContext)))
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CShader::Bind_ShaderResourceView(const _char* pVariableName, ID3D11ShaderResourceView* pSRV)
{
    if (!m_pEffect)
    {
        return E_FAIL;
    }

    LPD3D11EFFECTVARIABLE pVariable = m_pEffect->GetVariableByName(pVariableName);
    if (!pVariable)
    {
        return E_FAIL;
    }

    LPD3D11EFFECTSHADERRESOURCEVARIABLE pShaderResourceVariable = pVariable->AsShaderResource();
    if (!pShaderResourceVariable)
    {
        return E_FAIL;
    }

    return pShaderResourceVariable->SetResource(pSRV);
}

HRESULT CShader::Bind_ShaderResourceViewArray(const _char* pVariableName, ID3D11ShaderResourceView** pSRVs, _uint iNumTextures)
{
    if (!m_pEffect)
    {
        return E_FAIL;
    }

    LPD3D11EFFECTVARIABLE pVariable = m_pEffect->GetVariableByName(pVariableName);
    if (!pVariable)
    {
        return E_FAIL;
    }

    LPD3D11EFFECTSHADERRESOURCEVARIABLE pShaderResourceVariable = pVariable->AsShaderResource();
    if (!pShaderResourceVariable)
    {
        return E_FAIL;
    }

    return pShaderResourceVariable->SetResourceArray(pSRVs, 0, iNumTextures);
}

HRESULT CShader::Bind_Matrix(const _char* pVariableName, const _mat& Matrix)
{
    if (!m_pEffect)
    {
        return E_FAIL;
    }
    
    LPD3D11EFFECTVARIABLE pVariable = m_pEffect->GetVariableByName(pVariableName);
    if (!pVariable)
    {
        return E_FAIL;
    }

    LPD3D11EFFECTMATRIXVARIABLE pMatrixVariable = pVariable->AsMatrix();
    if (!pMatrixVariable)
    {
        return E_FAIL;
    }

    return pMatrixVariable->SetMatrix((_float*)&Matrix);
}

HRESULT CShader::Bind_Matrices(const _char* pVariableName, const _mat* pMatrices, _uint iNumMatrices)
{
    if (!m_pEffect)
    {
        return E_FAIL;
    }

    LPD3D11EFFECTVARIABLE pVariable = m_pEffect->GetVariableByName(pVariableName);
    if (!pVariable)
    {
        return E_FAIL;
    }

    LPD3D11EFFECTMATRIXVARIABLE pMatrixVariable = pVariable->AsMatrix();
    if (!pMatrixVariable)
    {
        return E_FAIL;
    }

    return pMatrixVariable->SetMatrixArray((_float*)pMatrices, 0, iNumMatrices);
}

HRESULT CShader::Bind_RawValue(const _char* pVariableName, const void* pData, _uint iDataSize)
{
    if (!m_pEffect)
    {
        return E_FAIL;
    }

    LPD3D11EFFECTVARIABLE pVariable = m_pEffect->GetVariableByName(pVariableName);
    if (!pVariable)
    {
        return E_FAIL;
    }

    return pVariable->SetRawValue(pData, 0, iDataSize);
}

CShader* CShader::Create(_dev pDevice, _context pContext, const wstring& strShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
    CShader* pInstance = new CShader(pDevice, pContext);

    if (FAILED(pInstance->Init_Prototype(strShaderFilePath, pElements, iNumElements)))
    {
        MSG_BOX("Failed to Create : CShader");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CShader::Clone(void* pArg)
{
    CShader* pInstance = new CShader(*this);

    if (FAILED(pInstance->Init(pArg)))
    {
        MSG_BOX("Failed to Clone : CShader");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CShader::Free()
{
    __super::Free();

    for (auto& pInputLayout : m_InputLayouts)
    {
        Safe_Release(pInputLayout);
    }

    m_InputLayouts.clear();

    Safe_Release(m_pEffect);
}
