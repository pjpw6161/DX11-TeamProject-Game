#include "ItemInfo.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "NineSlice.h"
#include "UI_Manager.h"
#include "SummonWindow.h"
#include "Item.h"

CItemInfo::CItemInfo(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CItemInfo::CItemInfo(const CItemInfo& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CItemInfo::Init_Prototype()
{
	return S_OK;
}

HRESULT CItemInfo::Init(void* pArg)
{

	m_eItemDesc = ((ITEMINFO_DESC*)pArg)->eItemDesc;
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = 300.f;
	m_fSizeY = 300.f;

	m_fX = (_float)g_iWinSizeX / 2.f;
	m_fY = (_float)g_iWinSizeY / 2.f;

	m_fDepth = (((ITEMINFO_DESC*)pArg)->fDepth) - 0.05f;
	m_eItemDesc = ((ITEMINFO_DESC*)pArg)->eItemDesc;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	

	m_rcRect = {
		  (LONG)(m_fX - m_fSizeX * 0.5f),
		  (LONG)(m_fY - m_fSizeY * 0.5f),
		  (LONG)(m_fX + m_fSizeX * 0.5f),
		  (LONG)(m_fY + m_fSizeY * 0.5f)
	};

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}
	return S_OK;
}

void CItemInfo::Tick(_float fTimeDelta)
{
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);


	if (PtInRect(&m_pExitButton->Get_Rect(), ptMouse) && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
	{
		m_isDead = true;
		return;
	}

	if (PtInRect(&m_SelectButton->Get_InitialRect(), ptMouse))
	{
		m_SelectButton->Set_Size(140.f, 80.f, 0.3f);
		if (m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
		{
			_bool isOtherItemExist = false;
			if (m_eItemDesc.iItemType == (_uint)ITEM_BOW)
			{
				if (FAILED(CUI_Manager::Get_Instance()->Set_WearableItem(W_BOW, m_eItemDesc)))
				{
					m_isDead = true;
					return;
				}
				isOtherItemExist = true;
			}
			else if (m_eItemDesc.iItemType == (_uint)ITEM_SWORD)
			{
				if (FAILED(CUI_Manager::Get_Instance()->Set_WearableItem(W_SWORD, m_eItemDesc)))
				{
					m_isDead = true;
					return;
				}
				isOtherItemExist = true;
			}
			else if (m_eItemDesc.iItemType == (_uint)ITEM_BODY)
			{
				if (FAILED(CUI_Manager::Get_Instance()->Set_WearableItem(W_CHEST, m_eItemDesc)))
				{
					m_isDead = true;
					return;
				}
				isOtherItemExist = true;
			}
			else if (m_eItemDesc.iItemType == (_uint)ITEM_TOP)
			{
				if (FAILED(CUI_Manager::Get_Instance()->Set_WearableItem(W_TOP, m_eItemDesc)))
				{
					m_isDead = true;
					return;
				}
				isOtherItemExist = true;
			}
			if (m_eItemDesc.eItemUsage == IT_VEHICLECARD)
			{
				CSummonWindow::SUMMON_DESC Desc{};
				Desc.iItemTier = (ITEM_TIER)m_eItemDesc.iItemTier;
				if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_UI"), TEXT("Prototype_GameObject_SummonWindow"), &Desc)))
				{
					m_isDead = true;
					return;
				}
				isOtherItemExist = true;
			}
			if (isOtherItemExist)
			{
				CUI_Manager::Get_Instance()->Delete_Item((INVEN_TYPE)m_eItemDesc.iInvenType, m_eItemDesc.strName);
			}
			m_isDead = true;
			return;
		}
	}
	else
	{
		m_SelectButton->Set_Size(150.f, 100.f, 0.35f);
	}

	m_SelectButton->Tick(fTimeDelta);
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
}

void CItemInfo::Late_Tick(_float fTimeDelta)
{
	m_pItemTex->Late_Tick(fTimeDelta);
	m_pExitButton->Late_Tick(fTimeDelta);
	m_pBackGround->Late_Tick(fTimeDelta);
	m_SelectButton->Late_Tick(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
}

HRESULT CItemInfo::Render()
{
	_vec4 vPointColor{};
	wstring strTier{};
	if (m_eItemDesc.iItemTier == (_uint)TIER_COMMON)
	{
		vPointColor = _vec4(1.f, 1.f, 1.f, 1.f);
		strTier = TEXT("일반");
	}
	else if (m_eItemDesc.iItemTier == (_uint)TIER_UNCOMMON)
	{
		vPointColor = _vec4(0.f, 0.7f, 0.f, 1.f);
		strTier = TEXT("고급");
	}
	else if (m_eItemDesc.iItemTier == (_uint)TIER_RARE)
	{
		vPointColor = _vec4(0.14f, 0.41f, 1.f, 1.f);
		strTier = TEXT("희귀");
	}
	else if (m_eItemDesc.iItemTier == (_uint)TIER_UNIQUE)
	{
		vPointColor = _vec4(0.47f, 0.24f, 1.f, 1.f);
		strTier = TEXT("영웅");
	}
	else if (m_eItemDesc.iItemTier == (_uint)TIER_LEGENDARY)
	{
		vPointColor = _vec4(0.7f, 0.45f, 0.1f, 1.f);
		strTier = TEXT("신화");
	}
	m_pGameInstance->Render_Text(L"Font_Malang", strTier + TEXT(" 등급"), _vec2(m_fX + 130.f + 1.f, m_fY - 123.f), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", strTier + TEXT(" 등급"), _vec2(m_fX + 130.f, m_fY - 123.f + 1.f), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f));
	m_pGameInstance->Render_Text(L"Font_Malang", strTier + TEXT(" 등급"), _vec2(m_fX + 130.f, m_fY - 123.f), 0.3f, vPointColor);
	

	m_pGameInstance->Render_Text(L"Font_Malang", m_eItemDesc.strName, _vec2(m_fX - 150.f + 1.f, m_fY - 180.f), 0.5f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", m_eItemDesc.strName, _vec2(m_fX - 150.f, m_fY - 180.f + 1.f), 0.5f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
	m_pGameInstance->Render_Text(L"Font_Malang", m_eItemDesc.strName, _vec2(m_fX - 150.f, m_fY - 180.f), 0.5f, vPointColor, 0.f, true);

	wstring str{};
	_vec2 vTextPos = _vec2(m_fX - 60.f, m_fY - 130.f);
	if (m_eItemDesc.iItemType ==ITEM_TOP)
	{
		m_pGameInstance->Render_Text(L"Font_Malang", TEXT("회피율 증가"), _vec2(vTextPos.x + 1.f, vTextPos.y), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
		m_pGameInstance->Render_Text(L"Font_Malang", TEXT("회피율 증가"), _vec2(vTextPos.x, vTextPos.y + 1.f), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
		m_pGameInstance->Render_Text(L"Font_Malang", TEXT("회피율 증가"), _vec2(vTextPos.x, vTextPos.y), 0.3f, _vec4(1.f, 1.f, 1.f, 1.f), 0.f, true);
	}	
	else if (m_eItemDesc.iItemType == ITEM_BODY)
	{
		m_pGameInstance->Render_Text(L"Font_Malang", TEXT("방어력 증가"), _vec2(vTextPos.x + 1.f, vTextPos.y), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
		m_pGameInstance->Render_Text(L"Font_Malang", TEXT("방어력 증가"), _vec2(vTextPos.x, vTextPos.y + 1.f), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
		m_pGameInstance->Render_Text(L"Font_Malang", TEXT("방어력 증가"), _vec2(vTextPos.x, vTextPos.y), 0.3f, _vec4(1.f, 1.f, 1.f, 1.f), 0.f, true);
	}
	else if (m_eItemDesc.iItemType == ITEM_SWORD || m_eItemDesc.iItemType == ITEM_BOW)
	{
		m_pGameInstance->Render_Text(L"Font_Malang", TEXT("공격력 증가"), _vec2(vTextPos.x + 1.f, vTextPos.y), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
		m_pGameInstance->Render_Text(L"Font_Malang", TEXT("공격력 증가"), _vec2(vTextPos.x, vTextPos.y + 1.f), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
		m_pGameInstance->Render_Text(L"Font_Malang", TEXT("공격력 증가"), _vec2(vTextPos.x, vTextPos.y), 0.3f, _vec4(1.f, 1.f, 1.f, 1.f), 0.f, true);
	}
	
	
	if (m_eItemDesc.eItemUsage == IT_HPPOTION)
	{
		m_pGameInstance->Render_Text(L"Font_Malang", TEXT("체력 회복량"), _vec2(vTextPos.x + 1.f, vTextPos.y), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
		m_pGameInstance->Render_Text(L"Font_Malang", TEXT("체력 회복량"), _vec2(vTextPos.x, vTextPos.y + 1.f), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
		m_pGameInstance->Render_Text(L"Font_Malang", TEXT("체력 회복량"), _vec2(vTextPos.x, vTextPos.y), 0.3f, _vec4(1.f, 1.f, 1.f, 1.f), 0.f, true);
	}
	else if (m_eItemDesc.eItemUsage == IT_MPPOTION)
	{
		m_pGameInstance->Render_Text(L"Font_Malang", TEXT("마나 회복량"), _vec2(vTextPos.x + 1.f, vTextPos.y), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
		m_pGameInstance->Render_Text(L"Font_Malang", TEXT("마나 회복량"), _vec2(vTextPos.x, vTextPos.y + 1.f), 0.3f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
		m_pGameInstance->Render_Text(L"Font_Malang", TEXT("마나 회복량"), _vec2(vTextPos.x, vTextPos.y), 0.3f, _vec4(1.f, 1.f, 1.f, 1.f), 0.f, true);
	}


	if (m_eItemDesc.eItemUsage != IT_VEHICLECARD)
	{
		m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_eItemDesc.iStatus) + TEXT("%"), _vec2(vTextPos.x + 1.f, vTextPos.y + 25.f), 0.7f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
		m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_eItemDesc.iStatus) + TEXT("%"), _vec2(vTextPos.x, vTextPos.y + 25.f + 1.f), 0.7f, _vec4(0.f, 0.f, 0.f, 1.f), 0.f, true);
		m_pGameInstance->Render_Text(L"Font_Malang", to_wstring(m_eItemDesc.iStatus) + TEXT("%"), _vec2(vTextPos.x, vTextPos.y + 25.f), 0.7f, _vec4(1.f, 0.9f, 0.8f, 1.f), 0.f, true);
	}
	
	return S_OK;
}


HRESULT CItemInfo::Add_Parts()
{
	CTextButton::TEXTBUTTON_DESC Button = {};

	Button.eLevelID = LEVEL_STATIC;
	Button.fDepth = m_fDepth + 0.01f;
	Button.strText = TEXT("");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Exit");
	Button.vPosition = _vec2(_vec2(m_fX + 145.f, m_fY - 170.f));
	Button.vSize = _vec2(30.f, 30.f);

	m_pExitButton = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);
	if (not m_pExitButton)
	{
		return E_FAIL;
	}

	if (m_eItemDesc.eItemUsage == IT_VEHICLECARD)
	{
		Button.strText = TEXT("소환");
	}
	else
	{
		Button.strText = TEXT("장착");
	}

	Button.fFontSize = 0.4f;
	Button.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	Button.vTextPosition = _vec2(0.f, -2.f);
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Button_Blue");
	Button.vPosition = _vec2(m_fX + 80.f, m_fY + 60.f);
	Button.vSize = _vec2(150.f, 100.f);
	m_SelectButton = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);
	if (not m_SelectButton)
	{
		return E_FAIL;
	}

	CNineSlice::SLICE_DESC SliceDesc{};
	SliceDesc.eLevelID = LEVEL_STATIC;
	SliceDesc.fDepth = m_fDepth + 0.02f;
	SliceDesc.fFontSize = 0.f;
	SliceDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_BG_Skill_Levelup_Icon_02");
	SliceDesc.strText = TEXT("");
	SliceDesc.vPosition = _vec2(m_fX, m_fY - 50.f);
	SliceDesc.vSize = _vec2(350.f, 300.f);
	m_pBackGround = (CNineSlice*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_NineSlice"), &SliceDesc);
	if (not m_pBackGround)
	{
		return E_FAIL;
	}

	CItem::ITEM_DESC ItemDesc = {};
	ItemDesc.bCanInteract = false;
	ItemDesc.eItemDesc = m_eItemDesc;
	ItemDesc.fDepth = m_fDepth + 0.01f;
	ItemDesc.vPosition = _vec2(m_fX - 110.f, m_fY - 100.f);
	ItemDesc.vSize = _vec2(80.f, 80.f);
	ItemDesc.isScreen = false;
	ItemDesc.haveBG = true;
	m_pItemTex = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Item"), &ItemDesc);
	if (not m_pItemTex)
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CItemInfo::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CItemInfo::Bind_ShaderResources()
{
	return S_OK;
}

CItemInfo* CItemInfo::Create(_dev pDevice, _context pContext)
{
	CItemInfo* pInstance = new CItemInfo(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CItemBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CItemInfo::Clone(void* pArg)
{
	CItemInfo* pInstance = new CItemInfo(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CItemBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CItemInfo::Free()
{
	__super::Free();

	Safe_Release(m_pItemTex);
	Safe_Release(m_pExitButton);
	Safe_Release(m_pBackGround);
	Safe_Release(m_SelectButton);

	Safe_Release(m_pRendererCom);
}
