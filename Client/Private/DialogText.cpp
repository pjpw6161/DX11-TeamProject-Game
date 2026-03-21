#include "DialogText.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "UI_Manager.h"

CDialogText::CDialogText(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CDialogText::CDialogText(const CDialogText& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CDialogText::Init_Prototype()
{
	return S_OK;
}

HRESULT CDialogText::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}


	m_strText = ((DIALOGTEXT_DESC*)pArg)->strText;
	m_eLevel = ((DIALOGTEXT_DESC*)pArg)->eLevelID;
	m_vTextPos = ((DIALOGTEXT_DESC*)pArg)->vTextPos;


	m_fX = (_float)g_ptCenter.x;
	m_fY = (_float)g_ptCenter.y;

	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY;
	m_fDepth = ((DIALOGTEXT_DESC*)pArg)->fDepth;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);


	return S_OK;
}

void CDialogText::Tick(_float fTimeDelta)
{
	m_fPeriod += fTimeDelta;
	if (m_fPeriod >= 0.08f && m_iIndex < m_strText.size())
	{
		m_fPeriod = 0.f;
		m_iIndex += 1;
	}
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
}

void CDialogText::Late_Tick(_float fTimeDelta)
{

	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CDialogText::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}	
	wstring strPrintText = m_strText.substr(0, m_iIndex);
	m_pGameInstance->Render_Text(L"Font_Malang", strPrintText ,_vec2(m_vTextPos.x + 0.5f, m_vTextPos.y), 0.4f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", strPrintText, _vec2(m_vTextPos.x, m_vTextPos.y + 0.5f), 0.4f, _vec4(0.f, 0.f, 0.f, 1.f));

	m_pGameInstance->Render_Text(L"Font_Malang", strPrintText, m_vTextPos, 0.4f);

	return S_OK;
}

HRESULT CDialogText::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CDialogText::Bind_ShaderResources()
{
	return S_OK;
}

CDialogText* CDialogText::Create(_dev pDevice, _context pContext)
{
	CDialogText* pInstance = new CDialogText(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CDialogText");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDialogText::Clone(void* pArg)
{
	CDialogText* pInstance = new CDialogText(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CDialogText");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDialogText::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
}
