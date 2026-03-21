#include "Camera_CutScene.h"
#include "Camera_Manager.h"
//#include "Trigger_Manager.h"

CCamera_CutScene::CCamera_CutScene(_dev pDevice, _context pContext)
	: CCamera(pDevice, pContext)
{

}

CCamera_CutScene::CCamera_CutScene(const CCamera_CutScene& rhs)
	: CCamera(rhs)
	, m_pTrigger_Manager(CTrigger_Manager::Get_Instance())
{
	Safe_AddRef(m_pTrigger_Manager);
}

HRESULT CCamera_CutScene::Init_Prototype()
{
	return S_OK;
}

HRESULT CCamera_CutScene::Init(void* pArg)
{

	if (FAILED(__super::Init(pArg)))
	{
		return E_FAIL;
	}

	m_iNextSectionIndex = 0;
	m_iCurrentSectionIndex = 0;
	m_fCutSceneSpeed = 10.f;
	m_fTimeDeltaAcc = 0.f;
	m_iLastFrame = 0;

	m_pCam_Manager = CCamera_Manager::Get_Instance();
	Safe_AddRef(m_pCam_Manager);

	return S_OK;
}

void CCamera_CutScene::Tick(_float fTimeDelta)
{
	if (m_pCam_Manager->Get_CameraModeIndex() != CM_CUTSCENE)
	{
		m_iFrame = 0;
		m_fTimeDeltaAcc = 0.f;
		m_iTotalFrame = 0;
		m_fTotalTimeDeltaAcc = 0.f;
		m_iCurrentSectionIndex = 0;
		m_iNextSectionIndex = 0;
		m_isPlayCutScene = false;

		if (!m_CameraAtList.empty() || !m_CameraEyeList.empty())
		{
			for (auto pCurve : m_CameraAtList)
			{
				Safe_Release(pCurve);
			}
			m_CameraAtList.clear();

			for (auto pCurve : m_CameraEyeList)
			{
				Safe_Release(pCurve);
			}
			m_CameraEyeList.clear();
		}
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

		for (auto pCurve : m_CameraAtList)
		{
			Safe_Release(pCurve);
		}
		m_CameraAtList.clear();

		for (auto pCurve : m_CameraEyeList)
		{
			Safe_Release(pCurve);
		}
		m_CameraEyeList.clear();

		m_pCam_Manager->Set_CameraModeIndex(CM_MAIN);
	}
	m_pGameInstance->Set_CameraNF(_float2(m_fNear, m_fFar));

	if (m_isPlayCutScene == false)
	{
		CutScene_Registration(m_pTrigger_Manager->Get_CutScene_Path());
		m_iFrame = 0;
		m_fTimeDeltaAcc = 0.f;
		m_iTotalFrame = 0;
		m_fTotalTimeDeltaAcc = 0.f;
		m_iCurrentSectionIndex = 0;
		m_iNextSectionIndex = 0;
		m_isPlayCutScene = true;
	}
	else
		Play_Camera(fTimeDelta);

	if (m_pTrigger_Manager->Get_Shake_Camera() == true)
	{
		m_pCam_Manager->Set_ShakeCam(true, 0.5f);
	}

	__super::Tick(fTimeDelta);
}

HRESULT CCamera_CutScene::Add_Eye_Curve(_mat matPoints, _float fCurveSpeed)
{
	_uint	iCutSceneType = CCutScene_Curve::SECTION_TYPE_EYE;

	SectionInfo SectionInfo;
	SectionInfo.mCutSceneMatrix = matPoints;
	SectionInfo.iSectionType = iCutSceneType;
	//SectionInfo.ppCurve = &m_pEyeCurve;

	m_pEyeCurve = static_cast<class CCutScene_Curve*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Camera_Curve"), &SectionInfo));

	m_pEyeCurve->Set_SectionSpeed(fCurveSpeed);
	m_pEyeCurve->Set_ControlPoints(matPoints);
	m_CameraEyeList.push_back(static_cast<class CCutScene_Curve*>(m_pEyeCurve));
	//Safe_AddRef(m_pEyeCurve);
	m_pEyeCurve = nullptr;
	return S_OK;
}

HRESULT CCamera_CutScene::Add_At_Curve(_mat matPoints)
{
	_uint	iCurrentLevel = m_pGameInstance->Get_CurrentLevelIndex();
	_uint	iCutSceneType = CCutScene_Curve::SECTION_TYPE_AT;

	SectionInfo SectionInfo;
	SectionInfo.mCutSceneMatrix = matPoints;
	SectionInfo.iSectionType = iCutSceneType;
	//SectionInfo.ppCurve = &m_pAtCurve;

	m_pAtCurve = static_cast<class CCutScene_Curve*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Camera_Curve"), &SectionInfo));

	m_pAtCurve->Set_SectionSpeed(5.f);
	m_pAtCurve->Set_ControlPoints(matPoints);
	m_CameraAtList.push_back(m_pAtCurve);
	//Safe_AddRef(m_pAtCurve);
	m_pAtCurve = nullptr;

	return S_OK;
}

void CCamera_CutScene::Play_Camera(_float fTimeDelta)
{
	if (m_CameraEyeList.size() > 0)
	{
		m_iSectionCount = (_uint)m_CameraEyeList.size();
		m_iLastFrame = m_CameraEyeList.front()->Get_CurveSize() * m_iSectionCount;
	}

	if (m_pTrigger_Manager->Is_BreakLoop())
	{
		for (auto pCurve : m_CameraAtList)
		{
			//pCurve->Kill();
			Safe_Release(pCurve);
		}
		m_CameraAtList.clear();

		for (auto pCurve : m_CameraEyeList)
		{
			//pCurve->Kill();
			Safe_Release(pCurve);
		}
		m_CameraEyeList.clear();

		m_pTrigger_Manager->LoopBroken();
		m_isPlayCutScene = false;

		return;
	}

	if (!m_CameraEyeList.empty() && m_iFrame > m_CameraEyeList.front()->Get_CurveSize() - 2)
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
			m_iFrame = 0;
			m_fTimeDeltaAcc = 0.f;
			m_iTotalFrame = 0;
			m_fTotalTimeDeltaAcc = 0.f;
			m_iCurrentSectionIndex = 0;
			m_iNextSectionIndex = 0;
			m_isPlayCutScene = false;

			for (auto pCurve : m_CameraAtList)
			{
				pCurve->Kill();
				Safe_Release(pCurve);
			}
			m_CameraAtList.clear();

			for (auto pCurve : m_CameraEyeList)
			{
				pCurve->Kill();
				Safe_Release(pCurve);
			}
			m_CameraEyeList.clear();
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

HRESULT CCamera_CutScene::CutScene_Registration(const wstring& strDataPath)
{
	//const TCHAR* pGetPath = L"../Bin/Data/test_CutScene.dat";
	const TCHAR* pGetPath = strDataPath.c_str();

	std::ifstream inFile(pGetPath, std::ios::binary);

	if (!inFile.is_open())
	{
		MessageBox(g_hWnd, L"../Bin/Data/파일을 찾지 못했습니다.", L"파일 로드 실패", MB_OK);
		return E_FAIL;
	}

	_uint InputNameSize;
	string InputName;
	inFile.read(reinterpret_cast<char*>(&InputNameSize), sizeof(_uint));
	inFile.read(reinterpret_cast<char*>(&InputName[0]), sizeof(InputNameSize));

	_uint iEyeSize;
	_uint iAtSize;
	inFile.read(reinterpret_cast<char*>(&iEyeSize), sizeof(_uint));
	inFile.read(reinterpret_cast<char*>(&iAtSize), sizeof(_uint));

	_mat m_mCameraEyePoint{};
	_mat m_mCameraAtPoint{};
	// 카메라 Eye
	_vec4 vEyeStartCurved{};
	_vec4 vEyeStartPos{};
	_vec4 vEyeEndPos{};
	_vec4 vEyeEndCurved{};
	// 카메라 At
	_vec4 vAtStartCurved{};
	_vec4 vAtStartPos{};
	_vec4 vAtEndPos{};
	_vec4 vAtEndCurved{};
	_float fEyeSpeed{};
	_float fCameraSpeed{};

	for (_uint i = 0; i < iEyeSize; i++)
	{

		inFile.read(reinterpret_cast<char*>(&vEyeStartCurved), sizeof(_vec4));
		m_mCameraEyePoint.Right(vEyeStartCurved);
		inFile.read(reinterpret_cast<char*>(&vEyeStartPos), sizeof(_vec4));
		m_mCameraEyePoint.Up(vEyeStartPos);
		inFile.read(reinterpret_cast<char*>(&vEyeEndPos), sizeof(_vec4));
		m_mCameraEyePoint.Look(vEyeEndPos);
		inFile.read(reinterpret_cast<char*>(&vEyeEndCurved), sizeof(_vec4));
		m_mCameraEyePoint.Position(vEyeEndCurved);

		inFile.read(reinterpret_cast<char*>(&fCameraSpeed), sizeof(_float));

		if (FAILED(Add_Eye_Curve(m_mCameraEyePoint, fCameraSpeed)))
		{
			return E_FAIL;
		}

	}
	for (_uint i = 0; i < iAtSize; ++i)
	{
		inFile.read(reinterpret_cast<char*>(&vAtStartCurved), sizeof(_vec4));
		m_mCameraAtPoint.Right(vAtStartCurved);
		inFile.read(reinterpret_cast<char*>(&vAtStartPos), sizeof(_vec4));
		m_mCameraAtPoint.Up(vAtStartPos);
		inFile.read(reinterpret_cast<char*>(&vAtEndPos), sizeof(_vec4));
		m_mCameraAtPoint.Look(vAtEndPos);
		inFile.read(reinterpret_cast<char*>(&vAtEndCurved), sizeof(_vec4));
		m_mCameraAtPoint.Position(vAtEndCurved);

		if (FAILED(Add_At_Curve(m_mCameraAtPoint)))
		{
			return E_FAIL;
		}
	}

	inFile.close();

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

	//if (!m_CameraAtList.empty())
	//{
		//for (int i = 0; i < m_CameraAtList.size(); i++)
		//{
		//	Safe_Release(m_CameraAtList[i]);
		//}
		for (auto iter : m_CameraAtList)
		{
			Safe_Release(iter);
		}
		m_CameraAtList.clear();
	//}
	//if (!m_CameraEyeList.empty())
	//{
		//for (int i = 0; i < m_CameraEyeList.size(); i++)
		//{
		//	Safe_Release(m_CameraEyeList[i]);
		//}
		for (auto iter : m_CameraEyeList)
		{
			Safe_Release(iter);
		}
		m_CameraEyeList.clear();
	//}

	Safe_Release(m_pEyeCurve);
	Safe_Release(m_pAtCurve);
	Safe_Release(m_pTrigger_Manager);
	Safe_Release(m_pCam_Manager);
}