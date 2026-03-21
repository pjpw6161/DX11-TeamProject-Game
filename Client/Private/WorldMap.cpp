#include "WorldMap.h"
#include "UI_Manager.h"
#include "Camera_Manager.h"
#include "TextButton.h"
#include "3DUITEX.h"
#include "Trigger_Manager.h"
#include "Camera_Manager.h"
#include "Event_Manager.h"

CWorldMap::CWorldMap(_dev pDevice, _context pContext)
	: CGameObject(pDevice, pContext)
{
}

CWorldMap::CWorldMap(const CWorldMap& rhs)
	: CGameObject(rhs)
{
}


HRESULT CWorldMap::Init_Prototype()
{
	m_isPrototype = true;
	return S_OK;
}

HRESULT CWorldMap::Init(void* pArg)
{

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_pTransformCom->Set_State(State::Pos, _vec4(1000.f,1000.f,1000.f, 1.f));

	if (FAILED(Add_Points()))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CWorldMap::Tick(_float fTimeDelta)
{

	if (m_isReady)
	{
		m_fReadyTime += fTimeDelta;
		_float fLength = (m_pGameInstance->Get_CameraPos() - m_pTransformCom->Get_State(State::Pos)).Length();
		if (!m_isActive && !m_bSelect)
		{
			if (fLength <= 100.f)
			{			
				CUI_Manager::Get_Instance()->Set_FullScreenUI(true);
				m_isReady = false;
				m_isActive = true;
			}
		}

		if (m_isActive && !m_bSelect)
		{
			if (fLength >= 100.f)
			{
				CUI_Manager::Get_Instance()->Set_FullScreenUI(false);
				m_isReady = false;
				m_isActive = false;
			}
		}

		if (m_bSelect && m_fReadyTime >= 0.5f)
		{
			CUI_Manager::Get_Instance()->Set_FullScreenUI(false);
			
			m_bSelect = false;
			m_isReady = false;
			m_isActive = false;
			switch (m_eCurPoint)
			{
			case VILLAGE:
				CUI_Manager::Get_Instance()->Set_Teleport(true, TS_Village);
				break;
			case DUNGEON:
				CUI_Manager::Get_Instance()->Set_Teleport(true, TS_Dungeon);
				break;
			case TOWER:
				CUI_Manager::Get_Instance()->Set_FullScreenUI(true);
				CUI_Manager::Get_Instance()->Open_InfinityTower(true);
				break;
			}
			return;
		}
	}

	if (m_pGameInstance->Key_Down(DIK_M) || CUI_Manager::Get_Instance()->Is_WorldMap())
	{
		m_pGameInstance->Play_Sound(TEXT("WorldMap_Click"), 1.f);
		if (!m_isActive && !m_isReady)
		{
			CCamera_Manager::Get_Instance()->Set_CameraState(CS_WORLDMAP);
			m_isReady = true;
			return;
		}
		if (m_isActive && !m_isReady)
		{
			CCamera_Manager::Get_Instance()->Set_CameraState(CS_DEFAULT);
			m_isReady = true;
			return;
		}
	}


	if (!m_isActive)
	{
		return;
	}
	m_pTransformCom->Set_OldMatrix();

	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	m_fWater_Nomal.x += fTimeDelta * 0.1f;
	m_fWater_Nomal.y -= fTimeDelta * 0.1f;

	for (size_t i = 0; i < MAP_END; i++)
	{
		if (m_Points[i] != nullptr)
		{
			if (PtInRect(&m_Points[i]->Get_Rect(), ptMouse) && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
			{
				if ((MAPPOINT)i != TOWER && CUI_Manager::Get_Instance()->Get_Riding(VC_FLY) == (Riding_Type)Type_End)
				{
					CEvent_Manager::Get_Instance()->Set_Alert(TEXT("공중 탈 것을 장착하세요."));
						return;
				}
				m_isReady = true;
				m_bSelect = true;
				m_fReadyTime = 0.f;
				m_eCurPoint = (MAPPOINT)i;
				CCamera_Manager::Get_Instance()->Set_CameraState(CS_DEFAULT);
				return;
			}

			m_Points[i]->Tick(fTimeDelta);
		}
	}
	m_pTitle->Tick(fTimeDelta);


}             

void CWorldMap::Late_Tick(_float fTimeDelta)
{
	if (!m_isActive)
	{
		return;
}
	m_pRendererCom->Add_RenderGroup(RenderGroup::RG_NonBlend, this);
	for (size_t i = 0; i < MAP_END; i++)
	{
		if (m_Points[i] != nullptr)
		{
			m_Points[i]->Late_Tick(fTimeDelta);
		}
	}
	m_pTitle->Late_Tick(fTimeDelta);
}

HRESULT CWorldMap::Render()
{

	if (FAILED(Bind_ShaderResources()))
	{
		return E_FAIL;
	}
	for (_uint j = 0; j < 3; j++)
	{
		_uint iNumMeshes = m_pModelCom[j]->Get_NumMeshes();

		for (_uint i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pModelCom[j]->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
			{
				return E_FAIL;
			}

			_bool HasNorTex{};
			if (FAILED(m_pModelCom[j]->Bind_Material(m_pShaderCom, "g_NormalTexture", i, TextureType::Normals)))
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
			if (j == 2)
			{
				if(i==1)
				{
					_float fy = m_fWater_Nomal.y * 0.7f;
					if (FAILED(m_pShaderCom->Bind_RawValue("g_fy", &fy, sizeof _float)))
					{
						return E_FAIL;
					}
				}
				else if (i == 2)
				{
					_float fy = m_fWater_Nomal.y * 1.5f;
					if (FAILED(m_pShaderCom->Bind_RawValue("g_fy", &fy, sizeof _float)))
					{
						return E_FAIL;
					}
				}
				else
				{
					_float fy = m_fWater_Nomal.y * 0.15f;
					if (FAILED(m_pShaderCom->Bind_RawValue("g_fy", &fy, sizeof _float)))
					{
						return E_FAIL;
					}
				}
				if (FAILED(m_pShaderCom->Begin(StaticPass_Worldmap_Water)))
				{
					return E_FAIL;
				}
			}
			else if (j == 1)
			{
			
				if (FAILED(m_pMaskTexture->Bind_ShaderResource(m_pShaderCom, "g_CloudTexture")))
				{
					return E_FAIL;
				}
				_float fy = m_fWater_Nomal.y * 0.08f;
				_float fx = m_fWater_Nomal.x * 0.08f;
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fy", &fy, sizeof _float)))
				{
					return E_FAIL;
				}
				if (FAILED(m_pShaderCom->Bind_RawValue("g_fx", &fx, sizeof _float)))
				{
					return E_FAIL;
				}
				if (FAILED(m_pShaderCom->Begin(StaticPass_Worldmap_Cloud)))
				{
					return E_FAIL;
				}
			}
			else
			{
				if (FAILED(m_pShaderCom->Begin(0)))
				{
					return E_FAIL;
				}
			}

			if (FAILED(m_pModelCom[j]->Render(i)))
			{
				return E_FAIL;
			}


		}
	}


	return S_OK;
}

HRESULT CWorldMap::Add_Points()
{
	C3DUITex::UITEX_DESC Desc{};

	Desc.eLevelID = LEVEL_STATIC;
	Desc.strText = TEXT("마나하임");
	Desc.vTextColor = _vec4(1.f, 0.7f, 0.1f, 1.f);
	Desc.vTextPosition = _vec2(0.f, 60.f);
	Desc.fFontSize = 0.4f;
	Desc.pParentTransform = m_pTransformCom;
	Desc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_World_Village");
	Desc.vPosition = _vec3(-10.f, 4.f, -3.f);
	Desc.vSize = _vec2(60.f, 60.f);


	m_Points[VILLAGE] = (C3DUITex*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_3DUITex"), &Desc);
	if (not m_Points[VILLAGE])
	{
		return E_FAIL;
	}

	Desc.strText = TEXT("던전");
	Desc.eLevelID = LEVEL_STATIC;
	Desc.pParentTransform = m_pTransformCom;
	Desc.vTextColor = _vec4(1.f, 0.9f, 0.8f, 1.f);
	Desc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_World_Dungeon");
	Desc.vPosition = _vec3(-25.f, 4.f, 8.f);
	Desc.vSize = _vec2(60.f, 60.f);


	m_Points[DUNGEON] = (C3DUITex*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_3DUITex"), &Desc);
	if (not m_Points[DUNGEON])
	{
		return E_FAIL;
	}

	Desc.strText = TEXT("무한의 탑");
	Desc.eLevelID = LEVEL_STATIC;
	Desc.pParentTransform = m_pTransformCom;
	Desc.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_World_Tower");
	Desc.vPosition = _vec3(-2.f, 4.f, 8.f);
	Desc.vSize = _vec2(60.f, 60.f);


	m_Points[TOWER] = (C3DUITex*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_3DUITex"), &Desc);
	if (not m_Points[TOWER])
	{
		return E_FAIL;
	}


	CTextButton::TEXTBUTTON_DESC Button = {};

	Button.eLevelID = LEVEL_STATIC;
	Button.fDepth = (_float)D_WINDOW / (_float)D_END;
	Button.eLevelID = LEVEL_STATIC;
	Button.fFontSize = 0.5f;
	Button.strText = TEXT("월드맵 / 미드가르드");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Back");
	Button.vPosition = _vec2(20.f, 25.f);
	Button.vSize = _vec2(50.f, 50.f);
	Button.vTextColor = _vec4(1.f, 1.f, 1.f, 1.f);
	Button.vTextPosition = _vec2(100.f, 0.f);

	m_pTitle = m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pTitle)
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CWorldMap::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	{
		return E_FAIL;
	}
	m_iShaderPass = StaticPass_Default;
	

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_WorldMap_Object"), TEXT("Com_Model0"), reinterpret_cast<CComponent**>(&m_pModelCom[0]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_WorldMap_Ground"), TEXT("Com_Model1"), reinterpret_cast<CComponent**>(&m_pModelCom[1]))))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_WorldMap_Water"), TEXT("Com_Model2"), reinterpret_cast<CComponent**>(&m_pModelCom[2]))))
	{
		return E_FAIL;
	}
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Texture_WorldMap_Cloud"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pMaskTexture))))
	{
		return E_FAIL;
	}
	
	return S_OK;
}

HRESULT CWorldMap::Bind_ShaderResources()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof _float4)))
	{
		return E_FAIL;
	}



	if (FAILED(m_pShaderCom->Bind_RawValue("g_CamNF", &m_pGameInstance->Get_CameraNF(), sizeof _float2)))
	{
		return E_FAIL;
	}

	return S_OK;
}


CWorldMap* CWorldMap::Create(_dev pDevice, _context pContext)
{
	CWorldMap* pInstance = new CWorldMap(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CWorldMap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWorldMap::Clone(void* pArg)
{
	CWorldMap* pInstance = new CWorldMap(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CWorldMap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWorldMap::Free()
{
	__super::Free();

	if (!m_isPrototype)
	{
		for (size_t i = 0; i < MAP_END; i++)
		{
			Safe_Release(m_Points[i]);
		}
	}

	for (int i = 0; i < 3; i++)
	{

	Safe_Release(m_pModelCom[i]);
	}

	Safe_Release(m_pTitle);
	Safe_Release(m_pMaskTexture);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
}
