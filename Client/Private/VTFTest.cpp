#include "VTFTest.h"

CVTFTest::CVTFTest(_dev pDevice, _context pContext)
    :CGameObject(pDevice, pContext)
{
}

CVTFTest::CVTFTest(const CVTFTest& rhs)
    :CGameObject(rhs)
{
}

HRESULT CVTFTest::Init_Prototype()
{
    return S_OK;
}

HRESULT CVTFTest::Init(void* pArg)
{
    if (FAILED(Add_Components()))
        return E_FAIL;

    //m_pTransformCom->Set_Position(_vec3(101.f, 4.f, 108.f));
    m_pTransformCom->Set_Position(_vec3(static_cast<_float>(rand() % 100), 4.f, static_cast<_float>(rand() % 100)));

    m_Animation.iAnimIndex = rand() % 20;
    m_Animation.fAnimSpeedRatio = 1.5f;
    m_Animation.isLoop = true;

    random_device rand;
    _randNum RandomNumber(rand());
    _randFloat RandomAnimPos(0.f, 1000.f);
    m_Animation.fStartAnimPos = RandomAnimPos(RandomNumber);

    return S_OK;
}

void CVTFTest::Tick(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Down(DIK_LEFT, InputChannel::UI))
    {
        m_Animation.iAnimIndex -= 1;
    }
    else if (m_pGameInstance->Key_Down(DIK_RIGHT, InputChannel::UI))
    {
        m_Animation.iAnimIndex += 1;
    }
    else if (m_pGameInstance->Key_Down(DIK_UP, InputChannel::UI))
    {
        if(m_Animation.isLoop)
        {
            m_Animation.isLoop = false;
        }
        else if (not m_Animation.isLoop)
        {
            m_Animation.isLoop = true;
        }
    }
    else if (m_pGameInstance->Key_Down(DIK_DOWN, InputChannel::UI))
    {
        if (m_Animation.bSkipInterpolation)
        {
            m_Animation.bSkipInterpolation = false;
        }
        else if (not m_Animation.bSkipInterpolation)
        {
            m_Animation.bSkipInterpolation = true;
        }
    }
    m_pModelCom->Set_Animation(m_Animation);
    m_Animation.fStartAnimPos = 0.f;
}

void CVTFTest::Late_Tick(_float fTimeDelta)
{
    m_pModelCom->Play_Animation(fTimeDelta);

    m_pRendererCom->Add_RenderGroup(RG_AnimNonBlend_Instance, this);
}

HRESULT CVTFTest::Render()
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

HRESULT CVTFTest::Render_Instance()
{
    if (FAILED(Bind_ShaderResources()))
    {
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CVTFTest::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
    {
        return E_FAIL;
    }

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VTF_Instance"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
    {
        return E_FAIL;
    }

    m_pShaderCom->Set_PassIndex(0);

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Model_VTFTest"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
    {
         return E_FAIL;
    }

    return S_OK;
}

HRESULT CVTFTest::Bind_ShaderResources()
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

CVTFTest* CVTFTest::Create(_dev pDevice, _context pContext)
{
    CVTFTest* pInstance = new CVTFTest(pDevice, pContext);

    if (FAILED(pInstance->Init_Prototype()))
    {
        MSG_BOX("Failed to Create : CVTFTest");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CVTFTest::Clone(void* pArg)
{
    CVTFTest* pInstance = new CVTFTest(*this);

    if (FAILED(pInstance->Init(pArg)))
    {
        MSG_BOX("Failed to Clone : CVTFTest");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVTFTest::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pRendererCom);
}
