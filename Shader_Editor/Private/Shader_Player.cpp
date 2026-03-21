#include "Shader_Player.h"


CShader_Player::CShader_Player(_dev pDevice, _context pContext)
	: CGameObject(pDevice,pContext)
{
}

CShader_Player::CShader_Player(const CShader_Player& rhs)
	: CGameObject(rhs)
{
}

HRESULT CShader_Player::Init_Prototype()
{
	return S_OK;
}

HRESULT CShader_Player::Init(void* pArg)
{
	if (FAILED(Add_Components()))
		return E_FAIL;

	m_Animation.isLoop = true;
	m_Animation.bSkipInterpolation = false;
	m_pTransformCom->Set_Scale(_vec3(4.f));
	Place_PartModels();
	m_pTransformCom->Set_Speed(1);

	Change_Parts(PT_BODY, 0);
	Change_Parts(PT_HAIR, 0);
	Change_Parts(PT_FACE, 0);

	return S_OK;
}

void CShader_Player::Tick(_float fTimeDelta)
{
}

void CShader_Player::Late_Tick(_float fTimeDelta)
{
	m_pModelCom->Play_Animation(0.f);
	m_pRendererCom->Add_RenderGroup(RG_NonBlend, this);
}

HRESULT CShader_Player::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	if (m_Body_CurrentIndex >= 0)
		Render_Parts(PT_BODY, m_Body_CurrentIndex);
	if (m_Hair_CurrentIndex >= 0)
		Render_Parts(PT_HAIR, m_Hair_CurrentIndex);
	if (m_Face_CurrentIndex >= 0)
		Render_Parts(PT_FACE, m_Face_CurrentIndex);

	return S_OK;
}

HRESULT CShader_Player::Place_PartModels()
{
	CRealtimeVTFModel::PART_DESC Desc{};
	Desc.ePartType = PT_BODY;

	Desc.FileName = "body0";

	if (FAILED(m_pModelCom->Place_Parts(Desc, true)))
		return E_FAIL;

	Desc.FileName = "body1";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "body2";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "body3";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "body4";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "body5";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.ePartType = PT_HELMET;
	Desc.FileName = "helmet0";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "helmet1";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;
	Desc.FileName = "helmet2";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;
	Desc.FileName = "helmet3";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;
	Desc.FileName = "helmet4";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;
	Desc.FileName = "helmet5";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;
	Desc.ePartType = PT_FACE;
	Desc.FileName = "face0";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "face1";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "face2";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "face3";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "face4";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "face5";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.ePartType = PT_HAIR;
	Desc.FileName = "hair0";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "hair1";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "hair2";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "hair3";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "hair4";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "hair5";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "hair6";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "hair7";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "hair8";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.ePartType = PT_WEAPON;

	Desc.FileName = "bow0";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "bow1";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "bow2";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "bow3";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "bow4";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "sword0";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "sword1";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "sword2";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	Desc.FileName = "sword3";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;
	Desc.FileName = "sword4";

	if (FAILED(m_pModelCom->Place_Parts(Desc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CShader_Player::Add_Info()
{
	return E_NOTIMPL;
}

HRESULT CShader_Player::Render_Parts(PART_TYPE Parts, _uint Index)
{

	for (size_t k = 0; k < m_pModelCom->Get_Num_PartMeshes(Parts, Index); k++)
	{
		if (FAILED(m_pModelCom->Bind_Part_Material(m_pShaderCom, "g_DiffuseTexture", TextureType::Diffuse, (_uint)Parts, (_uint)Index, k)))
			continue;

		_bool HasNorTex{};
		if (FAILED(m_pModelCom->Bind_Part_Material(m_pShaderCom, "g_NormalTexture", TextureType::Normals, (_uint)Parts, (_uint)Index, k)))
		{
			HasNorTex = false;
		}
		else
		{
			HasNorTex = true;
		}

		_bool HasSpecTex{};
		if (FAILED(m_pModelCom->Bind_Part_Material(m_pShaderCom, "g_SpecTexture", TextureType::Shininess, (_uint)Parts, (_uint)Index, k)))
		{
			HasSpecTex = false;
		}
		else
		{
			HasSpecTex = true;
		}

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
			return E_FAIL;

		HasSpecTex = false;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_HasSpecTex", &HasSpecTex, sizeof _bool)))
			return E_FAIL;


		if (FAILED(m_pShaderCom->Begin(5)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render_Part((_uint)Parts, (_uint)Index, k)))
			return E_FAIL;
	}

	return S_OK;
}

void CShader_Player::Change_Parts(PART_TYPE PartsType, _int ChangeIndex)
{
	switch (PartsType)
	{
	case ShaderEdit::PT_HAIR:
		m_Hair_CurrentIndex = ChangeIndex;
		break;
	case ShaderEdit::PT_FACE:
		m_Face_CurrentIndex = ChangeIndex;
		break;
	case ShaderEdit::PT_BODY:
		m_Body_CurrentIndex = ChangeIndex;
		break;
	default:
		break;
	}
}

HRESULT CShader_Player::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), reinterpret_cast<CComponent**>(&m_pRendererCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_ShaderTest, TEXT("Prototype_Component_Shader_RTVTF"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Model_Player"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CShader_Player::Bind_ShaderResources()
{
	// WorldMatrix 바인드
	if (FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	// ViewMatrix 바인드
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform(TransformType::View))))
		return E_FAIL;

	// ProjMatrix 바인드
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform(TransformType::Proj))))
		return E_FAIL;

	// 카메라 Far 바인드
	if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &m_pGameInstance->Get_CameraNF().y, sizeof _float)))
		return E_FAIL;

	// 모션블러용 이전프레임 WorldMatrix 바인드
	if (FAILED(m_pShaderCom->Bind_Matrix("g_OldWorldMatrix", m_OldWorldMatrix)))
		return E_FAIL;

	// 모션블러용 이전프레임 ViewMatrix 바인드
	if (FAILED(m_pShaderCom->Bind_Matrix("g_OldViewMatrix", m_pGameInstance->Get_OldViewMatrix_vec4x4())))
		return E_FAIL;

	// 뼈 바인드
	if (FAILED(m_pModelCom->Bind_Bone(m_pShaderCom)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPos", &m_pGameInstance->Get_CameraPos(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Inversed(TransformType::Proj))))
		return E_FAIL;

	_float4 vColor = _float4(1.f, 1.f, 1.f, 1.f);
	if (FAILED(m_pShaderCom->Bind_RawValue("g_RimColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CShader_Player* CShader_Player::Create(_dev pDevice, _context pContext)
{
	CShader_Player* pInstance = new CShader_Player(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CShader_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CShader_Player::Clone(void* pArg)
{
	CShader_Player* pInstance = new CShader_Player(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CShader_Player");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShader_Player::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
}
