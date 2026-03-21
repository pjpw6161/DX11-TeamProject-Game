#include "Camera_CutScene.h"
#include "Camera_Manager.h"
#include "CutScene_Curve.h"

CCamera_CutScene::CCamera_CutScene(_dev pDevice, _context pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_CutScene::CCamera_CutScene(const CCamera_CutScene& rhs)
	: CCamera(rhs)
	//, m_pImGui_Manager(CImGui_Manager::Get_Instance())
{
	//Safe_AddRef(m_pImGui_Manager);
}

HRESULT CCamera_CutScene::Init_Prototype()
{
	return S_OK;
}

HRESULT CCamera_CutScene::Init(void* pArg)
{
	CamInfo = *(CameraInfo*)pArg;

	CCamera::Camera_Desc CamDesc;
	CamDesc.vCameraPos =CamInfo.vStartCutScene;

	CamDesc.vFocusPos = CamInfo.vEndCutScene;
	CamDesc.fFovY = XMConvertToRadians(60.f);
	CamDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	CamDesc.fNear = 0.1f;
	CamDesc.fFar = 1100.f;

	if (FAILED(__super::Init(&CamDesc)))
	{
		return E_FAIL;
	}

	if (FAILED(Add_Components()))
	{
		return E_FAIL;
	}

	m_pCam_Manager = CCamera_Manager::Get_Instance();
	Safe_AddRef(m_pCam_Manager);

	//_uint size_needed = MultiByteToWideChar(CP_UTF8, 0, CamInfo.strName.c_str(), -1, NULL, 0);
	//wstring wstrName(size_needed -1, 0);
	//MultiByteToWideChar(CP_UTF8, 0, CamInfo.strName.c_str(), -1, &wstrName[0], size_needed - 1);
	//m_strName = wstrName;
	if (CamInfo.ppCamera)
	{
		*CamInfo.ppCamera = this;
		CamInfo.ppCamera = nullptr;
	}
	wstring strLayerTag = TEXT("Layer_Camera");


	m_iNextSectionIndex = 0;
	m_iCurrentSectionIndex = 0;
	m_fCutSceneSpeed = CamInfo.fCameraSpeed;
	m_fTimeDeltaAcc = 0.f;
	m_iLastFrame = 0;
	
	m_isPreCameraLerp = false;
	m_vPreEye = _float3(0.f, 0.f, 0.f);
	m_vPreAt = _float3(0.f, 0.f, 0.f);


	return S_OK;
}

void CCamera_CutScene::Tick(_float fTimeDelta)
{
	if (m_pCam_Manager->Get_CameraModeIndex() != CM_CUTSCENE)
	{
		return;
	}
	if (m_pGameInstance->Key_Down(DIK_RETURN))
	{
		m_iFrame = 0;
		m_fTimeDeltaAcc = 0.f;
		m_iTotalFrame = 0;
		m_fTotalTimeDeltaAcc = 0.f;
		m_iCurrentSectionIndex = 0;
		m_iNextSectionIndex = 0;
		m_isPlayCutScene = false;

		m_pCam_Manager->Set_CameraModeIndex(CM_MAIN);
	}
	m_pGameInstance->Set_CameraNF(_float2(m_fNear, m_fFar));

	

	if (m_isPlayCutScene == false)
	{
		m_iFrame = 0;
		m_fTimeDeltaAcc = 0.f;
		m_iTotalFrame = 0;
		m_fTotalTimeDeltaAcc = 0.f;
		m_iCurrentSectionIndex = 0;
		m_iNextSectionIndex = 0;
		m_isPlayCutScene = true;
	}
	else
		if(m_isStop == false)
			Play_Camera(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CCamera_CutScene::Late_Tick(_float fTimeDelta)
{
	for (auto iter : m_CameraEyeList)
	{
		iter->Tick(fTimeDelta);
	}
	for (auto iter : m_CameraAtList)
	{
		iter->Tick(fTimeDelta);
	}
	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);
}

HRESULT CCamera_CutScene::Render()
{
	
	//if (FAILED(Bind_ShaderResources()))
	//	return E_FAIL;

	//_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	//for (_uint i = 0; i < iNumMeshes; ++i)
	//{
	//	if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", i, TextureType::Diffuse)))
	//	{
	//		continue;
	//	}

	//
	//	m_pShaderCom->Begin(0);

	//	m_pModelCom->Render(i);
	//}

	for (auto iter : m_CameraEyeList)
	{
		iter->Render();
	}
	for (auto iter : m_CameraAtList)
	{
		iter->Render();
	}

	return S_OK;
}

void CCamera_CutScene::Set_Delete_Curve()
{
	m_CameraAtList.back()->Kill();
	m_CameraAtList.pop_back();
	m_CameraEyeList.back()->Kill();
	m_CameraEyeList.pop_back();
}

string CCamera_CutScene::Get_Name()
{
	_uint size_needed = WideCharToMultiByte(CP_UTF8, 0, m_strName.c_str(), -1, NULL, 0, NULL, NULL);
	string str(size_needed-1, 0);
	WideCharToMultiByte(CP_UTF8, 0, m_strName.c_str(), -1, &str[0], size_needed, NULL, NULL);

	return str;
}

HRESULT CCamera_CutScene::Add_Eye_Curve(_vec4 vFirstPoint, _vec4 vSecondPoint, _float fSectionSpeed)
{
	_uint	iCurrentLevel = m_pGameInstance->Get_CurrentLevelIndex();
	_uint	iCutSceneType = CCutScene_Curve::SECTION_TYPE_EYE;

	SectionInfo SectionInfo;
	//SectionInfo.ppCurve = &m_pEyeCurve;
	SectionInfo.vStartCutScene = vFirstPoint;
	SectionInfo.vEndCutScene = vSecondPoint;
	wstring strCamEyeName = TEXT("Layer_Section_Eye");
	strSectionName = TEXT("Curve_Eye_") + to_wstring(m_CameraEyeList.size());
	SectionInfo.strSectionName = strSectionName;
	SectionInfo.iSectionType = iCutSceneType;

	m_pEyeCurve = static_cast<class CCutScene_Curve*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Camera_Curve"), &SectionInfo));
	_mat	matPoints;
	matPoints.Right(vFirstPoint);
	matPoints.Up(vFirstPoint);
	matPoints.Look(vSecondPoint);
	matPoints.Position(vSecondPoint);
	
	m_pEyeCurve->Set_SectionSpeed(fSectionSpeed);
	m_pEyeCurve->Set_ControlPoints(matPoints);
	m_CameraEyeList.push_back(static_cast<class CCutScene_Curve*>(m_pEyeCurve));
	m_pEyeCurve = nullptr;
	return S_OK;
}

HRESULT CCamera_CutScene::Add_Eye_Curve(_mat matPoints,  _float fCurveSpeed)
{
	_uint	iCurrentLevel = m_pGameInstance->Get_CurrentLevelIndex();
	_uint	iCutSceneType = CCutScene_Curve::SECTION_TYPE_EYE;

	SectionInfo SectionInfo;
	//SectionInfo.ppCurve = &m_pEyeCurve;
	SectionInfo.vStartCutScene = matPoints.Up();
	SectionInfo.vEndCutScene = matPoints.Look();
	wstring strCamEyeName = TEXT("Layer_Section_Eye");
	strSectionName = + TEXT("Curve_Eye_") + to_wstring(m_CameraEyeList.size());
	SectionInfo.strSectionName = strSectionName;
	SectionInfo.iSectionType = iCutSceneType;

	m_pEyeCurve = dynamic_cast<class CCutScene_Curve*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Camera_Curve"), &SectionInfo));

	m_pEyeCurve->Set_SectionSpeed(fCurveSpeed);
	m_pEyeCurve->Set_ControlPoints(matPoints);
	m_CameraEyeList.push_back(static_cast<class CCutScene_Curve*>(m_pEyeCurve));
	m_pEyeCurve = nullptr;
	return S_OK;
}

HRESULT CCamera_CutScene::Add_At_Curve(_vec4 vFirstPoint, _vec4 vSecondPoint, _float fSectionSpeed)
{
	_uint	iCurrentLevel = m_pGameInstance->Get_CurrentLevelIndex();
	_uint	iCutSceneType = CCutScene_Curve::SECTION_TYPE_AT;

	SectionInfo SectionInfo;
	//SectionInfo.ppCurve = &m_pAtCurve;
	SectionInfo.vStartCutScene = vFirstPoint;
	SectionInfo.vEndCutScene = vSecondPoint;
	wstring strCamAtName = L"Layer_Section_At";
	strSectionName =  L"Curve_At_" + to_wstring(m_CameraAtList.size());
	SectionInfo.strSectionName = strSectionName;
	SectionInfo.iSectionType = iCutSceneType;

	m_pAtCurve = dynamic_cast<class CCutScene_Curve*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Camera_Curve"), &SectionInfo));

	_mat	matPoints;
	matPoints.Right(vFirstPoint);
	matPoints.Up(vFirstPoint);
	matPoints.Look(vSecondPoint);
	matPoints.Position(vSecondPoint);
	m_pAtCurve->Set_SectionSpeed(fSectionSpeed);
	m_pAtCurve->Set_ControlPoints(matPoints);
	m_CameraAtList.push_back(static_cast<class CCutScene_Curve*>(m_pAtCurve));
	m_pAtCurve = nullptr;

	return S_OK;
}

HRESULT CCamera_CutScene::Add_At_Curve(_mat matPoints)
{
	_uint	iCurrentLevel = m_pGameInstance->Get_CurrentLevelIndex();
	_uint	iCutSceneType = CCutScene_Curve::SECTION_TYPE_AT;

	SectionInfo SectionInfo;
	//SectionInfo.ppCurve = &m_pAtCurve;
	SectionInfo.vStartCutScene = matPoints.Up();
	SectionInfo.vEndCutScene = matPoints.Look();
	wstring strCamAtName = L"Layer_Section_At";
	strSectionName = L"Curve_At_" + to_wstring(m_CameraAtList.size());
	SectionInfo.strSectionName = strSectionName;
	SectionInfo.iSectionType = iCutSceneType;
	m_pAtCurve = dynamic_cast<class CCutScene_Curve*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Camera_Curve"), &SectionInfo));

	m_pAtCurve->Set_SectionSpeed(5.f);
	m_pAtCurve->Set_ControlPoints(matPoints);
	m_CameraAtList.push_back(static_cast<class CCutScene_Curve*>(m_pAtCurve));
	m_pAtCurve = nullptr;

	return S_OK;
}

HRESULT CCamera_CutScene::Delete_Curve()
{
	

	return S_OK;
}

void CCamera_CutScene::Play_Camera(_float fTimeDelta)
{

	if (m_CameraEyeList.size() > 0)
	{
		m_iSectionCount = (_uint)m_CameraEyeList.size();
		m_iLastFrame = m_CameraEyeList.front()->Get_CurveSize() * m_iSectionCount;
	}

	if (m_iFrame > m_CameraEyeList.front()->Get_CurveSize() - 2 && !m_CameraEyeList.empty())
	{
		m_iFrame = 0;
		m_fTimeDeltaAcc = 0.f;
		m_iNextSectionIndex = m_iCurrentSectionIndex + 1;

		if (m_iNextSectionIndex < m_CameraEyeList.size())
		{
			m_iCurrentSectionIndex = m_iNextSectionIndex;
		}
		else
		{
			m_isPlayCutScene = false;
			m_pCam_Manager->Set_CameraModeIndex(CM_MAIN);
		}
	}
	if (m_iCurrentSectionIndex < m_CameraEyeList.size())
	{
		_vec4 vFrameEyePos = m_CameraEyeList[m_iCurrentSectionIndex]->Get_CurvePos(m_iFrame);
		_vec4 vFrameAtPos = m_CameraAtList[m_iCurrentSectionIndex]->Get_CurvePos(m_iFrame);

		_float fSectionSpeed = m_CameraEyeList[m_iCurrentSectionIndex]->Get_SectionSpeed();

		if (fTimeDelta < 1.0)
		{
			m_fTimeDeltaAcc += (_float)fTimeDelta * m_fCutSceneSpeed * fSectionSpeed;
		}

		m_fTotalTimeDeltaAcc += (_float)fTimeDelta * m_fCutSceneSpeed * fSectionSpeed;
		m_iFrame = (_uint)m_fTimeDeltaAcc;
		m_iTotalFrame = (_uint)m_fTotalTimeDeltaAcc;

		_vec4 vAt = _vec4(vFrameAtPos.x, vFrameAtPos.y, vFrameAtPos.z, 1.f);
		m_pTransformCom->Set_State(State::Pos, _vec4(vFrameEyePos.x, vFrameEyePos.y, vFrameEyePos.z, 1.f));
		m_pTransformCom->LookAt(vAt);
	}
}

void CCamera_CutScene::Start_Play()
{
	m_isStop = false;
	m_pCam_Manager->Set_CameraModeIndex(CM_CUTSCENE);
}

void CCamera_CutScene::Stop_Play()
{
	m_isStop = true;
}
void CCamera_CutScene::Reset_Play()
{
	m_iFrame = 0;
	m_fTimeDeltaAcc = 0.f;
	m_iTotalFrame = 0;
	m_fTotalTimeDeltaAcc = 0.f;
	m_iCurrentSectionIndex = 0;
	m_iNextSectionIndex = 0;
	m_isPlayCutScene = false;
	m_pCam_Manager->Set_CameraModeIndex(CM_MAIN);

}

HRESULT	CCamera_CutScene::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
	{
		return E_FAIL;
	}

	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxStatMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
	//{
	//	return E_FAIL;
	//}

	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Camera"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
	//{
	//	return E_FAIL;
	//}

	return S_OK;
}
HRESULT	CCamera_CutScene::Bind_ShaderResources()
{
	//_mat m_WorldMatrix = XMMatrixIdentity();
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
	{
		return E_FAIL;
	}	
	/*if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
	{
		return E_FAIL;
	}*/

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof _float4)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &m_pGameInstance->Get_CameraNF().y, sizeof _float)))
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

	return S_OK;
}



CCamera_CutScene* CCamera_CutScene::Create(_dev pDevice, _context pContext)
{
	CCamera_CutScene* pInstance = new CCamera_CutScene(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CCamera_CutScene");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_CutScene::Clone(void* pArg)
{
	CCamera_CutScene* pInstance = new CCamera_CutScene(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CCamera_CutScene");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_CutScene::Free()
{
	__super::Free();
	//Safe_Release(m_pImGui_Manager);
	for (int i = 0; i < m_CameraAtList.size(); i++) {
		Safe_Release(m_CameraAtList[i]);
	}
	m_CameraAtList.clear();

	for (int i = 0; i < m_CameraEyeList.size(); i++) {
		Safe_Release(m_CameraEyeList[i]);
	}
	m_CameraEyeList.clear();

	Safe_Release(m_pCam_Manager);
	Safe_Release(m_pRendererCom);
	//Safe_Release(m_pModelCom);
	//Safe_Release(m_pShaderCom);
	Safe_Release(m_pEyeCurve);
	Safe_Release(m_pAtCurve);
}