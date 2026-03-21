#include "Lake.h"
#include "GameInstance.h"
CLake::CLake(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CLake::CLake(const CLake& rhs)
	:CGameObject(rhs)
{
}

HRESULT CLake::Init_Prototype()
{
	return S_OK;
}

HRESULT CLake::Init(void* pArg)
{
	m_Desc = *reinterpret_cast<WATER_DESC*>(pArg);

	if (FAILED(Add_Component()))
		return E_FAIL;
	
	m_pTransformCom->Set_Position(m_Desc.vPos);

	_vec3 Scale = _vec3(m_Desc.vSize.x, m_Desc.vSize.y, 1.f);
	m_pTransformCom->Set_Scale(Scale);

	_vec4 vRight = m_pTransformCom->Get_State(State::Right);
	m_pTransformCom->Rotation(vRight, -90.f);


	return S_OK;
}

void CLake::Tick(_float fTimeDelta)
{

	//if (m_pGameInstance->Key_Down(DIK_K))
	//	m_Desc.fReflectionScale += 0.01f;
	//if (m_pGameInstance->Key_Down(DIK_I))
	//	m_Desc.fReflectionScale -= 0.01f;


	//if (m_pGameInstance->Key_Down(DIK_L))
	//	m_Desc.fRefractionScale += 0.01f;
	//if (m_pGameInstance->Key_Down(DIK_O))
	//	m_Desc.fRefractionScale -= 0.01f;
	m_pTransformCom->Set_OldMatrix();

	m_Desc.fWaterPos += m_Desc.fWaterSpeed * fTimeDelta;
}

void CLake::Late_Tick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(RG_Water, this);
}

HRESULT CLake::Render()
{

	// Begin을 호출하기 전에 셰이더에 넘겨줘야 한다.
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	// Pass가 하나만 있으므로 index는 0
	if(FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

// 컴포넌트 추가 함수
HRESULT CLake::Add_Component()
{
	// For.Com_Renderer
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		return E_FAIL;
	}

	// For.Com_Shader_Water
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Water"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	{
		return E_FAIL;
	}


	// For.Com_VIBuffer
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect_LookZ"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
	{
		return E_FAIL;
	}

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Water_Normal"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLake::Bind_ShaderResources()
{
	_float Y = m_pTransformCom->Get_CenterPos().y;

	_mat ReflectionViewMat = m_pGameInstance->ChagneViewForReflection(Y);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ReflectionViewMatrix", ReflectionViewMat)))
		return E_FAIL;

	//if (FAILED(m_pShaderCom->Bind_Matrix("g_ReflectionViewMatrix", XMMatrixIdentity())))
	//	return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_OldWorldMatrix(m_pShaderCom, "g_OldWorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_OldViewMatrix", m_pGameInstance->Get_OldViewMatrix())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(TransformType::View))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(TransformType::Proj))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShaderCom, "g_ReflectionTexture", L"Target_Reflection_Final")))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShaderCom, "g_ReflectionTexture", L"Target_Object_Reflection_Diffuse")))
	//	return E_FAIL;


	//if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShaderCom, "g_RefractionTexture", L"Target_Refraction_Final")))
	//	return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fReflectionScale", &m_Desc.fReflectionScale, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fRefractionScale", &m_Desc.fRefractionScale, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fWaterPos", &m_Desc.fWaterPos, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof(_vec4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamNF", &m_pGameInstance->Get_CameraNF(), sizeof(_float2))))
		return E_FAIL;


	return S_OK;
}


CLake* CLake::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	// 원형 객체 생성 -> Initialize_Prototype 을 호출한다
	CLake* pInstance = new CLake(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CLake");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLake::Clone(void* pArg)
{
	// 실사용 객체를 생성한다. -> 복사 생성자 호출

	CLake* pInstance = new CLake(*this);
	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CLake");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLake::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
