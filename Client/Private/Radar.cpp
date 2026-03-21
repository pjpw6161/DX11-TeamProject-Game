#include "Radar.h"
#include "GameInstance.h"
#include "TextButton.h"
#include "UI_Manager.h"

CRadar::CRadar(_dev pDevice, _context pContext)
	: COrthographicObject(pDevice, pContext)
{
}

CRadar::CRadar(const CRadar& rhs)
	: COrthographicObject(rhs)
{
}

HRESULT CRadar::Init_Prototype()
{
	m_isPrototype = true;
	return S_OK;
}

HRESULT CRadar::Init(void* pArg)
{
	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}
	
	m_fSizeX = 50.f;
	m_fSizeY = 50.f;

	m_fX = 40.f;
	m_fY = 120.f;
	m_fDepth = (_float)D_SCREEN / (_float)D_END;

	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	if (FAILED(Add_Parts()))
	{
		return E_FAIL;
	}

	return S_OK;
}

void CRadar::Tick(_float fTimeDelta)
{
	POINT ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	RECT rectUI = {
			  (LONG)(m_fX - m_fSizeX * 0.5f),
			  (LONG)(m_fY - m_fSizeY * 0.5f),
			  (LONG)(m_fX + m_fSizeX * 0.5f),
			  (LONG)(m_fY + m_fSizeY * 0.5f)
	};
	if (TRUE == PtInRect(&rectUI, ptMouse) && m_pGameInstance->Mouse_Down(DIM_LBUTTON, InputChannel::UI))
	{
		if (!m_isActive)
		{
			m_isActive = true;
		}
		else if (m_isActive)
		{
			m_isActive = false;
		}
	}

	
	__super::Apply_Orthographic(g_iWinSizeX, g_iWinSizeY);

	if (m_isActive)
	{
		/* 지도 회전 */
		_vec4 vCameraLook = m_pGameInstance->Get_CameraLook(); 
		vCameraLook.Normalize();

		_vec2 vPos = _vec2(0.f, 1.f);
		_float fAngle = atan2(vCameraLook.z, vCameraLook.x) - atan2(vPos.y, vPos.x);
		m_fAngleCamera = fAngle * 180.0f / XM_PI;
		
		m_pRadar->Get_Transform()->Rotation(_vec4(0.f, 0.f, 1.f, 0.f), m_fAngleCamera * -1.f);

		/* 플레이어 화살표 회전 */
		CTransform* pPlayerTransform = (CTransform*)m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"));
		if (pPlayerTransform == nullptr)
		{
			return;
		}
		_vec4 vPlayerLook = pPlayerTransform->Get_State(State::Look);
		vPlayerLook.Normalize();
		fAngle = atan2(vPlayerLook.z, vPlayerLook.x) - atan2(vPos.y, vPos.x);
		m_fAnglePlayer = fAngle * 180.0f / XM_PI;

		m_pPlayerIcon->Get_Transform()->Rotation(_vec4(0.f, 0.f, 1.f, 0.f), m_fAnglePlayer + m_fAngleCamera * -1.f);


		m_pRadar->Tick(fTimeDelta);
		m_pPlayerIcon->Tick(fTimeDelta); 
	}
	else
	{
		m_pButton->Tick(fTimeDelta);
	}
}

void CRadar::Late_Tick(_float fTimeDelta)
{
	if (CUI_Manager::Get_Instance()->Showing_FullScreenUI())
	{
		return;
	}
	if (m_isActive)
	{
		m_pRadar->Late_Tick(fTimeDelta);
		m_pPlayerIcon->Late_Tick(fTimeDelta);
		m_pMonsterIcon->Late_Tick(fTimeDelta);
		m_pNpcIcon->Late_Tick(fTimeDelta);
		m_pRendererCom->Add_RenderGroup(RenderGroup::RG_UI, this);
	}
	else
	{
		m_pButton->Late_Tick(fTimeDelta);
	}

}

HRESULT CRadar::Render()
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

	for (size_t i = 0; i < CUI_Manager::TYPE_END; i++)
	{
		if (CUI_Manager::Get_Instance()->Get_RadarPosition((CUI_Manager::TYPE)i).empty())
		{
			continue;
		}
		Render_Icons(i);
	}

	return S_OK;
}

void CRadar::Render_Icons(_uint iIdx)
{
	if ((CUI_Manager::TYPE)iIdx >= CUI_Manager::TYPE_END)
	{
		return;
	}

	vector<CTransform*> vecTransform = CUI_Manager::Get_Instance()->Get_RadarPosition((CUI_Manager::TYPE)iIdx);
	for (auto& iter : vecTransform)
	{
		_vec3 vObjectPos = iter->Get_State(State::Pos);
		CTransform* pPlayerTransform = (CTransform*)m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"));
		_vec3 vPlayerPos = pPlayerTransform->Get_State(State::Pos);
		_vec3 vLength = XMVector3Length(vPlayerPos - vObjectPos);

		if (vLength.x < 45.f)
		{
			_vec2 MapCenter = m_pRadar->Get_Position();
			_float fMapPosX = (vObjectPos.x - vPlayerPos.x) / 100.f * 140.f;
			_float fMapPosY = (vObjectPos.z - vPlayerPos.z) / 100.f * 140.f;

			_vec2 vObjectRadarPos = _vec2(MapCenter.x + fMapPosX, MapCenter.y + fMapPosY * -1.f);
			_vec3 fDir = vObjectPos - vPlayerPos;
			_float fAngle = atan2(fDir.z, fDir.x) - atan2(1.f, 0.f);

			_float fTheta = (m_fAngleCamera + fAngle * -1.f) * XM_PI / 180.0f;

			_float fCosTheta = cos(fTheta);
			_float fSinTheta = sin(fTheta);

			_float fRotateX = (vObjectRadarPos.x - MapCenter.x) * fCosTheta - (vObjectRadarPos.y - MapCenter.y) * fSinTheta;
			_float fRotateY = (vObjectRadarPos.x - MapCenter.x) * fSinTheta + (vObjectRadarPos.y - MapCenter.y) * fCosTheta;

			_vec2 vNewPos = _vec2(fRotateX + MapCenter.x, fRotateY + MapCenter.y);
			switch ((CUI_Manager::TYPE)iIdx)
			{
			case CUI_Manager::MONSTER:
				m_pMonsterIcon->Set_Position(vNewPos);
				m_pMonsterIcon->Render();
				break;
			case CUI_Manager::NPC:
				m_pNpcIcon->Set_Position(vNewPos);
				m_pNpcIcon->Render();
				break;

			}

		}
		else if (vLength.x >= 45.f)
		{
			switch ((CUI_Manager::TYPE)iIdx)
			{
			case CUI_Manager::MONSTER:
				m_pMonsterIcon->Set_Position(_vec2(-10.f, -10.f));
				break;
			case CUI_Manager::NPC:
				m_pNpcIcon->Set_Position(_vec2(-10.f, -10.f));
				break;

			}

		}
	}
}

HRESULT CRadar::Add_Parts()
{
	CTextButton::TEXTBUTTON_DESC Button = {};

	Button.eLevelID = LEVEL_STATIC;
	Button.fDepth = m_fDepth;
	Button.strText = TEXT("");
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_map");
	Button.vPosition = _vec2(m_fX, m_fY);
	Button.vSize = _vec2(m_fSizeX, m_fSizeY);

	m_pButton = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pButton)
	{
		return E_FAIL;
	}

	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_radar");
	Button.vPosition = _vec2(m_fX + 90.f, m_fY + 50.f);
	Button.vSize = _vec2(140.f, 140.f);
	m_pRadar = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pRadar)
	{
		return E_FAIL;
	}

	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_DirArrow");
	Button.fDepth = m_fDepth - 0.02f;
	Button.vPosition = _vec2(m_fX + 90.f, m_fY + 50.f);
	Button.vSize = _vec2(25.f, 25.f);
	m_pPlayerIcon = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pPlayerIcon)
	{
		return E_FAIL;
	}
	
	Button.fDepth = m_fDepth;
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_MonsterIcon");
	Button.vPosition = _vec2(-10.f, -10.f);//m_fX, m_fY);
	Button.vSize = _vec2(5.f, 5.f);
	m_pMonsterIcon = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pMonsterIcon)
	{
		return E_FAIL;
	}
	
	Button.strTexture = TEXT("Prototype_Component_Texture_UI_Gameplay_NpcIcon");
	Button.vPosition = _vec2(-10.f, 10.f);
	Button.vSize = _vec2(20.f, 20.f);
	m_pNpcIcon = (CTextButton*)m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_TextButton"), &Button);

	if (not m_pNpcIcon)
	{
		return E_FAIL;
	}
	
	return S_OK;
}

HRESULT CRadar::Add_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Gameplay_mapclick"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRadar::Bind_ShaderResources()
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

CRadar* CRadar::Create(_dev pDevice, _context pContext)
{
	CRadar* pInstance = new CRadar(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CRadar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRadar::Clone(void* pArg)
{
	CRadar* pInstance = new CRadar(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CRadar");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRadar::Free()
{
	__super::Free();


	Safe_Release(m_pRadar);
	Safe_Release(m_pButton);
	Safe_Release(m_pNpcIcon);
	Safe_Release(m_pPlayerIcon);
	Safe_Release(m_pMonsterIcon);

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

}