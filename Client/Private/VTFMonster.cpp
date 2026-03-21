#include "VTFMonster.h"

CVTFMonster::CVTFMonster(_dev pDevice, _context pContext)
    :CGameObject(pDevice, pContext)
{
}

CVTFMonster::CVTFMonster(const CVTFMonster& rhs)
    :CGameObject(rhs)
{
}

HRESULT CVTFMonster::Init_Prototype()
{
    return S_OK;
}

HRESULT CVTFMonster::Init(void* pArg)
{
    if (pArg)
    {
        VTFMONSTER_DESC* pVtfMonsterDesc = reinterpret_cast<VTFMONSTER_DESC*>(pArg);

        m_strModelTag = pVtfMonsterDesc->strModelTag;
        m_pTransformCom->Set_Position(pVtfMonsterDesc->vPosition);
    }

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_pShaderCom->Set_PassIndex(0);

    random_device rand;
    m_RandomNumber = _randNum(rand());

    return S_OK;
}

void CVTFMonster::Tick(_float fTimeDelta)
{
    m_pModelCom->Set_Animation(m_Animation);
    m_pTransformCom->Gravity(fTimeDelta);
}

void CVTFMonster::Late_Tick(_float fTimeDelta)
{
    m_pModelCom->Play_Animation(fTimeDelta);

    if (m_pGameInstance->IsIn_Fov_World(m_pTransformCom->Get_CenterPos()))
    {
        m_pRendererCom->Add_RenderGroup(RG_AnimNonBlend_Instance, this);
    }
}

void CVTFMonster::Set_Damage(_int iDamage, _uint MonAttType)
{
    m_iHP -= iDamage;

    m_IsHitted = true;
}

HRESULT CVTFMonster::Render()
{
    if (FAILED(Bind_ShaderResources()))
    {
        return E_FAIL;
    }

    if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
    {
        return E_FAIL;
    }

    if (FAILED(m_pModelCom->Bind_Animation(m_pShaderCom)))
    {
        return E_FAIL;
    }

    if (FAILED(m_pModelCom->Bind_PlayAnimation(m_pShaderCom)))
    {
        return E_FAIL;
    }

    for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
        {
            _bool bFailed = true;
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

        if (FAILED(m_pShaderCom->Begin(0)))
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

HRESULT CVTFMonster::Render_Instance()
{
    if (FAILED(Bind_ShaderResources()))
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CVTFMonster::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
    {
        return E_FAIL;
    }

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VTF_Instance"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
    {
        return E_FAIL;
    }

    if (FAILED(__super::Add_Component(LEVEL_VILLAGE, m_strModelTag, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
    {
         return E_FAIL;
    }

    return S_OK;
}

HRESULT CVTFMonster::Bind_ShaderResources()
{
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

    if (true == m_pGameInstance->Get_TurnOnShadow()) {

        CASCADE_DESC Desc = m_pGameInstance->Get_CascadeDesc();

        if (FAILED(m_pShaderCom->Bind_Matrices("g_CascadeView", Desc.LightView, 3)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Bind_Matrices("g_CascadeProj", Desc.LightProj, 3)))
            return E_FAIL;

    }

    return S_OK;
}

void CVTFMonster::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pRendererCom);
}
