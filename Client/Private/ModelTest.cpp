#include "ModelTest.h"

CModelTest::CModelTest(_dev pDevice, _context pContext)
    : CMonster(pDevice, pContext)
{
}

CModelTest::CModelTest(const CModelTest& rhs)
    : CMonster(rhs)
{
}

HRESULT CModelTest::Init_Prototype()
{
    return S_OK;
}

HRESULT CModelTest::Init(void* pArg)
{
    m_strModelTag = TEXT("Prototype_Model_ModelTest");

    if (FAILED(__super::Add_Components()))
    {
        return E_FAIL;
    }

	//m_pTransformCom->Set_State(State::Pos, _vec4(static_cast<_float>(rand() % 30) + 60.f, 0.f, static_cast<_float>(rand() % 30) + 60.f, 1.f));
	m_pTransformCom->Set_State(State::Pos, _vec4(71.f, 0.f, 110.f, 1.f));

    m_Animation.iAnimIndex = 0;
    m_Animation.isLoop = true;
    m_Animation.bSkipInterpolation = true;
    m_Animation.fAnimSpeedRatio = 2.f;

    m_pTransformCom->Set_Speed(5.f);

    return S_OK;
}

void CModelTest::Tick(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Down(DIK_RBRACKET))
    {
        m_Animation.iAnimIndex += 1;
        if (m_Animation.iAnimIndex >= m_pModelCom->Get_NumAnim())
        {
            m_Animation.iAnimIndex -= 1;
        }
    }
    if (m_pGameInstance->Key_Down(DIK_LBRACKET))
    {
        if (m_Animation.iAnimIndex != 0)
        {
            m_Animation.iAnimIndex -= 1;
        }
    }

    // ÀÓ½Ã
    if (m_pGameInstance->Key_Pressing(DIK_UP))
    {
        m_pTransformCom->Go_Straight(fTimeDelta);
    }
	if (m_pGameInstance->Key_Pressing(DIK_DOWN))
	{
		m_pTransformCom->Go_Backward(fTimeDelta);
	}
	if (m_pGameInstance->Key_Pressing(DIK_LEFT))
	{
		m_pTransformCom->Go_Left(fTimeDelta);
	}
	if (m_pGameInstance->Key_Pressing(DIK_RIGHT))
	{
		m_pTransformCom->Go_Right(fTimeDelta);
	}
    //

    m_pModelCom->Set_Animation(m_Animation);
}

void CModelTest::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);
}

HRESULT CModelTest::Render()
{
    __super::Render();

    //wstring strTotalAnim = L"¾Ö´Ô °³¼ö : " + to_wstring(m_pModelCom->Get_NumAnim());
    //wstring strCurrAnim = L"ÇöÀç ¾Ö´Ô : " + to_wstring(m_Animation.iAnimIndex);

    //m_pGameInstance->Render_Text(L"Font_Dialogue", strTotalAnim, _vec2(100.f, 650.f), 0.5f, _vec4(1.f, 1.f, 1.f, 1.f));
    //m_pGameInstance->Render_Text(L"Font_Dialogue", strCurrAnim, _vec2(100.f, 680.f), 0.5f, _vec4(1.f, 1.f, 1.f, 1.f));

    return S_OK;
}

HRESULT CModelTest::Add_Collider()
{
    return S_OK;
}

void CModelTest::Update_Collider()
{
}

CModelTest* CModelTest::Create(_dev pDevice, _context pContext)
{
    CModelTest* pInstance = new CModelTest(pDevice, pContext);

    if (FAILED(pInstance->Init_Prototype()))
    {
        MSG_BOX("Failed to Create : CModelTest");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CModelTest::Clone(void* pArg)
{
    CModelTest* pInstance = new CModelTest(*this);

    if (FAILED(pInstance->Init(pArg)))
    {
        MSG_BOX("Failed to Clone : CModelTest");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CModelTest::Free()
{
    __super::Free();
}
