#include "TestTree.h"

CTestTree::CTestTree(_dev pDevice, _context pContext)
    : CGameObject(pDevice, pContext)
{
}

CTestTree::CTestTree(const CTestTree& rhs)
    : CGameObject(rhs)
{
}

HRESULT CTestTree::Init_Prototype()
{
    return S_OK;
}

HRESULT CTestTree::Init(void* pArg)
{
    return S_OK;
}

void CTestTree::Tick(_float fTimeDelta)
{
}

void CTestTree::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);
}

HRESULT CTestTree::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

    return S_OK;
}


HRESULT CTestTree::Render_Reflection(_float4 vClipPlane)
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

    return S_OK;
}

HRESULT CTestTree::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(TransformType::View))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(TransformType::Proj))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_CamNF", &m_pGameInstance->Get_CameraNF(), sizeof(_float2))))
		return E_FAIL;

	return S_OK;
}

HRESULT CTestTree::Add_Component()
{
	return S_OK;
}

CTestTree* CTestTree::Create(_dev pDevice, _context pContext)
{
	CTestTree* pInstance = new CTestTree(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CTestTree");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CTestTree::Clone(void* pArg)
{
	CTestTree* pInstance = new CTestTree(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CTestTree");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTestTree::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
