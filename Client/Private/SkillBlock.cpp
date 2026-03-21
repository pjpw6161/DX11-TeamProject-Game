#include "SkillBlock.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "UI_Manager.h"

CSkillBlock::CSkillBlock(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CSkillBlock::CSkillBlock(const CSkillBlock& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSkillBlock::Init_Prototype()
{
	m_isPrototype = true;
	return S_OK;
}

HRESULT CSkillBlock::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}
	if (FAILED(Add_Slots()))
	{
		return E_FAIL;
	}

	CUI_Manager::Get_Instance()->Set_SkillBlock(this);
	return S_OK;
}

void CSkillBlock::Tick(_float fTimeDelta)
{
	
	_int iIdx = CUI_Manager::Get_Instance()->Get_WeaponType(PT_SWORD, &m_eCurType);

	/* ½ºÅ³ºÏ¿¡¼­ ÀåÂøÇÑ½ºÅ³ ¹Þ¾Æ¿È */
	if (CUI_Manager::Get_Instance()->Is_SkillSlotChange())
	{
		CUI_Manager::Get_Instance()->Set_SkillSlotChange(false);

		for (size_t j = 0; j < WP_END; j++)
		{
			for (size_t i = 0; i < SKILL_END; i++)
			{
				CSkillSlot* pSkillSlot = CUI_Manager::Get_Instance()->Get_SkillSlot((WEAPON_TYPE)j, (SKILLSLOT)i);
				if (pSkillSlot == nullptr)
				{
					continue;
				}
				if (!pSkillSlot->Is_Full())
				{
					m_pSlots[j][i]->Delete_Skill();
					continue;
				}
				SKILLINFO tInfo = pSkillSlot->Get_SkillInfo();
				m_pSlots[j][i]->Set_Skill(tInfo);
			}
		}
	
	}

	

	for (size_t i = 0; i < SKILL_END; i++)
	{
		m_pSlots[m_eCurType][i]->Tick(fTimeDelta);
	}
}

void CSkillBlock::Late_Tick(_float fTimeDelta)
{
	if (CUI_Manager::Get_Instance()->Showing_FullScreenUI())
	{
		return;
	}
	for (size_t i = 0; i < SKILL_END; i++)
	{
		m_pSlots[m_eCurType][i]->Late_Tick(fTimeDelta);
	}
}

HRESULT CSkillBlock::Render()
{
	

	return S_OK;
}

_bool CSkillBlock::Use_Skill(WEAPON_TYPE eType, SKILLSLOT eSlot, _int* iIndex, _int* iMp)
{

	if (m_pSlots[eType][eSlot]->Use_Skill())
	{
		//¸¶³ª »©°í
		*iIndex = m_pSlots[m_eCurType][eSlot]->Get_SkillInfo().iSkillIdx;
		*iMp = m_pSlots[m_eCurType][eSlot]->Get_SkillInfo().iMp;
		return true;
	}
	*iIndex = -1;

	return false;
}

HRESULT CSkillBlock::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}



	return S_OK;
}

HRESULT CSkillBlock::Bind_ShaderResources()
{
	return S_OK;
}

HRESULT CSkillBlock::Add_Slots()
{
	for (size_t j = 0; j < WP_END; j++)
	{
		for (size_t i = 0; i < SKILL_END; i++)
		{
			CSkillSlot::SKILLSLOT_DESC SkillSlotDesc = {};
			SkillSlotDesc.eSlotMode = CSkillSlot::SCREEN;
			SkillSlotDesc.fDepth = (_float)D_SCREEN / (_float)D_END - 0.1f;
			SkillSlotDesc.vSize = _float2(60.f, 60.f);
			SkillSlotDesc.vPosition = _float2((_float)g_iWinSizeX / 2.f - 250.f + (SkillSlotDesc.vSize.x / 2.f) + (SkillSlotDesc.vSize.x * i) + (-15.f * i), 675.f);
			SkillSlotDesc.iSlotIdx = (_uint)i;
			m_pSlots[j][i] = (CSkillSlot*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_SkillSlot"), &SkillSlotDesc);
			if (m_pSlots[j][i] == nullptr)
			{
				return E_FAIL;
			}
		}
	}

	return S_OK;
}

CSkillBlock* CSkillBlock::Create(_dev pDevice, _context pContext)
{
	CSkillBlock* pInstance = new CSkillBlock(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CSkillBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkillBlock::Clone(void* pArg)
{
	CSkillBlock* pInstance = new CSkillBlock(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSkillBlock");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkillBlock::Free()
{
	__super::Free();

	if (!m_isPrototype)
	{
		for (size_t j = 0; j < WP_END; j++)
		{
			for (size_t i = 0; i < SKILL_END; i++)
			{
				Safe_Release(m_pSlots[j][i]);
			}
		}

	}

	Safe_Release(m_pRendererCom);

}
