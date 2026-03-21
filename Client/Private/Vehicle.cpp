#include "Vehicle.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "TextButton.h"
#include "TextButtonColor.h"
#include "VehicleBook.h"
#include "NineSlice.h"


CVehicle::CVehicle(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CVehicle::CVehicle(const CVehicle& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CVehicle::Init_Prototype()
{
	return S_OK;
}

HRESULT CVehicle::Init(void* pArg)
{

	m_eRidingType = ((VEHICLE_DESC*)pArg)->eRidingType;

	if (FAILED(Init_Info()))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_fSizeX = ((VEHICLE_DESC*)pArg)->vSize.x;
	m_fSizeY = ((VEHICLE_DESC*)pArg)->vSize.y;

	m_fX = ((VEHICLE_DESC*)pArg)->vPosition.x;
	m_fY = ((VEHICLE_DESC*)pArg)->vPosition.y;

	m_fDepth = ((VEHICLE_DESC*)pArg)->fDepth;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	

	m_rcRect = {
		  (LONG)(m_fX - m_fSizeX * 0.5f),
		  (LONG)(m_fY - m_fSizeY * 0.5f),
		  (LONG)(m_fX + m_fSizeX * 0.5f),
		  (LONG)(m_fY + m_fSizeY * 0.5f)
	};



	CTextButtonColor::TEXTBUTTON_DESC TextButton = {};
	TextButton.eLevelID = LEVEL_STATIC;
	TextButton.strTexture = TEXT("");
	TextButton.fDepth = m_fDepth - 0.01f;
	TextButton.fAlpha = 1.f;
	TextButton.fFontSize = 0.45f;
	TextButton.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	TextButton.strText = TEXT("");
	TextButton.vSize = _vec2(513.f, 513.f);
	TextButton.vPosition = _vec2(TextButton.vSize.x / 2.f - 10.f, TextButton.vSize.x / 2.f + 50.f);
	TextButton.vTextPosition = _vec2(0.f, 0.f);
	TextButton.strTexture = m_tVehicleInfo.strDescName;
	m_pDetail = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);

	if (not m_pDetail)
	{
		return E_FAIL;
	}
	m_pDetail->Set_Pass(VTPass_UI_Alpha);
	

	TextButton.fDepth = m_fDepth - 0.03f;
	TextButton.vSize = _vec2(20.f, 20.f);
	TextButton.vPosition = _vec2(m_fX, m_fX);

	if (m_tVehicleInfo.eType == VC_GROUND)
	{
		TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_EquipButtonC");
		m_pEquipButton = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);

		if (not m_pEquipButton)
		{
			return E_FAIL;
		}
	}
	else
	{
		TextButton.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_EquipButtonV");
		m_pEquipButton = (CTextButtonColor*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButtonColor"), &TextButton);

		if (not m_pEquipButton)
		{
			return E_FAIL;
		}
	}

	m_pEquipButton->Set_Pass(VTPass_UI);


	CNineSlice::SLICE_DESC SliceDesc{};
	SliceDesc.eLevelID = LEVEL_STATIC;
	SliceDesc.fDepth = m_fDepth - 0.02f;
	SliceDesc.strText = TEXT("");
	SliceDesc.vPosition = _vec2(m_fX, m_fY);
	SliceDesc.vSize = _vec2(m_fSizeX, m_fSizeY);
	SliceDesc.vTextPosition = _vec2(0.f, 0.f);
	SliceDesc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Border");
	m_pEquip = (CNineSlice*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_NineSlice"), &SliceDesc);
	if (not m_pEquip)
	{
		return E_FAIL;
	}

	CTextButton::TEXTBUTTON_DESC Button{};
	Button.eLevelID = LEVEL_STATIC;
	Button.fDepth = m_fDepth - 0.01f;
	Button.fFontSize = 0.5f;
	Button.strText = TEXT("");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_skSelect");
	Button.vPosition = _vec2(m_fX, m_fY);
	Button.vSize = _vec2(m_fSizeX, m_fSizeY);
	Button.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	Button.vTextPosition = _vec2(70.f, 0.f);

	m_pSelected = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pSelected)
	{
		return E_FAIL;
	}

	return S_OK;
}

void CVehicle::Tick(_float fTimeDelta)
{
	if (!m_isSelected && m_pRiding)
	{
		Safe_Release(m_pRiding);
	}

	if (m_isSelected)
	{
		_vec2 vPos = m_pDetail->Get_TransPosition();
		_vec2 vSize = m_pDetail->Get_Size();
		if (vPos.x < vSize.x / 2.f)
		{
			m_pDetail->Set_Position(_vec2(vPos.x + fTimeDelta * 50.f, vPos.y));
		}

		m_fAlpha += fTimeDelta * 2.f;
		if (m_fAlpha >= 1.f)
		{
			m_fAlpha = 1.f;
		}
		m_pDetail->Set_Alpha(m_fAlpha);
		m_pSelected->Set_Position(_vec2(m_fX, m_fY));


		m_pDetail->Tick(fTimeDelta);
		m_pSelected->Tick(fTimeDelta);
		if (m_pRiding != nullptr)
		{
			m_pRiding->Tick(fTimeDelta);
		}
	}
	if (m_isEquip)
	{

		m_pEquipButton->Set_Position(_vec2(m_fX - 120.f, m_fY - 15.f));
		m_pEquipButton->Tick(fTimeDelta);

		m_pEquip->Set_Position(_vec2(m_fX, m_fY));
		m_pEquip->Set_Size(m_fSizeX - 20.f, m_fSizeY - 16.f);
		m_pEquip->Tick(fTimeDelta);
	}

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
}

void CVehicle::Late_Tick(_float fTimeDelta)
{
	if (m_isSelected)
	{
		m_pDetail->Late_Tick(fTimeDelta);
		m_pSelected->Late_Tick(fTimeDelta);
		if (m_pRiding != nullptr)
		{
			m_pRiding->Late_Tick(fTimeDelta);
		}
	}
	if (m_isEquip)
	{
		m_pEquip->Late_Tick(fTimeDelta);
		m_pEquipButton->Late_Tick(fTimeDelta);
	}

	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);

}

HRESULT CVehicle::Render()
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

void CVehicle::Set_Position(_vec2 vPos)
{
	m_fX = vPos.x;
	m_fY = vPos.y;	
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);
	m_rcRect = {
	  (LONG)(m_fX - m_fSizeX * 0.5f),
	  (LONG)(m_fY - m_fSizeY * 0.5f),
	  (LONG)(m_fX + m_fSizeX * 0.5f),
	  (LONG)(m_fY + m_fSizeY * 0.5f)
	};

}

void CVehicle::Set_SelectVehicle(_bool isSelect)
{
	if (!m_isSelected && isSelect == true)
	{
		if (m_pRiding == nullptr)
		{
			m_pRiding = (CRiding*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Riding"), &m_tRidingDesc);
			if (not m_pRiding)
			{
				return;
			}
		}

		m_pDetail->Set_Position(_vec2(m_pDetail->Get_Position()));
		m_fAlpha = 0.f;
	}
	if (!isSelect)
	{
		Safe_Release(m_pRiding);
	}
	m_isSelected = isSelect;
}
HRESULT CVehicle::Init_Info()
{

	Riding_Desc riding_desc{};


	switch (m_eRidingType)
	{
	case Bird:
		m_tVehicleInfo.eRidingType = m_eRidingType;
		m_tVehicleInfo.eType = VC_FLY;
		m_tVehicleInfo.strDescName = TEXT("Prototype_Component_Texture_UI_Gameplay_DescBird");
		m_strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Bird");
		riding_desc.Type = Bird;
		break;
	case Wyvern:
		m_tVehicleInfo.eRidingType = m_eRidingType;
		m_tVehicleInfo.eType = VC_FLY;
		m_tVehicleInfo.strDescName = TEXT("Prototype_Component_Texture_UI_Gameplay_DescWyvern");
		m_strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Wyvern");
		riding_desc.Type = Wyvern;
		break;
	case Falar:
		m_tVehicleInfo.eRidingType = m_eRidingType;
		m_tVehicleInfo.eType = VC_FLY;
		m_tVehicleInfo.strDescName = TEXT("Prototype_Component_Texture_UI_Gameplay_DescFalar");
		m_strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Falar");
		riding_desc.Type = Falar;
		break;
	case Horse:
		m_tVehicleInfo.eRidingType = m_eRidingType;
		m_tVehicleInfo.eType = VC_GROUND;
		m_tVehicleInfo.strDescName = TEXT("Prototype_Component_Texture_UI_Gameplay_DescHorse");
		m_strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Horse");
		riding_desc.Type = Horse;
		break;
	case Tiger:
		m_tVehicleInfo.eRidingType = m_eRidingType;
		m_tVehicleInfo.eType = VC_GROUND;
		m_tVehicleInfo.strDescName = TEXT("Prototype_Component_Texture_UI_Gameplay_DescTiger");
		m_strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Tiger");
		riding_desc.Type = Tiger;
		break;
	case Nihilir:
		m_tVehicleInfo.eRidingType = m_eRidingType;
		m_tVehicleInfo.eType = VC_GROUND;
		m_tVehicleInfo.strDescName = TEXT("Prototype_Component_Texture_UI_Gameplay_DescNihilir");
		m_strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Nihilir");
		riding_desc.Type = Nihilir;
		break;
	}

	riding_desc.iMode = 1;
	m_tRidingDesc = riding_desc;
	

	return S_OK;
}

HRESULT CVehicle::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_strTexture, TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}


	return S_OK;
}

HRESULT CVehicle::Bind_ShaderResources()
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

CVehicle* CVehicle::Create(_dev pDevice, _context pContext)
{
	CVehicle* pInstance = new CVehicle(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CVehicle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CVehicle::Clone(void* pArg)
{
	CVehicle* pInstance = new CVehicle(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CVehicle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVehicle::Free()
{
	__super::Free();
	Safe_Release(m_pRiding);
	Safe_Release(m_pEquip);
	Safe_Release(m_pDetail);
	Safe_Release(m_pSelected);
	Safe_Release(m_pEquipButton);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
