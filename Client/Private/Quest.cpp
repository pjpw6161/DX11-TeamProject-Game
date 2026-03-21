#include "Quest.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "UI_Manager.h"

CQuest::CQuest(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CQuest::CQuest(const CQuest& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CQuest::Init_Prototype()
{
	return S_OK;
}

HRESULT CQuest::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}
	

	m_fSizeX = 30.f;
	m_fSizeY = 50.f;

	m_fX = 1220.f;
	m_fY = 120.f;

	m_fDepth = (_float)D_SCREEN / (_float)D_END;

	m_fMin = m_fY - m_fSizeY / 2.f;
	m_fMax = m_fY + 50.f;
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CQuest::Tick(_float fTimeDelta)
{

	if (CUI_Manager::Get_Instance()->Showing_FullScreenUI())
	{
		return;
	}
	if (m_vecQuest.empty())
	{
		m_bNewQuestIn = false;
	}


	CUI_Manager::Get_Instance()->Set_Picking_UI(false);
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	RECT rectUI = {
			  (LONG)(m_fX - m_fSizeX * 0.5f),
			  (LONG)(m_fY - m_fSizeY * 0.5f),
			  (LONG)(m_fX + m_fSizeX * 0.5f),
			  (LONG)(m_fY + m_fSizeY * 0.5f)
	};

	if (TRUE == PtInRect(&rectUI, ptMouse))
	{
		if (m_isActive && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
		{
			m_pGameInstance->Play_Sound(TEXT("Quest_Click"));
			Sort_Quest();
			m_isActive = false;
		}
		else if (!m_isActive && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
		{
			m_pGameInstance->Play_Sound(TEXT("Quest_Click"));
			Sort_Quest();
			m_bNewQuestIn = false;
			m_isActive = true;
		}
	}

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	m_pSymbol->Tick(fTimeDelta);

	if (m_isActive)
	{

		RECT rectUI = {
		  (LONG)(m_fX - 200.f * 0.5f),
		  (LONG)(m_fY - 200.f * 0.5f),
		  (LONG)(m_fX + 200.f * 0.5f),
		  (LONG)(m_fY + 200.f * 0.5f)
		};

		if (TRUE == PtInRect(&rectUI, ptMouse))
		{
			CUI_Manager::Get_Instance()->Set_Picking_UI(true);
			if (m_vecQuest.empty())
			{
				return;
			}

			if (m_vecQuest.back()->Get_Position().y + m_fSizeY / 2.f <= m_fMax && m_vecQuest.front()->Get_Position().y - m_fSizeY / 2.f >= m_fMin)
			{
				return;
			}
			if (m_vecQuest.back()->Get_Position().y + m_fSizeY / 2.f < m_fMax)
			{
				_uint iSize = m_vecQuest.size();
				for (auto& iter : m_vecQuest)
				{
					iSize--;
					_float fY = (m_fMax - m_fSizeY / 2.f) - (iSize * m_fSizeY) - (iSize * 4.f);
					iter->Set_Position(_float2(iter->Get_Position().x, fY));

				}
				return;
			}
			if (m_vecQuest.front()->Get_Position().y - m_fSizeY / 2.f > m_fMin)
			{
				Sort_Quest();
				return;
			}

			if (m_pGameInstance->Get_MouseMove(MouseState::wheel) > 0)
			{
				for (auto& iter : m_vecQuest)
				{
					iter->Set_Position(_vec2(iter->Get_Position().x, iter->Get_Position().y - 5.f));
				}
			}

			if (m_pGameInstance->Get_MouseMove(MouseState::wheel) < 0)
			{

				for (auto& iter : m_vecQuest)
				{
					iter->Set_Position(_vec2(iter->Get_Position().x, iter->Get_Position().y + 5.f));
				}
			}
		}
	}
	for (auto& iter : m_vecQuest)
	{
		iter->Tick(fTimeDelta);
	}
}

void CQuest::Late_Tick(_float fTimeDelta)
{
	if (CUI_Manager::Get_Instance()->Showing_FullScreenUI())
	{
		return;
	}

	for (_uint i = 0; i < m_vecQuest.size(); i++)
	{
		m_vecQuest[i]->Late_Tick(fTimeDelta);
		if (!m_isActive && i <= 0)
			break;
	}

	if (m_isActive)
	{
		m_pButton->Late_Tick(fTimeDelta);
	}

	if (m_bNewQuestIn)
	{
		m_pNotify->Late_Tick(fTimeDelta);
	}

	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
	m_pSymbol->Late_Tick(fTimeDelta);
}

HRESULT CQuest::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(VTPass_UI)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CQuest::Set_Quest(CQuestBox::QUESTBOX_DESC& QuestBoxDesc)
{
	_float fY = m_fY + (m_vecQuest.size() * m_fSizeX) + (m_vecQuest.size() * 1.f);
	QuestBoxDesc.vPosition = _float2(m_fX - m_fSizeX / 2.f, fY);
	QuestBoxDesc.fMax = m_fMax;
	QuestBoxDesc.fMin = m_fMin;
	CQuestBox* pQuestBox = (CQuestBox*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_QuestBox"), &QuestBoxDesc);
	if (not pQuestBox)
	{
		return E_FAIL;
	}
	if (pQuestBox->IsMain())
	{
		m_vecQuest.insert(m_vecQuest.begin(), pQuestBox);
	}
	else
	{
		m_vecQuest.push_back(pQuestBox);
	}

	m_bNewQuestIn = true;

	Sort_Quest();
	
	return S_OK;
}

void CQuest::Sort_Quest()
{
	if (m_vecQuest.empty())
	{
		return;
	}

	_uint iSize = 0;
	for (auto& iter : m_vecQuest)
	{
		_float fY = m_fY + (iSize * m_fSizeY) + (iSize * 4.f);
		iter->Set_Position(_float2(iter->Get_Position().x, fY));
		iSize++;
	}
}

_bool CQuest::Update_Quest(const wstring& strQuest)
{
	_bool bUpdateQuest = false;
	_uint i = 0;
	for (i = 0; i < m_vecQuest.size(); i++)
	{
		if (m_vecQuest[i]->Get_QuestName() == strQuest)
		{
			bUpdateQuest = m_vecQuest[i]->Update_Quest();
			break;
		}
	}

	if (bUpdateQuest)
	{
		Safe_Release(m_vecQuest[i]);
		m_vecQuest.erase(m_vecQuest.begin() + i);
		Sort_Quest();
		return true;
	}

	return false;
}

HRESULT CQuest::Add_Parts()
{
	CTextButton::TEXTBUTTON_DESC Button = {};
	Button.eLevelID = LEVEL_STATIC;
	Button.fDepth = m_fDepth - 0.02f;
	Button.strText = TEXT("");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_ShadeButton");
	Button.vPosition = _vec2(m_fX, m_fY);
	Button.vSize = _vec2(m_fSizeX, m_fSizeY);

	m_pButton = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pButton)
	{
		return E_FAIL;
	}

	Button.eLevelID = LEVEL_STATIC;
	Button.fDepth = m_fDepth - 0.03f;
	Button.strText = TEXT("");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Notify");
	Button.vPosition = _vec2(m_fX + 7.f, m_fY - 14.f);
	Button.vSize = _vec2(40.f, 40.f);

	m_pNotify = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pNotify)
	{
		return E_FAIL;
	}



	Button.eLevelID = LEVEL_STATIC;
	Button.fDepth = m_fDepth - 0.01f;
	Button.strText = TEXT("");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_questbox");
	Button.vPosition = _vec2(m_fX, m_fY);
	Button.vSize = _vec2(30.f, 30.f);

	m_pSymbol  = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pSymbol)
	{
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CQuest::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_QuestAlpha"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CQuest::Bind_ShaderResources()
{
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_ViewMatrix))
		|| FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
	{
		return E_FAIL;
	}

	return S_OK;
}

CQuest* CQuest::Create(_dev pDevice, _context pContext)
{
	CQuest* pInstance = new CQuest(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CQuest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CQuest::Clone(void* pArg)
{
	CQuest* pInstance = new CQuest(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CQuest");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQuest::Free()
{
	__super::Free();

	for (auto& iter : m_vecQuest)
	{
		Safe_Release(iter);
	}
	m_vecQuest.clear();

	Safe_Release(m_pSymbol);
	Safe_Release(m_pButton);
	Safe_Release(m_pNotify);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}