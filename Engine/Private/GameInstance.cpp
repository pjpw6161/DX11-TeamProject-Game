#include "GameInstance.h"
#include "Graphic_Device.h"
#include "Timer_Manager.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Picking.h"
#include "Light_Manager.h"
#include "Font_Manager.h"
#include "Frustum.h"
#include "Collision_Manager.h"
#include "RenderTarget_Manager.h"
#include "Cascade_Manager.h"
#include "Video_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance);

CGameInstance::CGameInstance()
{
}

HRESULT CGameInstance::Init_Engine(_uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, _Inout_ _dev* ppDevice, _Inout_ _context* ppContext)
{
	m_pGraphic_Device = CGraphic_Device::Create(GraphicDesc, ppDevice, ppContext);
	if (!m_pGraphic_Device)
	{
		return E_FAIL;
	}

	m_pTimer_Manager = CTimer_Manager::Create();
	if (!m_pTimer_Manager)
	{
		return E_FAIL;
	}

	m_pLevel_Manager = CLevel_Manager::Create();
	if (!m_pLevel_Manager)
	{
		return E_FAIL;
	}

	m_pObject_Manager = CObject_Manager::Create(iNumLevels);
	if (!m_pObject_Manager)
	{
		return E_FAIL;
	}

	m_pComponent_Manager = CComponent_Manager::Create(iNumLevels, *ppDevice, *ppContext);
	if (!m_pComponent_Manager)
	{
		return E_FAIL;
	}

	m_pInput_Manager = CInput_Device::Create(GraphicDesc.hInst, GraphicDesc.hWnd);
	if (!m_pInput_Manager)
	{
		return E_FAIL;
	}

	m_pLight_Manager = CLight_Manager::Create(iNumLevels);
	if (!m_pLight_Manager)
	{
		return E_FAIL;
	}

	m_pFont_Manager = CFont_Manager::Create(*ppDevice, *ppContext);
	if (!m_pFont_Manager)
	{
		return E_FAIL;
	}

	m_pFrustum = CFrustum::Create();
	if (!m_pFrustum)
	{
		return E_FAIL;
	}

	m_pCollision_Manager = CCollision_Manager::Create();
	if (!m_pCollision_Manager)
	{
		return E_FAIL;
	}

	m_pRenderTarget_Manager = CRenderTarget_Manager::Create(*ppDevice, *ppContext);
	if (!m_pRenderTarget_Manager)
	{
		return E_FAIL;
	}

	m_pPipeLine = CPipeLine::Create();
	if (!m_pPipeLine)
	{
		return E_FAIL;
	}

	m_pPicking = CPicking::Create(*ppDevice, *ppContext, GraphicDesc.hWnd, GraphicDesc.iWinSizeX, GraphicDesc.iWinSizeY);
	if (!m_pPicking)
	{
		return E_FAIL;
	}

	m_pPhysX_Manager = CPhysX_Manager::Create(*ppDevice, *ppContext);
	if (!m_pPhysX_Manager)
	{
		return E_FAIL;
	}

	m_pVideo_Manager = CVideo_Manager::Create(GraphicDesc.hWnd, _uint2(GraphicDesc.iWinSizeX, GraphicDesc.iWinSizeY));
	if (!m_pVideo_Manager)
	{
		return E_FAIL;
	}

	m_pCascade_Manager = CCascade_Manager::Create(*ppDevice, *ppContext);
	if (!m_pCascade_Manager)
	{
		return E_FAIL;
	}

	m_vecLevelInvalid.resize(iNumLevels);

	return S_OK;
}

void CGameInstance::Tick_Engine(_float fTimeDelta)
{
	if (!m_pLevel_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pLevel_Manager is NULL");
	}
	if (!m_pObject_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pObject_Manager is NULL");
	}

	m_pInput_Manager->Update_InputDev();

	if (Key_Down(DIK_F1, InputChannel::Engine))
	{
		if (m_bSkipDebugRender)
		{
			m_bSkipDebugRender = false;
		}
		else
		{
			m_bSkipDebugRender = true;
		}
	}

	if (Key_Down(DIK_F5, InputChannel::Engine)) 
	{
		m_bTurnOnShadow = !m_bTurnOnShadow;
	}

	if (Is_Playing_Video())
	{
	#ifdef _DEBUG
		Print_StringStream();
	#endif // _DEBUG

		return;
	}

	m_pLevel_Manager->Tick(fTimeDelta);
	m_pObject_Manager->Tick(fTimeDelta);
	if (m_Function_Tick_FX)
	{
		m_Function_Tick_FX(fTimeDelta);
	}
	m_pPipeLine->Tick();
	m_pPicking->Tick();

	m_pFrustum->Tick();

	if (true == m_bTurnOnShadow)
		m_pCascade_Manager->Update_Cascade();

	m_pObject_Manager->Release_DeadObjects();
	m_pObject_Manager->Late_Tick(fTimeDelta);
	m_pInput_Manager->Late_Update_InputDev();
	if (m_pSound_Manager)
	{
		m_pSound_Manager->Update();
	}
	if (m_Function_LateTick_FX)
	{
		m_Function_LateTick_FX(fTimeDelta);
	}

	m_pLight_Manager->Tick(m_iLevelIndex, fTimeDelta);

#ifdef _DEBUG
	Print_StringStream();
#endif // _DEBUG

}

void CGameInstance::Clear(_uint iLevelIndex)
{
	if (!m_pObject_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pObject_Manager is NULL");
	}
	if (!m_pComponent_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pComponent_Manager is NULL");
	}


	m_pObject_Manager->Clear(iLevelIndex);
	m_pComponent_Manager->Clear(iLevelIndex);
	if (m_Function_Clear_FX)
		m_Function_Clear_FX(iLevelIndex);
}

HRESULT CGameInstance::Clear_BackBuffer_View(_vec4 vClearColor)
{
	if (!m_pGraphic_Device)
	{
		MSG_BOX("FATAL ERROR : m_pGraphic_Device is NULL");
	}

	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (!m_pGraphic_Device)
	{
		MSG_BOX("FATAL ERROR : m_pGraphic_Device is NULL");
	}

	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (!m_pGraphic_Device)
	{
		MSG_BOX("FATAL ERROR : m_pGraphic_Device is NULL");
	}

	return m_pGraphic_Device->Present();
}

ID3D11Resource* CGameInstance::Get_BackBufferTexture()
{
	if (!m_pGraphic_Device)
	{
		MSG_BOX("FATAL ERROR : m_pGraphic_Device is NULL");
		return nullptr;
	}

	return m_pGraphic_Device->Get_BackBufferTexture();
}

HRESULT CGameInstance::Add_Timer(const wstring& strTimerTag)
{
	if (!m_pTimer_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pTimer_Manager is NULL");
	}

	return m_pTimer_Manager->Add_Timer(strTimerTag);
}

_float CGameInstance::Compute_TimeDelta(const wstring& strTimerTag)
{
	if (!m_pTimer_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pTimer_Manager is NULL");
	}

	return m_pTimer_Manager->Compute_TimeDelta(strTimerTag);
}

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel* pNextLevel)
{
	if (!m_pLevel_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pLevel_Manager is NULL");
	}

	return m_pLevel_Manager->Open_Level(iLevelIndex, pNextLevel);
}

HRESULT CGameInstance::Render()
{
	if (!m_pLevel_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pLevel_Manager is NULL");
	}

	return m_pLevel_Manager->Render();
}

HRESULT CGameInstance::Add_Prototype_GameObejct(const wstring& strPrototypeTag, CGameObject* pPrototype)
{
	if (!m_pObject_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pObject_Manager is NULL");
	}

	return m_pObject_Manager->Add_Prototype(strPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Add_Layer(_uint iLevelIndex, const wstring strLayerTag, const wstring& strPrototypeTag, void* pArg)
{
	if (!m_pObject_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pObject_Manager is NULL");
	}

	return m_pObject_Manager->Add_Layer(iLevelIndex, strLayerTag, strPrototypeTag, pArg);
}

CGameObject* CGameInstance::Clone_Object(const wstring& strPrototypeTag, void* pArg)
{
	if (!m_pObject_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pObject_Manager is NULL");
	}

	return m_pObject_Manager->Clone_Object(strPrototypeTag, pArg);
}

CComponent* CGameInstance::Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComponentTag, _uint iIndex)
{
	if (!m_pObject_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pObject_Manager is NULL");
	}

	return m_pObject_Manager->Get_Component(iLevelIndex, strLayerTag, strComponentTag, iIndex);
}

CGameObject* CGameInstance::Find_Prototype(const wstring& strPrototypeTag)
{
	if (!m_pObject_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pObject_Manager is NULL");
	}
	return m_pObject_Manager->Find_Prototype(strPrototypeTag);
}

CLayer* CGameInstance::Find_Layer(_uint iLevelIndex, const wstring& strLayerTag)
{
	if (!m_pObject_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pObject_Manager is NULL");
	}
	return m_pObject_Manager->Find_Layer(iLevelIndex, strLayerTag);
}

_uint CGameInstance::Get_LayerSize(_uint iLevelIndex, const wstring& strLayerTag)
{
	if (!m_pObject_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pObject_Manager is NULL");
	}
	return m_pObject_Manager->Get_LayerSize(iLevelIndex, strLayerTag);
}

HRESULT CGameInstance::Add_Prototype_Component(_uint iLevelIndex, const wstring& strPrototype, CComponent* pPrototype)
{
	if (!m_pComponent_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pComponent_Manager is NULL");
	}

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, strPrototype, pPrototype);
}

CComponent* CGameInstance::Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg)
{
	if (!m_pComponent_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pComponent_Manager is NULL");
	}

	return m_pComponent_Manager->Clone_Component(iLevelIndex, strPrototypeTag, pArg);
}

_bool CGameInstance::Key_Pressing(_ubyte iKey)
{
	if (!m_pInput_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pInput_Manager is NULL");
	}

	return m_pInput_Manager->Key_Pressing(iKey);
}

_bool CGameInstance::Key_Down(_ubyte iKey, InputChannel eInputChannel)
{
	if (!m_pInput_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pInput_Manager is NULL");
	}

	return m_pInput_Manager->Key_Down(iKey, eInputChannel);
}

_bool CGameInstance::Key_Up(_ubyte iKey, InputChannel eInputChannel)
{
	if (!m_pInput_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pInput_Manager is NULL");
	}

	return m_pInput_Manager->Key_Up(iKey, eInputChannel);
}

_bool CGameInstance::Mouse_Pressing(_long iKey)
{
	if (!m_pInput_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pInput_Manager is NULL");
	}

	return m_pInput_Manager->Mouse_Pressing(iKey);
}

_bool CGameInstance::Mouse_Down(_long iKey, InputChannel eInputChannel)
{
	if (!m_pInput_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pInput_Manager is NULL");
	}

	return m_pInput_Manager->Mouse_Down(iKey, eInputChannel);
}

_bool CGameInstance::Mouse_Up(_long iKey, InputChannel eInputChannel)
{
	if (!m_pInput_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pInput_Manager is NULL");
	}

	return m_pInput_Manager->Mouse_Up(iKey, eInputChannel);
}

_long CGameInstance::Get_MouseMove(MouseState eMouseState)
{
	if (!m_pInput_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pInput_Manager is NULL");
	}

	return m_pInput_Manager->Get_MouseMove(eMouseState);
}

_bool CGameInstance::Gamepad_Pressing(GAMPAD_KEY_STATE eKey)
{
	if (!m_pInput_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pInput_Manager is NULL");
	}

	return m_pInput_Manager->Gamepad_Pressing(eKey);
}

_bool CGameInstance::Gamepad_Down(GAMPAD_KEY_STATE eKey, InputChannel eInputChannel)
{
	if (!m_pInput_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pInput_Manager is NULL");
	}

	return m_pInput_Manager->Gamepad_Down(eKey, eInputChannel);
}

_bool CGameInstance::Gamepad_Up(GAMPAD_KEY_STATE eKey, InputChannel eInputChannel)
{
	if (!m_pInput_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pInput_Manager is NULL");
	}

	return m_pInput_Manager->Gamepad_Up(eKey, eInputChannel);
}

const _float& CGameInstance::Gamepad_LTrigger() const
{
	if (!m_pInput_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pInput_Manager is NULL");
	}

	return m_pInput_Manager->Gamepad_LTrigger();
}

const _float& CGameInstance::Gamepad_RTrigger() const
{
	if (!m_pInput_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pInput_Manager is NULL");
	}

	return m_pInput_Manager->Gamepad_RTrigger();
}

const _float2& CGameInstance::Gamepad_LStick() const
{
	if (!m_pInput_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pInput_Manager is NULL");
	}

	return m_pInput_Manager->Gamepad_LStick();
}

const _float2& CGameInstance::Gamepad_RStick() const
{
	if (!m_pInput_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pInput_Manager is NULL");
	}

	return m_pInput_Manager->Gamepad_RStick();
}

void CGameInstance::Gamepad_Vibration(_ushort LMotorSpeed, _ushort RMotorSpeed)
{
	if (!m_pInput_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pInput_Manager is NULL");
	}

	m_pInput_Manager->Gamepad_Vibration(LMotorSpeed, RMotorSpeed);
}

LIGHT_DESC* CGameInstance::Get_LightDesc(_uint iLevelIndex, const wstring& strLightTag)
{
	if (!m_pLight_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pLight_Manager is NULL");
	}

	return m_pLight_Manager->Get_LightDesc(iLevelIndex, strLightTag);
}

HRESULT CGameInstance::Add_Light(_uint iLevelIndex, const wstring& strLightTag, const LIGHT_DESC& LightDesc)
{
	if (!m_pLight_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pLight_Manager is NULL");
	}

	return m_pLight_Manager->Add_Light(iLevelIndex, strLightTag, LightDesc);
}

HRESULT CGameInstance::Delete_Light(_uint iLevelIndex, const wstring& strLightTag, _float fDimmerDuration)
{
	if (!m_pLight_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pLight_Manager is NULL");
	}

	return m_pLight_Manager->Delete_Light(iLevelIndex, strLightTag, fDimmerDuration);
}

HRESULT CGameInstance::Render_Lights(_uint iLevelIndex, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	if (!m_pLight_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pLight_Manager is NULL");
	}

	return m_pLight_Manager->Render(iLevelIndex, pShader, pVIBuffer);
}

HRESULT CGameInstance::Bind_Light_ViewProjMatrix(_uint iLevelIndex, const wstring& strLightTag, CShader* pShader, const _char* pViewVariableName, const _char* pProjVariableName)
{
	if (!m_pLight_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pLight_Manager is NULL");
	}

	return m_pLight_Manager->Bind_ViewProjMatrix(iLevelIndex, strLightTag, pShader, pViewVariableName, pProjVariableName);
}

_vec4 CGameInstance::Get_CameraPos() const
{
	if (!m_pPipeLine)
	{
		MSG_BOX("FATAL ERROR : m_pPipeLine is NULL");
	}

	return m_pPipeLine->Get_CameraPos();
}

_vec4 CGameInstance::Get_CameraLook() const
{
	if (!m_pPipeLine)
	{
		MSG_BOX("FATAL ERROR : m_pPipeLine is NULL");
	}

	return m_pPipeLine->Get_CameraLook();
}

_matrix CGameInstance::Get_Transform(TransformType eState) const
{
	if (!m_pPipeLine)
	{
		MSG_BOX("FATAL ERROR : m_pPipeLine is NULL");
	}

	return m_pPipeLine->Get_Transform(eState);
}

_matrix CGameInstance::Get_Transform_Inversed(TransformType eState) const
{
	if (!m_pPipeLine)
	{
		MSG_BOX("FATAL ERROR : m_pPipeLine is NULL");
	}

	return m_pPipeLine->Get_Transform_Inversed(eState);
}

void CGameInstance::Set_Transform(TransformType eState, const _mat& TransformMatrix)
{
	if (!m_pPipeLine)
	{
		MSG_BOX("FATAL ERROR : m_pPipeLine is NULL");
	}

	m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

void CGameInstance::Set_OldViewMatrix(const _mat& TransformMatrix)
{
	if (!m_pPipeLine)
	{
		MSG_BOX("FATAL ERROR : m_pPipeLine is NULL");
	}

	m_pPipeLine->Set_OldViewMatrix(TransformMatrix);
}

_mat CGameInstance::Get_OldViewMatrix_vec4x4() const
{
	if (!m_pPipeLine)
	{
		MSG_BOX("FATAL ERROR : m_pPipeLine is NULL");
	}

	return m_pPipeLine->Get_OldViewMatrix_vec4x4();
}

_matrix CGameInstance::Get_OldViewMatrix() const
{
	if (!m_pPipeLine)
	{
		MSG_BOX("FATAL ERROR : m_pPipeLine is NULL");
	}

	return m_pPipeLine->Get_OldViewMatrix();
}

_mat CGameInstance::ChagneViewForReflection(_float fWaterHeight)
{
	if (!m_pPipeLine)
	{
		MSG_BOX("FATAL ERROR : m_pPipeLine is NULL");
	}

	return m_pPipeLine->ChagneViewForReflection(fWaterHeight);
}

void CGameInstance::Update_PipeLine()
{
	if (!m_pPipeLine)
	{
		MSG_BOX("FATAL ERROR : m_pPipeLine is NULL");
	}

	return m_pPipeLine->Tick();
}

void CGameInstance::TransformRay_ToLocal(_mat WorldMatrix)
{
	if (!m_pPicking)
	{
		MSG_BOX("FATAL ERROR : m_pPicking is NULL");
	}

	return m_pPicking->TransformRay_ToLocal(WorldMatrix);
}

_bool CGameInstance::Picking_InWorld(_vec4 vPoint1, _vec4 vPoint2, _vec4 vPoint3, _vec3* pPickPos)
{
	if (!m_pPicking)
	{
		MSG_BOX("FATAL ERROR : m_pPicking is NULL");
	}

	return m_pPicking->Picking_InWorld(vPoint1, vPoint2, vPoint3, pPickPos);
}

_bool CGameInstance::Picking_InLocal(_vec4 vPoint1, _vec4 vPoint2, _vec4 vPoint3, _vec4* pPickPos)
{
	if (!m_pPicking)
	{
		MSG_BOX("FATAL ERROR : m_pPicking is NULL");
	}

	return m_pPicking->Picking_InLocal(vPoint1, vPoint2, vPoint3, pPickPos);
}

_bool CGameInstance::Picking_InLocal(_vec4 vPoint1, _vec4 vPoint2, _vec4 vPoint3, _vec4 vNormal, _vec4* pPickPos)
{
	if (!m_pPicking)
	{
		MSG_BOX("FATAL ERROR : m_pPicking is NULL");
	}

	return m_pPicking->Picking_InLocal(vPoint1, vPoint2, vPoint3, vNormal, pPickPos);
}

_float4 CGameInstance::PickingDepth(_float x, _float y)
{
	if (!m_pPicking)
	{
		MSG_BOX("FATAL ERROR : m_pPicking is NULL");
	}

	return m_pPicking->PickingDepth(x, y);
}
_int CGameInstance::FastPicking(_uint x, _uint y)
{
	if (!m_pPicking)
	{
		MSG_BOX("FATAL ERROR : m_pPicking is NULL");
	}

	return m_pPicking->FastPicking(x, y);
}

_vec4 CGameInstance::Compute_MousePicked_Terrain(_float44 matTerrainWorld, _float3* pVerticesPos, _uint iNumVerticesX, _uint iNumVerticesZ)
{
	if (nullptr == m_pPicking)
		return _vec4(0.f, 0.f, 0.f, 0.f);

	return m_pPicking->Compute_MousePicked_Terrain(matTerrainWorld, pVerticesPos, iNumVerticesX, iNumVerticesZ);
}
_vec4 CGameInstance::Compute_MousePicked_MeshTerrain(_float44 matTerrainWorld, vector<VTXNORTEX> vVertices, vector<_ulong> vIndices)
{
	if (nullptr == m_pPicking)
		return _vec4(0.f, 0.f, 0.f, 0.f);

	return m_pPicking->Compute_MousePicked_MeshTerrain(matTerrainWorld, vVertices, vIndices);
}
HRESULT CGameInstance::Ready_Texture2D()
{
	if (nullptr == m_pPicking)
	{
		return E_FAIL;
	}
	return m_pPicking->Ready_Texture2D();
}
HRESULT CGameInstance::Ready_FastPicking()
{
	if (nullptr == m_pPicking)
	{
		return E_FAIL;
	}
	return m_pPicking->Ready_FastPicking();
}
_vec4 CGameInstance::Get_World_Pos()
{
	if (nullptr == m_pPicking)
	{
		return _vec4(0.f, 0.f, 0.f, 0.f);
	}
	return m_pPicking->Get_World_Pos();
}
_vec4 CGameInstance::Get_World_Dir()
{
	if (nullptr == m_pPicking)
	{
		return _vec4(0.f, 0.f, 0.f, 0.f);
	}
	return m_pPicking->Get_World_Dir();
}
HRESULT CGameInstance::Add_Font(const wstring& strFontTag, const wstring& strFilePath)
{
	if (!m_pFont_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pFont_Manager is NULL");
	}

	return m_pFont_Manager->Add_Font(strFontTag, strFilePath);
}

HRESULT CGameInstance::Render_Text(const wstring& strFontTag, const wstring& strText, const _float2& vPosition, _float fScale, _vec4 vColor, _float fRotation, _bool isFront)
{
	if (!m_pFont_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pFont_Manager is NULL");
	}

	return m_pFont_Manager->Render(strFontTag, strText, vPosition, fScale, vColor, fRotation, isFront);
}

_vec2 CGameInstance::Get_TextSize(const wstring& strFontTag, const wstring& strText)
{
	if (!m_pFont_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pFont_Manager is NULL");
	}
	return m_pFont_Manager->Get_TextSize(strFontTag, strText);

}

_bool CGameInstance::IsIn_Fov_World(_vec4 vPos, _float fRange)
{
	if (!m_pFrustum)
	{
		MSG_BOX("FATAL ERROR : m_pFrustum is NULL");
	}

	return m_pFrustum->IsIn_Fov_World(vPos, fRange);
}

void CGameInstance::Transform_ToLocalSpace(_mat vWorldInversed)
{
	if (!m_pFrustum)
	{
		MSG_BOX("FATAL ERROR : m_pFrustum is NULL");
	}

	m_pFrustum->Transform_ToLocalSpace(vWorldInversed);
}

_bool CGameInstance::IsIn_Fov_Local(_vec4 vPos, _float fRange)
{
	if (!m_pFrustum)
	{
		MSG_BOX("FATAL ERROR : m_pFrustum is NULL");
	}

	return m_pFrustum->IsIn_Fov_Local(vPos, fRange);
}

HRESULT CGameInstance::Register_CollisionObject(CGameObject* pObject, CCollider* pHitCollider, _bool IsPlayer, CCollider* AttRangeCollider, CCollider* ParryingCollider)
{
	if (!m_pCollision_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pCollision_Manager is NULL");
	}

	return m_pCollision_Manager->Register_CollisionObject(pObject, pHitCollider, IsPlayer, AttRangeCollider, ParryingCollider);
}

void CGameInstance::Delete_CollisionObject(CGameObject* pObject, _bool IsPlayer)
{
	if (!m_pCollision_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pCollision_Manager is NULL");
	}

	m_pCollision_Manager->Delete_CollisionObject(pObject, IsPlayer);
}

void CGameInstance::Attack_Monster(CCollider* pCollider, _uint iDamage, _uint iDamageType)
{
	if (!m_pCollision_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pCollision_Manager is NULL");
	}

	m_pCollision_Manager->Attack_Monster(pCollider, iDamage, iDamageType);
}

_bool CGameInstance::CheckCollision_Monster(CCollider* pCollider)
{
	if (!m_pCollision_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pCollision_Manager is NULL");
	}

	return m_pCollision_Manager->CheckCollision_Monster(pCollider);
}

_bool CGameInstance::CheckCollision_Parrying(CCollider* pCollider)
{
	if (!m_pCollision_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pCollision_Manager is NULL");
	}

	return m_pCollision_Manager->CheckCollision_Parrying(pCollider);
}

_bool CGameInstance::Attack_Player(CCollider* pCollider, _uint iDamage, _uint iDamageType)
{
	if (!m_pCollision_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pCollision_Manager is NULL");
	}

	return m_pCollision_Manager->Attack_Player(pCollider, iDamage, iDamageType);
}

_bool CGameInstance::CheckCollision_Player(CCollider* pCollider)
{
	if (!m_pCollision_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pCollision_Manager is NULL");
	}

	return m_pCollision_Manager->CheckCollision_Player(pCollider);
}
CCollider* CGameInstance::Get_Nearest_MonsterCollider()
{
	if (!m_pCollision_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pCollision_Manager is NULL");
	}

	return m_pCollision_Manager->Get_Nearest_MonsterCollider();
}
HRESULT CGameInstance::Register_CollisionCulling(CGameObject* pObject, CCollider* pCollider)
{
	if (!m_pCollision_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pCollision_Manager is NULL");
		//return E_FAIL;
	}

	return m_pCollision_Manager->Register_CollisionCulling(pObject, pCollider);
}
_bool CGameInstance::CheckCollision_Culling(CCollider* pCollider)
{
	if (!m_pCollision_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pCollision_Manager is NULL");
		//return false;
	}

	return m_pCollision_Manager->CheckCollision_Culling(pCollider);
}
CollideFace CGameInstance::Get_CollideFace(CCollider* pAABBCollider, CCollider* pShereCollider)
{
	if (!m_pCollision_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pCollision_Manager is NULL");
	}
	
	return m_pCollision_Manager->Get_CollideFace(pAABBCollider, pShereCollider);
}
void CGameInstance::Init_PhysX_Character(CTransform* pTransform, CollisionGroup eGroup, PxCapsuleControllerDesc* pDesc)
{
	if (!m_pPhysX_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pPhysX_Manager is NULL");
	}

	m_pPhysX_Manager->Init_PhysX_Character(pTransform, eGroup, pDesc);
}

void CGameInstance::Init_PhysX_MoveableObject(CTransform* pTransform)
{
	if (!m_pPhysX_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pPhysX_Manager is NULL");
	}

	m_pPhysX_Manager->Init_PhysX_MoveableObject(pTransform);
}

void CGameInstance::Apply_PhysX(CTransform* pTransform)
{
	if (!m_pPhysX_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pPhysX_Manager is NULL");
	}

	m_pPhysX_Manager->Apply_PhysX(pTransform);
}

void CGameInstance::Update_PhysX(CTransform* pTransform)
{
	if (!m_pPhysX_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pPhysX_Manager is NULL");
	}

	m_pPhysX_Manager->Update_PhysX(pTransform);
}

PxRigidStatic* CGameInstance::Cook_StaticMesh(_uint iNumVertices, void* pVertices, _uint iNumIndices, void* pIndices)
{
	if (!m_pPhysX_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pPhysX_Manager is NULL");
	}

	return m_pPhysX_Manager->Cook_StaticMesh(iNumVertices, pVertices, iNumIndices, pIndices);
}

_bool CGameInstance::Raycast(_vec3 vOrigin, _vec3 vDir, _float fDist, PxRaycastBuffer& Buffer)
{
	if (!m_pPhysX_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pPhysX_Manager is NULL");
	}

	return m_pPhysX_Manager->Raycast(vOrigin, vDir, fDist, Buffer);
}

_bool CGameInstance::Raycast(_vec4 vOrigin, _vec4 vDir, _float fDist, PxRaycastBuffer& Buffer, PxQueryFilterData Filter)
{
	if (!m_pPhysX_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pPhysX_Manager is NULL");
	}

	return m_pPhysX_Manager->Raycast(vOrigin, vDir, fDist, Buffer, Filter);
}

_bool CGameInstance::Raycast(_vec4 vOrigin, _vec4 vDir, _float fDist, PxRaycastBuffer& Buffer)
{
	if (!m_pPhysX_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pPhysX_Manager is NULL");
	}

	return m_pPhysX_Manager->Raycast(vOrigin, vDir, fDist, Buffer);
}

void CGameInstance::PhysXTick(_float fTimeDelta)
{
	if (!m_pPhysX_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pPhysX_Manager is NULL");
	}

	m_pPhysX_Manager->Tick(fTimeDelta);
}

#ifdef _DEBUG
HRESULT CGameInstance::Render_PhysX()
{
	if (m_bSkipDebugRender)
	{
		return S_OK;
	}
	if (FAILED(m_pPhysX_Manager->Render()))
	{
		MSG_BOX("Failed to Render PhysX");
		return E_FAIL;
	}

	return S_OK;
}
#endif // _DEBUG

HRESULT CGameInstance::Add_RenderTarget(const wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _vec4& vColor, const _bool isMultiSample)
{
	if (!m_pRenderTarget_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pRenderTarget_Manager is NULL");
	}

	return m_pRenderTarget_Manager->Add_RenderTarget(strTargetTag, iWidth, iHeight, ePixelFormat, vColor, isMultiSample);
}
HRESULT CGameInstance::Add_MRT(const wstring& strMRTTag, const wstring& strTargetTag)
{
	if (!m_pRenderTarget_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pRenderTarget_Manager is NULL");
	}

	return m_pRenderTarget_Manager->Add_MRT(strMRTTag, strTargetTag);
}
HRESULT CGameInstance::Begin_MRT(const wstring& strMRTTag, ID3D11DepthStencilView* pDepthStencillView, const _bool shouldClear)
{
	if (!m_pRenderTarget_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pRenderTarget_Manager is NULL");
	}

	return m_pRenderTarget_Manager->Begin_MRT(strMRTTag, pDepthStencillView, shouldClear);
}
HRESULT CGameInstance::End_MRT()
{
	if (!m_pRenderTarget_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pRenderTarget_Manager is NULL");
	}

	return m_pRenderTarget_Manager->End_MRT();
}

HRESULT CGameInstance::Bind_ShaderResourceView(CShader* pShader, const _char* pVariableName, const wstring& strTargetTag)
{
	if (!m_pRenderTarget_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pRenderTarget_Manager is NULL");
	}

	return m_pRenderTarget_Manager->Bind_ShaderResourceView(pShader, pVariableName, strTargetTag);
}

ID3D11Texture2D* CGameInstance::Get_Texture2D(const wstring& strTargetTag)
{
	if (!m_pRenderTarget_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pRenderTarget_Manager is NULL");
	}

	return m_pRenderTarget_Manager->Get_Texture2D(strTargetTag);
}

ID3D11ShaderResourceView* CGameInstance::Get_SRV(const wstring& strTargetTag)
{
	if (!m_pRenderTarget_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pRenderTarget_Manager is NULL");
	}

	return m_pRenderTarget_Manager->Get_SRV(strTargetTag);
}

HRESULT CGameInstance::Copy_Texture(const wstring& strTargetTag, ID3D11Resource* pTexture)
{
	if (!m_pRenderTarget_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pRenderTarget_Manager is NULL");
	}

	return m_pRenderTarget_Manager->Copy_Texture(strTargetTag, pTexture);
}

#ifdef _DEBUG
HRESULT CGameInstance::Ready_Debug_RT(const wstring& strTargetTag, _float2 vPos, _float2 vSize)
{
	if (!m_pRenderTarget_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pRenderTarget_Manager is NULL");
	}

	return m_pRenderTarget_Manager->Ready_Debug(strTargetTag, vPos, vSize);
}

HRESULT CGameInstance::Render_Debug_RT(const wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
	if (m_bSkipDebugRender)
	{
		return S_OK;
	}
	if (!m_pRenderTarget_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pRenderTarget_Manager is NULL");
	}

	return m_pRenderTarget_Manager->Render_Debug(strMRTTag, pShader, pVIBuffer);
}
#endif // _DEBUG

HRESULT CGameInstance::Init_SoundManager()
{
	m_pSound_Manager = CSound_Manager::Create();
	if (!m_pSound_Manager)
	{
		return E_FAIL;
	}

	return S_OK;
}

_bool CGameInstance::Is_SoundManager_Ready()
{
	return static_cast<_bool>(m_pSound_Manager);
}

_int CGameInstance::Play_Sound(const wstring& strSoundTag, _float fVolume, _bool isLoop, _float fStartPosRatio)
{
	if (!m_pSound_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pSound_Manager is NULL");
	}

	return m_pSound_Manager->Play_Sound(strSoundTag, fVolume, isLoop, fStartPosRatio);
}
void CGameInstance::PlayBGM(const wstring& strSoundTag, float fVolume)
{
	if (!m_pSound_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pSound_Manager is NULL");
	}

	return m_pSound_Manager->PlayBGM(strSoundTag, fVolume);
}
void CGameInstance::StopSound(_uint iChannel)
{
	if (!m_pSound_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pSound_Manager is NULL");
	}

	return m_pSound_Manager->StopSound(iChannel);
}
void CGameInstance::StopAll()
{
	if (!m_pSound_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pSound_Manager is NULL");
	}

	return m_pSound_Manager->StopAll();
}

HRESULT CGameInstance::FadeoutSound(_uint iChannel, _float fTimeDelta, _float fFadeoutSecond, _bool IsReusable, _float fFadeSoundRatio)
{
	if (!m_pSound_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pSound_Manager is NULL");
	}

	return m_pSound_Manager->FadeoutSound(iChannel, fTimeDelta, fFadeoutSecond, IsReusable, fFadeSoundRatio);
}

HRESULT CGameInstance::FadeinSound(_uint iChannel, _float fTimeDelta, _float fFadeinSecond, _float fFadeSoundRatio)
{
	if (!m_pSound_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pSound_Manager is NULL");
	}

	return m_pSound_Manager->FadeinSound(iChannel, fTimeDelta, fFadeinSecond, fFadeSoundRatio);
}

void CGameInstance::Register_CreateEffect_Callback(Func_CreateFX Function)
{
	m_Function_Create_FX = Function;
}

void CGameInstance::Register_DeleteEffect_Callback(Func_DeleteFX Function)
{
	m_Function_Delete_FX = Function;
}

void CGameInstance::Register_Tick_LateTick_Callback(Func_TickFX Tick, Func_TickFX Late_Tick)
{
	m_Function_Tick_FX = Tick;
	m_Function_LateTick_FX = Late_Tick;
}

void CGameInstance::Register_HasCreated_Callback(Func_HasCreatedFX Function)
{
	m_Function_HasCreated = Function;
}

void CGameInstance::Register_Clear_Callback(Func_ClearFX Function)
{
	m_Function_Clear_FX = Function;
}

void CGameInstance::Create_Effect(const wstring& strEffectTag, _mat* pMatrix, const _bool& isFollow)
{
	if (not m_Function_Create_FX)
	{
		MSG_BOX("Function is not ready!!");
		return;
	}
	m_Function_Create_FX(strEffectTag, pMatrix, isFollow);
}

void CGameInstance::Delete_Effect(const void* pMatrix)
{
	if (not m_Function_Delete_FX)
	{
		MSG_BOX("Function is not ready!!");
		return;
	}
	m_Function_Delete_FX(pMatrix);
}

_bool CGameInstance::Has_Created_Effect(const void* pMatrixKey)
{
	return m_Function_HasCreated(pMatrixKey);
}

HRESULT CGameInstance::Play_Video(const wstring& strVideoFilePath)
{
	if (!m_pVideo_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pVideo_Manager is NULL");
	}

	return m_pVideo_Manager->Play_Video(strVideoFilePath);
}

void CGameInstance::Stop_Video()
{
	if (!m_pVideo_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pVideo_Manager is NULL");
	}

	return m_pVideo_Manager->Stop_Video();
}

const _bool CGameInstance::Is_Playing_Video()
{
	if (!m_pVideo_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pVideo_Manager is NULL");
	}

	return m_pVideo_Manager->Is_Playing_Video();
}

void CGameInstance::Set_StopKey(_ubyte iKey)
{
	if (!m_pVideo_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pVideo_Manager is NULL");
	}

	return m_pVideo_Manager->Set_StopKey(iKey);
}

CASCADE_DESC CGameInstance::Get_CascadeDesc()
{
	if (not m_pCascade_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pCascade_Manager is NULL");
		return CASCADE_DESC();
	}

	return m_pCascade_Manager->Get_CascadeDesc();
}

const _float2& CGameInstance::Get_CameraNF() const
{
	return m_vCameraNF;
}

const _uint& CGameInstance::Get_CurrentLevelIndex() const
{
	return m_iLevelIndex;
}

const _float& CGameInstance::Get_TimeRatio() const
{
	return m_fTimeRatio;
}

const _float2& CGameInstance::Get_FogNF() const
{
	return m_vFogNF;
}

const _color& CGameInstance::Get_FogColor() const
{
	return m_vFogColor;
}

const _float& CGameInstance::Get_HellHeight() const
{
	return m_fHellHeight;
}

#ifdef _DEBUG
stringstream& CGameInstance::Get_StringStream()
{
	return m_OutputStream;
}

void CGameInstance::Add_String_to_Stream(const string& strText)
{
	m_OutputStream << strText << endl;
}
void CGameInstance::Set_FPS(const _uint iFPS)
{
	m_iFPS = iFPS;
}
const _uint& CGameInstance::Get_FPS()
{
	return m_iFPS;
}
#endif

_bool CGameInstance::Get_IsPlayingSound(_uint iChannel)
{
	if (!m_pSound_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pSound_Manager is NULL");
	}

	return m_pSound_Manager->Get_IsPlayingSound(iChannel);
}

_float CGameInstance::Get_ChannelVolume(_uint iChannel)
{
	if (!m_pSound_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pSound_Manager is NULL");
	}

	return m_pSound_Manager->GetChannelVolume(iChannel);
}

_bool CGameInstance::Get_IsLoopingSound(_uint iChannel)
{
	if (!m_pSound_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pSound_Manager is NULL");
	}

	return m_pSound_Manager->Get_IsLoopingSound(iChannel);
}

_float CGameInstance::Get_ChannelCurPosRatio(_uint iChannel)
{
	if (!m_pSound_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pSound_Manager is NULL");
	}

	return m_pSound_Manager->Get_CurPosRatio(iChannel);
}

void CGameInstance::Set_CameraNF(const _float2& vCamNF)
{
	m_vCameraNF = vCamNF;
}

void CGameInstance::Set_CurrentLevelIndex(const _uint& iIndex)
{
	m_iLevelIndex = iIndex;
}

void CGameInstance::Set_TimeRatio(const _float fRatio)
{
	m_fTimeRatio = fRatio;
}

void CGameInstance::Set_FogNF(const _float2& vFogNF)
{
	m_vFogNF = vFogNF;
}

void CGameInstance::Set_FogColor(const _color& vFogColor)
{
	m_vFogColor = vFogColor;
}

void CGameInstance::Set_HellHeight(const _float& fHeight)
{
	m_fHellHeight = fHeight;
}

void CGameInstance::Set_ChannelVolume(_uint iChannel, _float fVolume)
{
	if (!m_pSound_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pSound_Manager is NULL");
	}

	return m_pSound_Manager->SetChannelVolume(iChannel, fVolume);
}

void CGameInstance::Set_ChannelStartVolume(_uint iChannel)
{
	if (!m_pSound_Manager)
	{
		MSG_BOX("FATAL ERROR : m_pSound_Manager is NULL");
	}

	return m_pSound_Manager->SetChannelStartVolume(iChannel);
}

void CGameInstance::Set_InputString(const wstring& strInput)
{
	m_strInput = strInput;
}

void CGameInstance::Set_CompleteInputString(const wstring& strInput)
{
	m_strCompleteInput = strInput;
}

void CGameInstance::Set_ComposingInputString(const wstring& strInput)
{
	m_strComposingInput = strInput;
}

void CGameInstance::Set_ComposingState(const _bool isComposing)
{
	m_isComposing = isComposing;

	if (not m_strComposingInput.empty())
	{
		m_isPopInput = false;
	}
}

void CGameInstance::Popback_InputString()
{
	if (m_isPopInput and not m_strCompleteInput.empty())
	{
		m_strCompleteInput.pop_back();
	}

	if (not m_isComposing)
	{
		m_isPopInput = true;
	}
	else
	{
		m_isPopInput = false;
	}
}

const _bool& CGameInstance::IsSkipDebugRendering() const
{
	return m_bSkipDebugRender;
}

const wstring& CGameInstance::Get_InputString()
{
	m_strInput = m_strCompleteInput + m_strComposingInput;
	return m_strInput;
}

const wstring& CGameInstance::Get_CompleteInputString() const
{
	return m_strCompleteInput;
}

void CGameInstance::Level_ShutDown(_uint iCurrentLevel)
{
	if (iCurrentLevel >= m_vecLevelInvalid.size())
	{
		MSG_BOX("Wrong Level Index");
		return;
	}

	m_vecLevelInvalid[iCurrentLevel] = true;
}

_bool CGameInstance::Is_Level_ShutDown(_uint iCurrentLevel)
{
	if (iCurrentLevel >= m_vecLevelInvalid.size())
	{
		MSG_BOX("Wrong Level Index");
		return false;
	}

	return m_vecLevelInvalid[iCurrentLevel];
}

#ifdef _DEBUG
void CGameInstance::Print_StringStream()
{
	if (m_strPrevStream == m_OutputStream.str())
	{
		m_OutputStream = {};
		m_OutputStream.clear();
		return;
	}

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coordScreen = { 0, 0 };
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwConSize;

	GetConsoleScreenBufferInfo(hConsole, &csbi);
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

	FillConsoleOutputCharacter(hConsole, (TCHAR)' ', dwConSize, coordScreen, &cCharsWritten);

	GetConsoleScreenBufferInfo(hConsole, &csbi);
	FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);

	SetConsoleCursorPosition(hConsole, coordScreen);

	m_strPrevStream = m_OutputStream.str();
	cout << m_strPrevStream << flush;
	m_OutputStream = {};
	m_OutputStream.clear();
}
#endif

void CGameInstance::Clear_Managers()
{
	Safe_Release(m_pSound_Manager);
	Safe_Release(m_pCollision_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pTimer_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pPicking);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pRenderTarget_Manager);
	Safe_Release(m_pPhysX_Manager);
	Safe_Release(m_pCascade_Manager);
	Safe_Release(m_pVideo_Manager);
	Safe_Release(m_pInput_Manager);
}

void CGameInstance::Release_Engine()
{
	CGameInstance::Get_Instance()->Clear_Managers();

	CGameInstance::Destroy_Instance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pGraphic_Device);
}
