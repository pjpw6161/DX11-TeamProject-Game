#include "Skill_Model.h"
#include "Effect_Manager.h"
#include "Effect_Dummy.h"
#include "Arrow.h"
#include "Camera_Manager.h"

CSkill_Model::CSkill_Model(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CSkill_Model::CSkill_Model(const CSkill_Model& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSkill_Model::Init_Prototype()
{
	return S_OK;
}

HRESULT CSkill_Model::Init(void* pArg)
{

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_pTransformCom->Set_State(State::Pos, _vec4(0.f,200.f,0.f,1.f));

	m_Animation.iAnimIndex = 0;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = true;
	m_Animation.fAnimSpeedRatio = 1.7f;
	return S_OK;
}

void CSkill_Model::Tick(_float fTimeDelta)
{
	if (CCamera_Manager::Get_Instance()->Get_CameraState() != CS_SKILLBOOK)
	{
		m_bView = false;
		return;
	}


	
	

	if (m_eCurAnimState == SWORD1 or m_eCurAnimState == SWORD2 or m_eCurAnimState == SWORD3 or m_eCurAnimState == SWORD4)
	{
		m_pTransformCom->Set_State(State::Pos, _vec4(0.f, 300.f, 2.f, 1.f));
		m_pWeapon_ModelCom->Set_Animation(m_Animation);
		m_pModelCom->Set_Animation(m_Animation);
	
	}
	else
	{
		m_pTransformCom->Set_State(State::Pos, _vec4(0.f, 300.f, 0.f, 1.f));
		m_pModelCom->Set_Animation(m_Animation);
	}
	m_Animation.bRestartAnimation = false;
	After_BowAtt();
	
	if (m_bArrowRain_Start)
	{
		Arrow_Rain();
	}

}

void CSkill_Model::Late_Tick(_float fTimeDelta)
{
	if (CCamera_Manager::Get_Instance()->Get_CameraState() != CS_SKILLBOOK)
	{
		return;
	}

	if (!m_bView)
	{
		return;
	}


	if (m_eCurAnimState == SWORD1 or m_eCurAnimState == SWORD2 or m_eCurAnimState == SWORD3 or m_eCurAnimState == SWORD4)
	{
	
		m_pWeapon_ModelCom->Play_Animation(fTimeDelta);
		m_pModelCom->Play_Animation(fTimeDelta);
	}
	else
	{
		m_pModelCom->Play_Animation(fTimeDelta);
	}
	
	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);
}

HRESULT CSkill_Model::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		
		if (i == 0)
		{
			if (m_eCurAnimState == SWORD1 or m_eCurAnimState == SWORD2 or m_eCurAnimState == SWORD3 or m_eCurAnimState == SWORD4)
			{
				continue;
			}
		}

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
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
		
		_bool HasMaskTex{};
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_MaskTexture", i, TextureType::Shininess)))
		{
			HasMaskTex = false;
		}
		else
		{
			HasMaskTex = true;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasMaskTex", &HasMaskTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(AnimPass_Default)))
		{
			return E_FAIL;
		}

		if (FAILED(m_pModelCom->Render(i)))
		{
			return E_FAIL;
		}
	}

	if (m_eCurAnimState == SWORD1 or m_eCurAnimState == SWORD2 or m_eCurAnimState == SWORD3 or m_eCurAnimState == SWORD4)
	{

		for (_uint i = 0; i < m_pWeapon_ModelCom->Get_NumMeshes(); i++)
		{
			if (FAILED(m_pWeapon_ModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
			{
			}

			_bool HasNorTex{};
			if (FAILED(m_pWeapon_ModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
			{
				HasNorTex = false;
			}
			else
			{
				HasNorTex = true;
			}

			_bool HasMaskTex{};
			if (FAILED(m_pWeapon_ModelCom->Bind_Material(m_pShaderCom, "g_MaskTexture", i, TextureType::Shininess)))
			{
				HasMaskTex = false;
			}
			else
			{
				HasMaskTex = true;
			}

			if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
			{
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Bind_RawValue("g_HasMaskTex", &HasMaskTex, sizeof _bool)))
			{
				return E_FAIL;
			}

			if (FAILED(m_pWeapon_ModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
			{
				return E_FAIL;
			}

			if (FAILED(m_pShaderCom->Begin(AnimPass_Default)))
			{
				return E_FAIL;
			}

			if (FAILED(m_pWeapon_ModelCom->Render(i)))
			{
				return E_FAIL;
			}
		}
	}
	return S_OK;
}

void CSkill_Model::After_BowAtt()
{
	_float Index = m_pModelCom->Get_CurrentAnimPos();

	if (m_eCurAnimState == BOW1)
	{
		if (Index > 2.f && Index < 4.f)
		{
			m_ReadyArrow = true;
		}
		if (Index >= 16.f && Index <= 19.f && m_ReadyArrow)
		{
			Create_Arrow(BOW1);
			m_ReadyArrow = false;
		}
		else if (Index > 20.f && Index <= 24.f && !m_ReadyArrow)
		{
			m_ReadyArrow = true;
		}
		else if (Index >= 30.f && Index <= 32.f && m_ReadyArrow)
		{
			Create_Arrow(BOW1);
			m_ReadyArrow = false;
		}
		else if (Index > 33.f && Index <= 35.f && !m_ReadyArrow)
		{
			m_ReadyArrow = true;
		}
		else if (Index >= 47.f && Index <= 49.f && m_ReadyArrow)
		{
			Create_Arrow(BOW1);
			m_ReadyArrow = false;
		}
	}
	else if (m_eCurAnimState == BOW2)
	{
		if (Index > 2.f && Index < 4.f)
		{
			m_ReadyArrow = true;
		}

		if (Index >= 5.f && Index <= 6.f)
		{
			m_pTransformCom->Jump(7.f);
		}
		else if (Index >= 34.f && Index <= 36.f && m_ReadyArrow)
		{
			Create_Arrow(BOW2);
			m_ReadyArrow = false;
		}
	}
	else if (m_eCurAnimState == BOW3)
	{
		if (Index > 2.f && Index < 4.f)
		{
			m_ReadyArrow = true;
		}

		if (Index >= 19.f && Index <= 20.f)
		{
			Create_Arrow(SWORD4);

		}
		else if (Index >= 50.f && m_ReadyArrow)
		{
			Create_Arrow(BOW3);
			m_ReadyArrow = false;
		}
	}
}

void CSkill_Model::Create_Arrow(SKILLMODEL_ANIM Skill)
{
	_mat offet = _mat::CreateTranslation(_vec3(0.f, 0.9f, 0.f));
	_mat bone = (*m_pModelCom->Get_BoneMatrix("bowstring"));
	_mat world = m_pTransformCom->Get_World_Matrix();
	world = offet * bone * world;
	Arrow_Type type{};

	switch (Skill)
	{
	case BOW1:
	{
		type.world = world;
		type.vLook = m_pTransformCom->Get_State(State::Look);
		type.Att_Type = AT_Bow_Skill1;
		type.MonCollider = nullptr;

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Arrow"), &type)))
		{
			return;
		}
	}
	break;
	case BOW2:
	{
		type.world = world;
		type.vLook = m_pTransformCom->Get_State(State::Look);
		type.Att_Type = AT_Bow_Skill2;
		type.MonCollider = nullptr;

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Arrow"), &type)))
		{
			return;
		}
	}
	break;
	case SWORD4:
	{
		type.world = world;
		type.vLook = m_pTransformCom->Get_State(State::Look);
		type.vLook.y += 0.6f;
		type.Att_Type = AT_Bow_Skill3_Start;
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Arrow"), &type)))
		{
			return;
		}
	}
	break;
	case BOW3:
	{
		m_bArrowRain_Start = true;
		m_iArrowRain = 0;
	}
	break;
	case BOW4:
	{
		type.world = world;
		type.vLook = m_pTransformCom->Get_State(State::Look);
		type.vLook.y -= 0.25f;
		type.Att_Type = AT_Bow_Skill4;
		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Arrow"), &type)))
		{
			return;
		}
	}
	break;
	default:
		break;
	}


}

void CSkill_Model::Arrow_Rain()
{
	if (m_iArrowRain == 0)
	{
		m_vArrowLook = m_pTransformCom->Get_State(State::Look);
	}

	if (m_iArrowRain < 40)
	{
		Arrow_Type Type{};
		Type.Att_Type = AT_Bow_Skill3;
		_float random = (_float)(rand() % 50);
		_int randommos = rand() % 2;
		if (randommos == 0)
		{
			random *= -1;
		}
		random *= 0.05f;
		_float  random2 = (_float)(rand() % 51);
		_int randommo = rand() % 2;
		if (randommo == 0)
		{
			random2 *= -1;
		}
		random2 *= 0.05f;
		Type.vPos = m_pTransformCom->Get_State(State::Pos) + m_vArrowLook * 7.f + _vec4(random, 10.f, random2, 0.f)/* + m_pTransformCom->Get_State(State::Right) * 4.f*/;
		Type.vLook = _vec4(0.01f, -1.f, 0.f, 0.f);

		if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Arrow"), TEXT("Prototype_GameObject_Arrow"), &Type)))
		{
			return;
		}
		m_iArrowRain++;
	}
	else
	{
		m_bArrowRain_Start = false;
	}
}

HRESULT CSkill_Model::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Skill_Model"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), m_pTransformCom)))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Skill_Model_Weapon"), TEXT("Com_Model1"), reinterpret_cast<CComponent**>(&m_pWeapon_ModelCom), m_pTransformCom)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSkill_Model::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

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

	return S_OK;
}

void CSkill_Model::Change_AnimState(SKILLMODEL_ANIM eAnim)
{
	m_bView = true;
	m_eCurAnimState = eAnim;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = true;
	m_Animation.fAnimSpeedRatio = 1.7f;
	m_Animation.iAnimIndex = eAnim;
	m_Animation.bRestartAnimation = true;
}

CSkill_Model* CSkill_Model::Create(_dev pDevice, _context pContext)
{
	CSkill_Model* pInstance = new CSkill_Model(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CSkill_Model");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill_Model::Clone(void* pArg)
{
	CSkill_Model* pInstance = new CSkill_Model(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSkill_Model");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_Model::Free()
{

	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pWeapon_ModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
}
