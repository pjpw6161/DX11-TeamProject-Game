#include "Launcher.h"

#include "Projectile.h"
#include "Effect_Dummy.h"
#include "Effect_Manager.h"

CLauncher::CLauncher(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CLauncher::CLauncher(const CLauncher& rhs)
	: CGameObject(rhs)
{
}

HRESULT CLauncher::Init_Prototype()
{
	return S_OK;
}

HRESULT CLauncher::Init(void* pArg)
{
	m_eType = *(LAUNCHER_TYPE*)pArg;

	_vec3 vCenterPos = _vec3(-2000.f, 0.f, 2000.f);

	switch (m_eType)
	{
	case Client::CLauncher::TYPE_RANDOM_POS:
	{
		m_strModelTag = TEXT("Prototype_Model_LokiStone");
		m_Animation.iAnimIndex = 0;
		m_Animation.isLoop = true;
		m_Animation.fAnimSpeedRatio = 2.f;

		m_pTransformCom->Set_Scale(_vec3(0.7f));

		random_device dev;
		_randNum RandomNumber(dev());
		_randFloat Random = _randFloat(-1.f, 1.f);

		_vec3 vRandomDir = _vec3(Random(RandomNumber), 0.f, Random(RandomNumber)).Get_Normalized();

		m_pTransformCom->Set_Position(vCenterPos + (rand() % 8 + 1) * vRandomDir); // Radius : 8

		m_iPassIndex = AnimPass_Dissolve;
	}

		break;

	case Client::CLauncher::TYPE_FLOOR:

		break;

	case Client::CLauncher::TYPE_LASER:
		break;

	case Client::CLauncher::TYPE_PIZZA:
		break;
	}

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}



	return S_OK;
}

void CLauncher::Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_OldMatrix();

	if (m_pGameInstance->Key_Pressing(DIK_DOWN))
	{
		Kill();
	}

	if (m_fDissolveRatio <= 0.f)
	{
		m_iPassIndex = AnimPass_Rim;
	}

	m_fTime += fTimeDelta;
	m_fProjectileCreateTime += fTimeDelta;

	switch (m_eType)
	{
	case Client::CLauncher::TYPE_RANDOM_POS:

		if (m_fTime >= 2.5f)
		{
			if (m_fProjectileCreateTime >= 1.5f)
			{
				if (m_iProjectileCount <= 3)
				{
					for (size_t i = 0; i < 8; i++)
					{
						CProjectile::PROJECTILE_DESC Desc = {};
						Desc.eType = CProjectile::TYPE_RANDOM_POS;
						Desc.pLauncherTransform = m_pTransformCom;
						Desc.vStartPos = m_pTransformCom->Get_State(State::Pos);

						m_pGameInstance->Add_Layer(LEVEL_VILLAGE, TEXT("Layer_Projectile"), TEXT("Prototype_GameObject_Projectile"), &Desc);
					}

					++m_iProjectileCount;
				}

				else
				{
					Kill();
				}

				m_fProjectileCreateTime = 0.f;
			}
		}


		break;
	case Client::CLauncher::TYPE_FLOOR:
		break;
	case Client::CLauncher::TYPE_LASER:
		break;
	case Client::CLauncher::TYPE_PIZZA:
		break;
	}

	m_pModelCom->Set_Animation(m_Animation);

}

void CLauncher::Late_Tick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);
}

HRESULT CLauncher::Render()
{
	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
			_bool bFailed = true;
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

		if (FAILED(m_pShaderCom->Begin(m_iPassIndex)))
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

HRESULT CLauncher::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_strModelTag, TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom), m_pTransformCom)))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_T_EFF_Noise_04_BC"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pDissolveTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLauncher::Bind_ShaderResources()
{
	if (m_iPassIndex == AnimPass_Dissolve)
	{
		if (FAILED(m_pDissolveTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture")))
		{
			return E_FAIL;
		}

		m_fDissolveRatio -= 0.02f;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveRatio", &m_fDissolveRatio, sizeof _float)))
		{
			return E_FAIL;
		}

		_bool bHasNorTex = true;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &bHasNorTex, sizeof _bool)))
		{
			return E_FAIL;
		}
	}

	if (m_iPassIndex == AnimPass_Rim)
	{
		_vec4 vColor = Colors::Gold;
		if (FAILED(m_pShaderCom->Bind_RawValue("g_RimColor", &vColor, sizeof vColor)))
		{
			return E_FAIL;
		}
	}

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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CLauncher* CLauncher::Create(_dev pDevice, _context pContext)
{
	CLauncher* pInstance = new CLauncher(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CLauncher");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLauncher::Clone(void* pArg)
{
	CLauncher* pInstance = new CLauncher(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CLauncher");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLauncher::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColliderCom);

	Safe_Release(m_pDissolveTextureCom);

}
