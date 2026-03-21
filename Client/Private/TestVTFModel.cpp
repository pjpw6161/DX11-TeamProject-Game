#include "TestVTFModel.h"

CTestVTFModel::CTestVTFModel(_dev pDevice, _context pContext)
    : CGameObject(pDevice, pContext)
{
}

CTestVTFModel::CTestVTFModel(const CTestVTFModel& rhs)
    : CGameObject(rhs)
{
}

HRESULT CTestVTFModel::Init_Prototype()
{
    return S_OK;
}

HRESULT CTestVTFModel::Init(void* pArg)
{
    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Place_PartModels()))
        return E_FAIL;

    m_pTransformCom->Set_State(State::Pos, _vec4(0.f, 0.f, 0.f, 1.f));

    m_iHP = 10;

    m_Animation.iAnimIndex = 0;
    m_Animation.isLoop = true;
    m_Animation.bSkipInterpolation = false;
    m_Animation.fAnimSpeedRatio = 2.f;

    m_pModelCom->Set_Animation(m_Animation);

    return S_OK;
}

void CTestVTFModel::Tick(_float fTimeDelta)
{
    // 모션블러용으로 이전프레임 WorldMatrix를 저장합니다
    m_OldWorldMatrix = m_pTransformCom->Get_World_Matrix();

    if (m_pGameInstance->Key_Pressing(DIK_W)) {
        _vec4 vPos = m_pTransformCom->Get_State(State::Pos);
        vPos += _vec4(1.f, 0.f, 0.f, 0.f) * fTimeDelta;
        m_pTransformCom->Set_State(State::Pos, vPos);
    }

    if (m_pGameInstance->Key_Down(DIK_L)) {
        m_UsingMotionBlur = !m_UsingMotionBlur;
        m_pModelCom->Set_UsingMotionBlur(m_UsingMotionBlur);
    }

    if (m_pGameInstance->Key_Down(DIK_M)) {
        m_Animation.iAnimIndex++;
        m_pModelCom->Set_Animation(m_Animation);
    }

    if (m_pGameInstance->Key_Down(DIK_N)) {
        m_Animation.iAnimIndex--;
        m_pModelCom->Set_Animation(m_Animation);
    }

    if (m_UsingMotionBlur)
        hi = 1;
    else
        hi = 0;
}

void CTestVTFModel::Late_Tick(_float fTimeDelta)
{
    m_pModelCom->Play_Animation(fTimeDelta);
    m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);

}

HRESULT CTestVTFModel::Render()
{
    if (FAILED(Bind_ShaderResources()))
    {
        return E_FAIL;
    }

    for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
        {
            continue;
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

        _bool HasSpecTex{};
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_SpecTexture", i, TextureType::Shininess)))
        {
            HasSpecTex = false;
        }
        else
        {
            HasSpecTex = true;
        }

        if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
        {
            return E_FAIL;
        }

        if (FAILED(m_pShaderCom->Bind_RawValue("g_HasSpecTex", &HasSpecTex, sizeof _bool)))
        {
            return E_FAIL;
        }

        if (FAILED(m_pShaderCom->Begin(hi)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Render(i)))
            return E_FAIL;
    }

    for (size_t i = 0; i < m_pModelCom->Get_NumPartType(); i++)
    {
        for (size_t j = 0; j < m_pModelCom->Get_NumPart(i); j++)
        {
            // 해당 파츠가 그려야하는 지 알아보기
            if (false == m_pModelCom->Get_PartIsRender(i, j))
                continue;

			for (size_t k = 0; k < m_pModelCom->Get_Num_PartMeshes(i, j); k++)
            {
                if (FAILED(m_pModelCom->Bind_Part_Material(m_pShaderCom, "g_DiffuseTexture", TextureType::Diffuse, i, j, k)))
                    continue;

                _bool HasNorTex{};
                if (FAILED(m_pModelCom->Bind_Part_Material(m_pShaderCom, "g_NormalTexture", TextureType::Normals, i, j, k)))
                {
                    HasNorTex = false;
                }
                else
                {
                    HasNorTex = true;
                }

                _bool HasSpecTex{};
                if (FAILED(m_pModelCom->Bind_Part_Material(m_pShaderCom, "g_SpecTexture", TextureType::Shininess, i, j, k)))
                {
                    HasSpecTex = false;
                }
                else
                {
                    HasSpecTex = true;
                }

                if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
                {
                    return E_FAIL;
                }

                if (FAILED(m_pShaderCom->Bind_RawValue("g_HasSpecTex", &HasSpecTex, sizeof _bool)))
                {
                    return E_FAIL;
                }

                if (FAILED(m_pShaderCom->Begin(hi)))
                    return E_FAIL;

				if (FAILED(m_pModelCom->Render_Part(i, j, k)))
                    return E_FAIL;
            }
        }
    }

    return S_OK;
}

HRESULT CTestVTFModel::Add_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_SELECT, TEXT("Prototype_Component_Shader_RTVTF"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_SELECT, TEXT("Prototype_Model_RTVTFRabbit"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CTestVTFModel::Bind_ShaderResources()
{
    // WorldMatrix 바인드
    if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    // ViewMatrix 바인드
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(TransformType::View))))
        return E_FAIL;

    // ProjMatrix 바인드
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(TransformType::Proj))))
        return E_FAIL;

    // 카메라 Far 바인드
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &m_pGameInstance->Get_CameraNF().y, sizeof _float)))
        return E_FAIL;

    // 모션블러용 이전프레임 WorldMatrix 바인드
    if (FAILED(m_pShaderCom->Bind_Matrix("g_OldWorldMatrix", m_OldWorldMatrix)))
        return E_FAIL;

    // 모션블러용 이전프레임 ViewMatrix 바인드
    if (FAILED(m_pShaderCom->Bind_Matrix("g_OldViewMatrix", m_pGameInstance->Get_OldViewMatrix_vec4x4())))
        return E_FAIL;
    
    // 뼈 바인드
    if (FAILED(m_pModelCom->Bind_Bone(m_pShaderCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CTestVTFModel::Place_PartModels()
{
    CRealtimeVTFModel::PART_DESC Desc{};
    Desc.ePartType = hair; // enum 값으로 파츠타입을 정해서 주세용
    Desc.FileName = "R_2005_BD.ao";

    // 파츠타입을 정한후 bool 값은 처음에 랜더할것만 true 주고 랜더 안할것들은 안주면 자동으로 false 들어갑니다
    if (FAILED(m_pModelCom->Place_Parts(Desc, true)))
        return E_FAIL;

    Desc.FileName = "R_3009_BD.ao";

    if (FAILED(m_pModelCom->Place_Parts(Desc)))
        return E_FAIL;

    Desc.FileName = "R_5001_BD.ao";

    if (FAILED(m_pModelCom->Place_Parts(Desc)))
        return E_FAIL;

    Desc.FileName = "R_5001_HD.ao";

    if (FAILED(m_pModelCom->Place_Parts(Desc)))
        return E_FAIL;

    Desc.FileName = "R_9000_FC.ao";

    if (FAILED(m_pModelCom->Place_Parts(Desc)))
        return E_FAIL;

    Desc.FileName = "R_9000_HR.ao";

    if (FAILED(m_pModelCom->Place_Parts(Desc)))
        return E_FAIL;

    Desc.FileName = "R_9000_S_HR.ao";

    if (FAILED(m_pModelCom->Place_Parts(Desc)))
        return E_FAIL;

    Desc.FileName = "R_9001_FC.ao";

    if (FAILED(m_pModelCom->Place_Parts(Desc)))
        return E_FAIL;

    Desc.FileName = "R_9001_HR.ao";

    if (FAILED(m_pModelCom->Place_Parts(Desc)))
        return E_FAIL;

    Desc.FileName = "R_9002_HR.ao";

    if (FAILED(m_pModelCom->Place_Parts(Desc)))
        return E_FAIL;

    Desc.FileName = "R_9003_FC.ao";

    if (FAILED(m_pModelCom->Place_Parts(Desc)))
        return E_FAIL;

    Desc.FileName = "R_9004_FC.ao";

    if (FAILED(m_pModelCom->Place_Parts(Desc)))
        return E_FAIL;

    Desc.FileName = "R_9004_HR.ao";

    if (FAILED(m_pModelCom->Place_Parts(Desc)))
        return E_FAIL;

    Desc.FileName = "R_9006_HR.ao";

    if (FAILED(m_pModelCom->Place_Parts(Desc)))
        return E_FAIL;

    Desc.FileName = "R_9008_HR.ao";

    if (FAILED(m_pModelCom->Place_Parts(Desc)))
        return E_FAIL;

    Desc.FileName = "R_9012_HR.ao";

    if (FAILED(m_pModelCom->Place_Parts(Desc)))
        return E_FAIL;

    Desc.FileName = "RA_2004_WP.ao";

    if (FAILED(m_pModelCom->Place_Parts(Desc)))
        return E_FAIL;

    Desc.FileName = "RA_3007_WP.ao";

    if (FAILED(m_pModelCom->Place_Parts(Desc)))
        return E_FAIL;

    Desc.FileName = "RS_2004_WP.ao";

    if (FAILED(m_pModelCom->Place_Parts(Desc)))
        return E_FAIL;

    Desc.FileName = "RS_3007_WP.ao";

    if (FAILED(m_pModelCom->Place_Parts(Desc)))
        return E_FAIL;

    Desc.FileName = "RS_5001_WP.ao";

    if (FAILED(m_pModelCom->Place_Parts(Desc)))
        return E_FAIL;


    return S_OK;
}

CTestVTFModel* CTestVTFModel::Create(_dev pDevice, _context pContext)
{
    CTestVTFModel* pInstance = new CTestVTFModel(pDevice, pContext);

    if (FAILED(pInstance->Init_Prototype()))
    {
        MSG_BOX("Failed to Create : CTestVTFModel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTestVTFModel::Clone(void* pArg)
{
    CTestVTFModel* pInstance = new CTestVTFModel(*this);

    if (FAILED(pInstance->Init(pArg)))
    {
        MSG_BOX("Failed to Clone : CTestVTFModel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTestVTFModel::Free()
{
    __super::Free();

    Safe_Release(m_pModelCom);
    Safe_Release(m_pRendererCom);
    Safe_Release(m_pShaderCom);
}
