#include "Select_Model.h"
#include "Effect_Manager.h"
#include "Effect_Dummy.h"

CSelect_Model::CSelect_Model(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CSelect_Model::CSelect_Model(const CSelect_Model& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSelect_Model::Init_Prototype()
{
	return S_OK;
}

HRESULT CSelect_Model::Init(void* pArg)
{
	ObjectInfo* Info = (ObjectInfo*)pArg;
	m_strModelTag = Info->strPrototypeTag;

	Add_Components();

	m_pTransformCom->Set_State(State::Pos, Info->vPos);

	m_Animation.iAnimIndex = 0;
	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = true;

	return S_OK;
}

void CSelect_Model::Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_OldMatrix();

	if (m_pModelCom->IsAnimationFinished(S_MOTION))
	{
		m_eCurAnimState = S_PICK_IDLE;
		m_Animation.iAnimIndex = S_PICK_IDLE;
		m_Animation.isLoop = true;
		m_Animation.fAnimSpeedRatio = 1.5f;

	}
	if (m_pModelCom->IsAnimationFinished(S_CANCEL))
	{
		m_Animation.iAnimIndex = S_IDLE;
		m_Animation.isLoop = true;
		m_Animation.fAnimSpeedRatio = 2.5f;
	}
	if (m_eCurAnimState == S_MOTION)
	{
		m_Animation.fAnimSpeedRatio = 1.7f;
	}

	m_pModelCom->Set_Animation(m_Animation);
}

void CSelect_Model::Late_Tick(_float fTimeDelta)
{
	if (m_pEffect)
	{
		m_pEffect->Late_Tick(fTimeDelta);
	}

	m_pModelCom->Play_Animation(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);
}

HRESULT CSelect_Model::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
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

	return S_OK;
}

HRESULT CSelect_Model::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_SELECT, m_strModelTag, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), m_pTransformCom)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSelect_Model::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pTransformCom->Bind_OldWorldMatrix(m_pShaderCom, "g_OldWorldMatrix")))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_Matrix("g_OldViewMatrix", m_pGameInstance->Get_OldViewMatrix())))
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

void CSelect_Model::Change_AnimState(SELECTMODEL_ANIM eAnim)
{
	m_eCurAnimState = eAnim;

	switch (m_eCurAnimState)
	{
	case S_IDLE:
		m_Animation.iAnimIndex = S_IDLE;
		m_Animation.isLoop = true;

		break;
	case S_MOTION:
		m_Animation.iAnimIndex = S_MOTION;
		m_Animation.isLoop = false;
		m_Animation.fAnimSpeedRatio = 1.5f;
		m_Animation.bRestartAnimation = false;

		break;
	case S_CANCEL:
		m_Animation.iAnimIndex = S_CANCEL;
		m_Animation.isLoop = false;
		m_Animation.fAnimSpeedRatio = 1.5f;

		break;
	}
}

CSelect_Model* CSelect_Model::Create(_dev pDevice, _context pContext)
{
	CSelect_Model* pInstance = new CSelect_Model(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CSelect_Model");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSelect_Model::Clone(void* pArg)
{
	CSelect_Model* pInstance = new CSelect_Model(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSelect_Model");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSelect_Model::Free()
{
	if (m_strModelTag == L"Prototype_Model_Select2" or m_strModelTag == L"Prototype_Model_Select1")
	{
		Safe_Release(m_pEffect);
	}
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
}
