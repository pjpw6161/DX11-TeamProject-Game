#include "SkillSlot.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "UI_Manager.h"
#include "Event_Manager.h"
#include "Skill.h"

CSkillSlot::CSkillSlot(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CSkillSlot::CSkillSlot(const CSkillSlot& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CSkillSlot::Init_Prototype()
{
	return S_OK;
}

HRESULT CSkillSlot::Init(void* pArg)
{
	m_eSlotMode = ((SKILLSLOT_DESC*)pArg)->eSlotMode;
	switch (m_eSlotMode)
	{
	case SCREEN:
		m_strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_Slot");
		break;
	case SKILLBOOK:
		m_strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_invenslot");
		break;
	}

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}


	m_fSizeX = ((SKILLSLOT_DESC*)pArg)->vSize.x;
	m_fSizeY = ((SKILLSLOT_DESC*)pArg)->vSize.y;

	m_fX = ((SKILLSLOT_DESC*)pArg)->vPosition.x;
	m_fY = ((SKILLSLOT_DESC*)pArg)->vPosition.y;

	m_fDepth = ((SKILLSLOT_DESC*)pArg)->fDepth;
	m_iSlotIdx = ((SKILLSLOT_DESC*)pArg)->iSlotIdx;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	m_rcRect = {
		  (LONG)(m_fX - m_fSizeX * 0.5f),
		  (LONG)(m_fY - m_fSizeY * 0.5f),
		  (LONG)(m_fX + m_fSizeX * 0.5f),
		  (LONG)(m_fY + m_fSizeY * 0.5f)
	};

	return S_OK;
}

void CSkillSlot::Tick(_float fTimeDelta)
{

	if (m_eSlotMode == SCREEN)
	{

	}


	if (m_pSkill != nullptr)
	{
		m_pSkill->Tick(fTimeDelta);
	}
}

void CSkillSlot::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
	
	if (m_pSkill != nullptr)
	{
		m_pSkill->Late_Tick(fTimeDelta);
	}
}

HRESULT CSkillSlot::Render()
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

HRESULT CSkillSlot::Set_Skill(SKILLINFO tSkillInfo)
{
	if (m_pSkill != nullptr)
	{
		Safe_Release(m_pSkill);
	}

	CSkill::SKILL_DESC SkillDesc = {};
	SkillDesc.fDepth = m_fDepth - 0.01f;
	SkillDesc.tSkillInfo = tSkillInfo;
	SkillDesc.vPosition = _vec2(m_fX, m_fY);
	if (m_eSlotMode == SCREEN)
	{
		SkillDesc.vSize = _vec2(m_fSizeX - 20.f, m_fSizeY - 20.f);
		SkillDesc.isScreen = true;
	}
	else
	{
		SkillDesc.vSize = _vec2(m_fSizeX, m_fSizeY);
		SkillDesc.isScreen = false;
	}

	m_pSkill = (CSkill*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Skill"), &SkillDesc);

	m_isFull = true;
	return S_OK;
}

void CSkillSlot::Delete_Skill()
{
	Safe_Release(m_pSkill);
	m_isFull = false;
}

SKILLINFO CSkillSlot::Get_SkillInfo()
{
	if (m_pSkill == nullptr)
	{
		SKILLINFO Info{};
		Info.strTexture = TEXT("");
		return Info;
	}
	return m_pSkill->Get_SkillInfo();
}

_bool CSkillSlot::Use_Skill()
{
	if (m_pSkill == nullptr)
	{	
		CEvent_Manager::Get_Instance()->Set_Alert(TEXT("스킬이 없습니다."));
		return false;
	}
	if (m_pSkill->Use_Skill())
	{
		return true;
	}
	
	return false;
}

HRESULT CSkillSlot::Add_Components()
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

HRESULT CSkillSlot::Bind_ShaderResources()
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

CSkillSlot* CSkillSlot::Create(_dev pDevice, _context pContext)
{
	CSkillSlot* pInstance = new CSkillSlot(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CSkillSlot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkillSlot::Clone(void* pArg)
{
	CSkillSlot* pInstance = new CSkillSlot(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSkillSlot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkillSlot::Free()
{
	__super::Free();

	Safe_Release(m_pSkill);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);
}
