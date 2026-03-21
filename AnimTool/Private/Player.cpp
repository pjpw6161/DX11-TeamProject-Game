#include "Player.h"
#include "Animation.h"

CPlayer::CPlayer(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer& rhs)
	: CGameObject(rhs)
	, m_iNumMonsterModels(rhs.m_iNumMonsterModels)
	, m_iNumSelectModels(rhs.m_iNumSelectModels)
{
}

CModel* CPlayer::Get_CurrentModel()
{
	return m_pModelCom;
}

CRealtimeVTFModel* CPlayer::Get_CurrentPlayerModel()
{
	return m_pPlayerModelCom;
}

CAnimation* CPlayer::Get_CurrentAnim()
{
	if (m_eType == TYPE_PLAYER)
	{
		return m_pPlayerModelCom->Get_Animation(m_pPlayerModelCom->Get_CurrentAnimationIndex());
	}
	else
	{
		return m_pModelCom->Get_Animation(m_pModelCom->Get_CurrentAnimationIndex());
	}
}

HRESULT CPlayer::Init_Prototype(_uint iNumMonsterModels, _uint iNumSelectModels)
{
	m_iNumMonsterModels = iNumMonsterModels;
	m_iNumSelectModels = iNumSelectModels;

	return S_OK;
}

HRESULT CPlayer::Init(void* pArg)
{
	m_pMonsterModelCom = new CModel * [m_iNumMonsterModels];
	m_pMonsterModelTag = new wstring[m_iNumMonsterModels];

	for (_uint i = 0; i < m_iNumMonsterModels; i++)
	{
		_tchar szPrototypeTag[MAX_PATH] = TEXT("");
		const wstring& strPrototypeTag = TEXT("Prototype_Model_Monster_%d");
		wsprintf(szPrototypeTag, strPrototypeTag.c_str(), i);
		m_pMonsterModelTag[i] = szPrototypeTag;
	}

	m_pSelectModelCom = new CModel * [m_iNumSelectModels];
	m_pSelectModelTag = new wstring[m_iNumSelectModels];

	for (_uint i = 0; i < m_iNumSelectModels; i++)
	{
		_tchar szPrototypeTag[MAX_PATH] = TEXT("");
		const wstring& strPrototypeTag = TEXT("Prototype_Model_Select_%d");
		wsprintf(szPrototypeTag, strPrototypeTag.c_str(), i);
		m_pSelectModelTag[i] = szPrototypeTag;
	}

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	if (FAILED(Place_PartModels()))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	if (m_eType == TYPE_MONSTER)
	{
		m_pModelCom = m_pMonsterModelCom[m_iCurrentIndex];
	}
	else if (m_eType == TYPE_SELECT)
	{
		m_pModelCom = m_pSelectModelCom[m_iCurrentIndex];
	}

	if (m_eType == TYPE_PLAYER)
	{
		if (m_pPlayerModelCom)
		{
			m_pPlayerModelCom->Play_Animation(fTimeDelta);
		}
	}
	else if (m_pModelCom)
	{
		m_pModelCom->Play_Animation(fTimeDelta);
	}
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);
}

HRESULT CPlayer::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	
	if (m_eType == TYPE_PLAYER)
	{
		for (size_t i = 0; i < 4; i++)
		{
			for (size_t k = 0; k < m_pPlayerModelCom->Get_Num_PartMeshes(i, 0); k++)
			{
				if (FAILED(m_pPlayerModelCom->Bind_Part_Material(m_pPlayerShaderCom, "g_DiffuseTexture", TextureType::Diffuse, (_uint)i, (_uint)0, k)))
					continue;

				_bool HasNorTex{};
				if (FAILED(m_pPlayerModelCom->Bind_Part_Material(m_pPlayerShaderCom, "g_NormalTexture", TextureType::Normals, (_uint)i, (_uint)0, k)))
				{
					HasNorTex = false;
				}
				else
				{
					HasNorTex = true;
				}

				_bool HasMaskTex{};
				if (FAILED(m_pPlayerModelCom->Bind_Part_Material(m_pPlayerShaderCom, "g_MaskTexture", TextureType::Shininess, (_uint)i, (_uint)0, k)))
				{
					HasMaskTex = false;
				}
				else
				{
					HasMaskTex = true;
				}

				if (FAILED(m_pPlayerShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
					return E_FAIL;

				HasMaskTex = false;

				if (FAILED(m_pPlayerShaderCom->Bind_RawValue("g_HasMaskTex", &HasMaskTex, sizeof _bool)))
					return E_FAIL;


				if (FAILED(m_pPlayerShaderCom->Begin(0)))
					return E_FAIL;

				if (FAILED(m_pPlayerModelCom->Render_Part((_uint)i, (_uint)0, k)))
					return E_FAIL;
			}
		}
	}
	else if (m_pModelCom)
	{
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

			if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
			{
				return E_FAIL;
			}

			if (FAILED(m_pModelCom->Bind_BoneMatrices(i, m_pShaderCom, "g_BoneMatrices")))
			{
				return E_FAIL;
			}

			_uint iPassIndex{ AnimPass_Default };

			if (FAILED(m_pShaderCom->Begin(iPassIndex)))
			{
				return E_FAIL;
			}

			if (FAILED(m_pModelCom->Render(i)))
			{
				return E_FAIL;
			}
		}
	}

	return S_OK;
}

HRESULT CPlayer::Place_PartModels()
{
	CRealtimeVTFModel::PART_DESC Desc{};

	Desc.ePartType = 0;
	Desc.FileName = "body5";

	if (FAILED(m_pPlayerModelCom->Place_Parts(Desc, true)))
		return E_FAIL;

	Desc.ePartType = 1;
	Desc.FileName = "face0";

	if (FAILED(m_pPlayerModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.ePartType = 2;
	Desc.FileName = "hair0";

	if (FAILED(m_pPlayerModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.ePartType = 3;

	Desc.FileName = "bow4";

	if (FAILED(m_pPlayerModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "sword4";

	if (FAILED(m_pPlayerModelCom->Place_Parts(Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_RTVTF"), TEXT("Com_Shader_Player"), reinterpret_cast<CComponent**>(&m_pPlayerShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Player"), TEXT("Com_Model_Player"), reinterpret_cast<CComponent**>(&m_pPlayerModelCom), m_pTransformCom)))
	{
		return E_FAIL;
	}

	for (_uint i = 0; i < m_iNumMonsterModels; i++)
	{
		_tchar szComName[MAX_PATH] = TEXT("");
		const wstring& strComName = TEXT("Com_Model_Monster%d");
		wsprintf(szComName, strComName.c_str(), i);
		wstring strFinalComName = szComName;

		if (FAILED(__super::Add_Component(LEVEL_TOOL, m_pMonsterModelTag[i], strFinalComName, reinterpret_cast<CComponent**>(&m_pMonsterModelCom[i]), m_pTransformCom)))
		{
			return E_FAIL;
		}
	}

	for (_uint i = 0; i < m_iNumSelectModels; i++)
	{
		_tchar szComName[MAX_PATH] = TEXT("");
		const wstring& strComName = TEXT("Com_Model_Select%d");
		wsprintf(szComName, strComName.c_str(), i);
		wstring strFinalComName = szComName;

		if (FAILED(__super::Add_Component(LEVEL_TOOL, m_pSelectModelTag[i], strFinalComName, reinterpret_cast<CComponent**>(&m_pSelectModelCom[i]), m_pTransformCom)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CPlayer::Bind_ShaderResources()
{
	if (m_eType == TYPE_PLAYER)
	{
		if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pPlayerShaderCom, "g_OldWorldMatrix")))
		{
			return E_FAIL;
		}

		if (FAILED(m_pPlayerShaderCom->Bind_Matrix("g_OldViewMatrix", m_pGameInstance->Get_OldViewMatrix())))
		{
			return E_FAIL;
		}
		// WorldMatrix 바인드
		if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pPlayerShaderCom, "g_WorldMatrix")))
			return E_FAIL;

		// ViewMatrix 바인드
		if (FAILED(m_pPlayerShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(TransformType::View))))
			return E_FAIL;

		// ProjMatrix 바인드
		if (FAILED(m_pPlayerShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(TransformType::Proj))))
			return E_FAIL;

		// 카메라 Far 바인드
		if (FAILED(m_pPlayerShaderCom->Bind_RawValue("g_CamNF", &m_pGameInstance->Get_CameraNF(), sizeof _float2)))
			return E_FAIL;

		m_pPlayerModelCom->Set_UsingMotionBlur(true);

		// 뼈 바인드
		if (FAILED(m_pPlayerModelCom->Bind_Bone(m_pPlayerShaderCom)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_OldWorldMatrix")))
		{
			return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Bind_Matrix("g_OldViewMatrix", m_pGameInstance->Get_OldViewMatrix())))
		{
			return E_FAIL;
		}

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
	}

	return S_OK;
}

CPlayer* CPlayer::Create(_dev pDevice, _context pContext, _uint iNumMonsterModels, _uint iNumSelectModels)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(iNumMonsterModels, iNumSelectModels)))
	{
		MSG_BOX("Failed to Create : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer::Clone(void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	if (m_pMonsterModelCom)
	{
		for (_uint i = 0; i < m_iNumMonsterModels; i++)
		{
			Safe_Release(m_pMonsterModelCom[i]);
		}
	}
	Safe_Delete_Array(m_pMonsterModelCom);

	Safe_Delete_Array(m_pMonsterModelTag);

	if (m_pSelectModelCom)
	{
		for (_uint i = 0; i < m_iNumSelectModels; i++)
		{
			Safe_Release(m_pSelectModelCom[i]);
		}
	}
	Safe_Delete_Array(m_pSelectModelCom);

	Safe_Delete_Array(m_pSelectModelTag);

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);

	Safe_Release(m_pPlayerModelCom);
	Safe_Release(m_pPlayerShaderCom);
}
