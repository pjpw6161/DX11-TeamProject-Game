#include "Select_Npc.h"

CSelect_Npc::CSelect_Npc(_dev pDevice, _context pContext)
    : CGameObject(pDevice, pContext)
{
}

CSelect_Npc::CSelect_Npc(const CSelect_Npc& rhs)
    : CGameObject(rhs)
{
}

HRESULT CSelect_Npc::Init_Prototype()
{
    return S_OK;
}

HRESULT CSelect_Npc::Init(void* pArg)
{
    ObjectInfo* Info = (ObjectInfo*)pArg;
    m_strModelTag = Info->strPrototypeTag;

    Add_Components();

    m_pTransformCom->Set_State(State::Pos, Info->vPos);
  

    m_Animation.iAnimIndex = 0;
    m_Animation.isLoop = true;
    m_Animation.bSkipInterpolation = true;

    return S_OK;
}

void CSelect_Npc::Tick(_float fTimeDelta)
{
    m_pTransformCom->Set_OldMatrix();

    m_pModelCom->Set_Animation(m_Animation);

}

void CSelect_Npc::Late_Tick(_float fTimeDelta)
{
    m_pModelCom->Play_Animation(fTimeDelta);
    m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);

}

HRESULT CSelect_Npc::Render()
{
    if (FAILED(Bind_ShaderResources()))
    {
        return E_FAIL;
    }

    for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
        {
        }

        _bool HasNorTex{};
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
        {
            HasNorTex = false;
        }
        else
        {
            HasNorTex = true;
        }

        _bool HasMaskTex{};
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MaskTexture", i, TextureType::Shininess)))
        {
            HasMaskTex = false;
        }
        else
        {
            HasMaskTex = true;
        }

        if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
        {
            return E_FAIL;
        }

        if (FAILED(m_pShaderCom->Bind_RawValue("g_HasMaskTex", &HasMaskTex, sizeof _bool)))
        {
            return E_FAIL;
        }

        if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
        {
            return E_FAIL;
        }

        if (FAILED(m_pShaderCom->Begin(AnimPass_Default)))
        {
            return E_FAIL;
        }

        if (FAILED(m_pModelCom->Render(i)))
        {
            return E_FAIL;
        }
    }

    return S_OK;
}

HRESULT CSelect_Npc::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
    {
        return E_FAIL;
    }

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
    {
        return E_FAIL;
    }

    if (FAILED(__super::Add_Component(LEVEL_SELECT, m_strModelTag, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CSelect_Npc::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
    {
        return E_FAIL;
    }

    if (FAILED(m_pTransformCom->Bind_OldWorldMatrix(m_pShaderCom, "g_OldWorldMatrix")))
    {
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_Matrix("g_OldViewMatrix", m_pGameInstance->Get_OldViewMatrix())))
    {
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(TransformType::View))))
    {
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(TransformType::Proj))))
    {
        return E_FAIL;
    }

    if (FAILED(m_pShaderCom->Bind_RawValue("g_CamNF", &m_pGameInstance->Get_CameraNF(), sizeof _float2)))
    {
        return E_FAIL;
    }

    return S_OK;
}

CSelect_Npc* CSelect_Npc::Create(_dev pDevice, _context pContext)
{
    CSelect_Npc* pInstance = new CSelect_Npc(pDevice, pContext);

    if (FAILED(pInstance->Init_Prototype()))
    {
        MSG_BOX("Failed to Create : CSelect_Npc");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSelect_Npc::Clone(void* pArg)
{
    CSelect_Npc* pInstance = new CSelect_Npc(*this);

    if (FAILED(pInstance->Init(pArg)))
    {
        MSG_BOX("Failed to Clone : CSelect_Npc");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSelect_Npc::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pRendererCom);
    Safe_Release(m_pShaderCom);
}
