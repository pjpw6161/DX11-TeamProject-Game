#include "VTFModel.h"
#include "Bone.h"
#include "Mesh.h"
#include "Texture.h"
#include "Animation.h"
#include "Shader.h"
#include "VIBuffer_Mesh_Instance.h"

_int CVTFModel::m_iNextInstanceID = 0;

CVTFModel::CVTFModel(_dev pDevice, _context pContext)
	: CComponent(pDevice, pContext)
{
}

CVTFModel::CVTFModel(const CVTFModel& rhs)
	: CComponent(rhs)
	, m_eType(rhs.m_eType)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_Meshes(rhs.m_Meshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_iNumAnimations(rhs.m_iNumAnimations)
	, m_Animations(rhs.m_Animations)
	, m_pTexture(rhs.m_pTexture)
	, m_pSRV(rhs.m_pSRV)
	, m_iInstanceID(rhs.m_iInstanceID)
{
	for (auto& pPrototypeAnim : m_Animations)
	{
		Safe_AddRef(pPrototypeAnim);
	}

	for (auto& pPrototypeMesh : m_Meshes)
	{
		Safe_AddRef(pPrototypeMesh);
	}

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMaterials)
		{
			Safe_AddRef(pTexture);
		}
	}

	Safe_AddRef(m_pTexture);
	Safe_AddRef(m_pSRV);

}

HRESULT CVTFModel::Init_Prototype(const string& strFilePath, const _bool& isCOLMesh, _fmatrix PivotMatrix)
{
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	m_eType = ModelType::End;

	_char szDirectory[MAX_PATH]{};
	_char szFileName[MAX_PATH]{};
	_char szTriggerExt[MAX_PATH] = ".animtrigger";
	_char szExt[MAX_PATH]{};

	if (false == isCOLMesh) {
		_splitpath_s(strFilePath.c_str(), nullptr, 0, szDirectory, MAX_PATH, szFileName, MAX_PATH, szExt, MAX_PATH);
		if (!strcmp(szExt, ".hyuntraanimmesh"))
		{
			m_eType = ModelType::Anim;
		}
		else
		{
			m_eType = ModelType::Static;
		}
	}
	else {
		m_eType = ModelType::Collision;
	}

	ifstream ModelFile(strFilePath.c_str(), ios::binary);
	if (ModelFile.is_open())
	{
		if (m_eType == ModelType::Anim)
		{
			if (FAILED(Read_Bones(ModelFile)))
			{
				return E_FAIL;
			}
		}

		if (FAILED(Read_Meshes(ModelFile, m_eType, PivotMatrix)))
		{
			return E_FAIL;
		}

		if (m_eType == ModelType::Anim) {
			for (auto& pMesh : m_Meshes)
				pMesh->Set_Bone_Offset(m_Bones);
		}


		if (FAILED(Read_Materials(ModelFile, strFilePath)))
		{
			return E_FAIL;
		}

		if (m_eType == ModelType::Anim)
		{
			if (FAILED(Read_Animations(ModelFile)))
			{
				return E_FAIL;
			}
		}

		ModelFile.close();

		_uint iMax = 0;
		for (auto& pAnimation : m_Animations)
			iMax = max(iMax, pAnimation->Get_MaxFrame());

		if (FAILED(CreateVTF(iMax)))
			return E_FAIL;
	}
	else
	{
		MSG_BOX("Failed to Open File!");
		return E_FAIL;
	}

	m_iInstanceID = m_iNextInstanceID++;

	return S_OK;
}

HRESULT CVTFModel::Init(void* pArg)
{
	m_PlayAnimDesc.PLAYANIM_DESC();

	return S_OK;
}

HRESULT CVTFModel::Play_Animation(_float fTimeDelta)
{
	m_OldAnimDesc = m_PlayAnimDesc.eCurrent;

	if (m_isAnimChanged)
	{
		if (m_AnimDesc.bSkipInterpolation)
		{
			//선형보간 스킵
			m_PlayAnimDesc.eCurrent = m_PlayAnimDesc.eNext;
			m_PlayAnimDesc.ResetNextAnim();
			m_isAnimChanged = false;
		}
		else if (!m_isInterpolating)
		{
			//선형보간 중이 아니면 선형보간을 시작한다
			m_PlayAnimDesc.SwitchTime = 0.f;
			m_isInterpolating = true;
		}
		else
		{
			//선형보간 중 애니메이션 동작
			m_PlayAnimDesc.SwitchTime += (fTimeDelta * m_AnimDesc.fAnimSpeedRatio);
			m_PlayAnimDesc.SwitchRatio = m_PlayAnimDesc.SwitchTime / m_PlayAnimDesc.SwitchDuration;

			if (m_PlayAnimDesc.SwitchRatio >= 1.f)
			{
				m_PlayAnimDesc.eCurrent = m_PlayAnimDesc.eNext;
				m_PlayAnimDesc.ResetNextAnim();
				m_isAnimChanged = false;
				m_isInterpolating = false;
			}
		}
	}
	else
	{
		CAnimation* pPlayingAnim = m_Animations[m_PlayAnimDesc.eCurrent.iAnimIndex];

		if (m_PlayAnimDesc.eCurrent.fTime == 0.f)
		{
			m_PlayAnimDesc.eCurrent.fTime = m_AnimDesc.fStartAnimPos;
		}

		if (not m_isFinished)
		{
			//애니메이션 동작
			m_PlayAnimDesc.eCurrent.fTime += (fTimeDelta * m_AnimDesc.fAnimSpeedRatio * pPlayingAnim->Get_TickPerSec());

			while (m_PlayAnimDesc.eCurrent.fTime >= 1.f)
			{
				m_PlayAnimDesc.eCurrent.fTime -= 1.f;
				m_PlayAnimDesc.eCurrent.iCurrFrame = (m_PlayAnimDesc.eCurrent.iCurrFrame + 1);
				m_PlayAnimDesc.eCurrent.iNextFrame = (m_PlayAnimDesc.eCurrent.iCurrFrame + 1);
			}

			m_PlayAnimDesc.eCurrent.fRatio = m_PlayAnimDesc.eCurrent.fTime;
		}
		
		if (static_cast<_float>(m_PlayAnimDesc.eCurrent.iCurrFrame) + m_PlayAnimDesc.eCurrent.fTime >=
			(static_cast<_float>(pPlayingAnim->Get_MaxFrame()) - 1.f) * m_AnimDesc.fDurationRatio)
		{
			if (m_AnimDesc.isLoop)
			{	
				//애니메이션이 끝났는데 루프면 처음부터 둥작
				m_PlayAnimDesc.eCurrent.iCurrFrame = static_cast<_uint>(floorf(m_AnimDesc.fStartAnimPos));
				m_PlayAnimDesc.eCurrent.fTime = m_AnimDesc.fStartAnimPos - floorf(m_AnimDesc.fStartAnimPos);
				m_PlayAnimDesc.eCurrent.fRatio = m_PlayAnimDesc.eCurrent.fTime;
				m_PlayAnimDesc.eCurrent.iNextFrame = m_PlayAnimDesc.eCurrent.iCurrFrame + 1;
			}
			else
			{
				//애니메이션이 끝났다
				m_PlayAnimDesc.eCurrent.iCurrFrame = static_cast<_uint>(pPlayingAnim->Get_MaxFrame() - 1.f);
				m_PlayAnimDesc.eCurrent.iNextFrame = m_PlayAnimDesc.eCurrent.iCurrFrame;
				m_PlayAnimDesc.eCurrent.fTime = 0.f;
				m_PlayAnimDesc.eCurrent.fRatio = m_PlayAnimDesc.eCurrent.fTime;
				m_isFinished = true;
			}
		}
		else
		{
			m_isFinished = false;
		}
	}

	return S_OK;
}

void CVTFModel::Set_Animation(ANIM_DESC Animation_Desc)
{
	if (Animation_Desc.iAnimIndex >= m_iNumAnimations)
	{
		Animation_Desc.iAnimIndex = m_iNumAnimations - 1;
	}

	while (static_cast<_uint>(Animation_Desc.fStartAnimPos) >= m_Animations[Animation_Desc.iAnimIndex]->Get_MaxFrame() - 1)
	{
		Animation_Desc.fStartAnimPos -= static_cast<_float>(m_Animations[Animation_Desc.iAnimIndex]->Get_MaxFrame() - 1);
	}

	if (m_AnimDesc.iAnimIndex != Animation_Desc.iAnimIndex or
		Animation_Desc.bRestartAnimation)
	{
		m_isAnimChanged = true;
		m_isFinished = false;
		m_isInterpolating = false;

		m_PlayAnimDesc.eNext.iAnimIndex = Animation_Desc.iAnimIndex;
		m_PlayAnimDesc.eNext.iCurrFrame = static_cast<_uint>(floorf(Animation_Desc.fStartAnimPos));
		m_PlayAnimDesc.eNext.iNextFrame = m_PlayAnimDesc.eNext.iCurrFrame + 1;
		m_PlayAnimDesc.eNext.fTime = Animation_Desc.fStartAnimPos - floorf(Animation_Desc.fStartAnimPos);
		m_PlayAnimDesc.eNext.fRatio = m_PlayAnimDesc.eNext.fTime;
		m_PlayAnimDesc.SwitchDuration = Animation_Desc.fInterpolationTime;
	}

	m_AnimDesc = Animation_Desc;
}

const _bool CVTFModel::IsAnimationFinished(_uint iAnimIndex) const
{
	if (m_PlayAnimDesc.eCurrent.iAnimIndex == iAnimIndex)
	{
		return m_isFinished;
	}
	else
	{
		return false;
	}
}

HRESULT CVTFModel::Bind_Material(CShader* pShader, const _char* pVariableName, _uint iMeshIndex, TextureType eTextureType)
{
	_uint iMatIndex = m_Meshes[iMeshIndex]->Get_MatIndex();

	if (iMatIndex >= m_iNumMaterials)
	{
		return E_FAIL;
	}

	CTexture* pMaterial = m_Materials[iMatIndex].pMaterials[ToIndex(eTextureType)];
	if (!pMaterial)
	{
		return E_FAIL;
	}

	return pMaterial->Bind_ShaderResource(pShader, pVariableName);
}

HRESULT CVTFModel::Bind_Animation(CShader* pShader)
{
	if (FAILED(pShader->Bind_ShaderResourceView("g_BoneTexture", m_pSRV)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVTFModel::Bind_OldAnimation(CShader* pShader)
{
	if (FAILED(pShader->Bind_RawValue("g_OldAnimDesc", &m_OldAnimDesc, sizeof(ANIMTIME_DESC))))
		return E_FAIL;

	return S_OK;
}

HRESULT CVTFModel::Bind_PlayAnimation(CShader* pShader)
{
	if (FAILED(pShader->Bind_RawValue("g_PlayAnimDesc", &m_PlayAnimDesc, sizeof(ANIMTIME_DESC))))
		return E_FAIL;

	return S_OK;
}

HRESULT CVTFModel::Render(_uint iMeshIndex)
{
	if (FAILED(m_Meshes[iMeshIndex]->Render()))
		return E_FAIL;
	return S_OK;
}

HRESULT CVTFModel::Render_Instancing(CVIBuffer_Mesh_Instance*& pInstanceBuffer, CShader*& pShader)
{
	if (FAILED(Bind_Animation(pShader)))
		return E_FAIL;

	for (_uint i = 0; i < m_Meshes.size(); ++i)
	{
		if (FAILED(Bind_Material(pShader, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
			return E_FAIL;
		}

		_bool HasNorTex{};
		if (FAILED(Bind_Material(pShader, "g_NormalTexture", i, TextureType::Normals)))
		{
			HasNorTex = false;
		}
		else
		{
			HasNorTex = true;
		}

		_bool HasMaskTex{};
		if (FAILED(Bind_Material(pShader, "g_MaskTexture", i, TextureType::Normals)))
		{
			HasMaskTex = false;
		}
		else
		{
			HasMaskTex = true;
		}

		if (FAILED(pShader->Bind_RawValue("g_HasNorTex", &HasNorTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (FAILED(pShader->Bind_RawValue("g_HasMaskTex", &HasMaskTex, sizeof _bool)))
		{
			return E_FAIL;
		}

		if (FAILED(pShader->Begin(pShader->Get_PassIndex())))
		{
			return E_FAIL;
		}

		if (FAILED(pInstanceBuffer->Render(m_Meshes[i])))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CVTFModel::Render_Shadow_Instancing(CVIBuffer_Mesh_Instance*& pInstanceBuffer, CShader*& pShader)
{
	if (FAILED(Bind_Animation(pShader)))
		return E_FAIL;

	for (_uint i = 0; i < m_Meshes.size(); ++i)
	{
		if (FAILED(Bind_Material(pShader, "g_DiffuseTexture", i, TextureType::Diffuse)))
		{
			return E_FAIL;
		}

		if (FAILED(pShader->Begin(2)))
		{
			return E_FAIL;
		}

		if (FAILED(pInstanceBuffer->Render(m_Meshes[i])))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CVTFModel::Read_Bones(ifstream& File)
{
	_uint iNumBones{};
	File.read(reinterpret_cast<_char*>(&iNumBones), sizeof _uint);

	for (size_t i = 0; i < iNumBones; i++)
	{
		CBone* pBone = CBone::Create(File);
		if (!pBone)
		{
			MSG_BOX("Failed to Read Bone!");
			return E_FAIL;
		}
		m_Bones.push_back(pBone);
	}

	return S_OK;
}

HRESULT CVTFModel::Read_Meshes(ifstream& File, const ModelType& eType, _fmatrix PivotMatrix)
{
	File.read(reinterpret_cast<_char*>(&m_iNumMeshes), sizeof _uint);
	m_Meshes.reserve(m_iNumMeshes);

	for (size_t i = 0; i < m_iNumMeshes; i++)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, eType, File, PivotMatrix);
		if (!pMesh)
		{
			MSG_BOX("Failed to Read Meshes!");
			return E_FAIL;
		}
		m_Meshes.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CVTFModel::Read_Animations(ifstream& File)
{
	File.read(reinterpret_cast<_char*>(&m_iNumAnimations), sizeof _uint);

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		CAnimation* pAnimation = CAnimation::Create(File, m_Bones, m_PivotMatrix);
		if (!pAnimation)
		{
			MSG_BOX("Failed to Read Animations!");
			return E_FAIL;
		}
		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

HRESULT CVTFModel::Read_Materials(ifstream& File, const string& strFilePath)
{
	_char szMatFilePath[MAX_PATH]{};
	_char szFullPath[MAX_PATH]{};

	_splitpath_s(strFilePath.c_str(), nullptr, 0, szMatFilePath, MAX_PATH, nullptr, 0, nullptr, 0);
	strcat_s(szMatFilePath, "../Texture/");

	File.read(reinterpret_cast<_char*>(&m_iNumMaterials), sizeof _uint);

	for (size_t i = 0; i < m_iNumMaterials; i++)
	{
		Model_Material Material{};
		_uint iNameSize{};
		_char* pFileName{};

		for (size_t j = 0; j < ToIndex(TextureType::End); j++)
		{
			File.read(reinterpret_cast<_char*>(&iNameSize), sizeof _uint);
			if (iNameSize == 1)
			{
				continue;
			}

			pFileName = new _char[iNameSize];
			File.read(pFileName, iNameSize);
			if (strlen(szMatFilePath) + strlen(pFileName) >= sizeof(szFullPath))
			{
				MSG_BOX("Name Is Too Long!");
			}
			strcpy_s(szFullPath, szMatFilePath);
			strcat_s(szFullPath, pFileName);
			Safe_Delete_Array(pFileName);

			_tchar szTexturePath[MAX_PATH]{};
			MultiByteToWideChar(CP_ACP, 0, szFullPath, static_cast<_int>(strlen(szFullPath)), szTexturePath, MAX_PATH);

			Material.pMaterials[j] = CTexture::Create(m_pDevice, m_pContext, szTexturePath);
			if (!Material.pMaterials[j])
			{
				MSG_BOX("Failed to Create Texture from Model!");
				return E_FAIL;
			}
		}

		m_Materials.push_back(Material);
	}

	return S_OK;
}

HRESULT CVTFModel::CreateVTF(_uint MaxFrame)
{


	ANIMTRANS_ARRAY* pAnimTransform = new ANIMTRANS_ARRAY[m_iNumAnimations];

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		if (FAILED(CreateAnimationTransform(i, pAnimTransform)))
			return E_FAIL;
	}

	D3D11_TEXTURE2D_DESC Desc = {};
	Desc.Width = m_Bones.size() * 4;
	Desc.Height = MaxFrame;
	Desc.ArraySize = m_iNumAnimations;
	Desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	Desc.Usage = D3D11_USAGE_IMMUTABLE;
	Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	Desc.MipLevels = 1;
	Desc.SampleDesc.Count = 1;

	const _uint BoneMatrixSize = m_Bones.size() * sizeof(_mat);
	const _uint AnimationSize = BoneMatrixSize * MaxFrame;
	void* AllAnimationPtr = malloc(AnimationSize * m_iNumAnimations);

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		_uint iAnimSize = i * AnimationSize;

		BYTE* AnimationPtr = reinterpret_cast<BYTE*>(AllAnimationPtr) + iAnimSize;

		for (size_t j = 0; j < MaxFrame; j++)
		{
			void* Ptr = AnimationPtr + j * BoneMatrixSize;
			memcpy(Ptr, pAnimTransform[i].TransformArray[j].data(), BoneMatrixSize);
		}
	}

	vector<D3D11_SUBRESOURCE_DATA> SubResourceData(m_iNumAnimations);

	for (size_t i = 0; i < m_iNumAnimations; i++)
	{
		void* Ptr = reinterpret_cast<BYTE*>(AllAnimationPtr) + i * AnimationSize;
		SubResourceData[i].pSysMem = Ptr;
		SubResourceData[i].SysMemPitch = BoneMatrixSize;
		SubResourceData[i].SysMemSlicePitch = AnimationSize;
	}

	if (FAILED(m_pDevice->CreateTexture2D(&Desc, SubResourceData.data(), &m_pTexture)))
		return E_FAIL;

	free(AllAnimationPtr);

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Format = Desc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	SRVDesc.Texture2DArray.MipLevels = 1;
	SRVDesc.Texture2DArray.ArraySize = m_iNumAnimations;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture, &SRVDesc, &m_pSRV)))
		return E_FAIL;

	Safe_Delete_Array(pAnimTransform);

	return S_OK;
}

void CVTFModel::Apply_TransformToActor(_fmatrix WorldMatrix)
{
	for (auto& pMesh : m_Meshes)
	{
		pMesh->Apply_TransformToActor(WorldMatrix);
	}
}

HRESULT CVTFModel::CreateAnimationTransform(_uint iIndex, ANIMTRANS_ARRAY* pAnimTransform)
{
	CAnimation* pAnimation = m_Animations[iIndex];

	for (size_t i = 0; i < pAnimation->Get_MaxFrame(); i++)
	{
		if (FAILED(pAnimation->Prepare_Animation(m_Bones, i)))
			return E_FAIL;


		for (size_t j = 0; j < m_Bones.size(); j++)
		{
			m_Bones[j]->Update_CombinedMatrix(m_Bones);

			pAnimTransform[iIndex].TransformArray[i][j] = m_Bones[j]->Get_OffsetMatrix() * *(m_Bones[j]->Get_CombinedMatrix()) * m_PivotMatrix;
		}
	}

	return S_OK;
}

CVTFModel* CVTFModel::Create(_dev pDevice, _context pContext, const string& strFilePath, const _bool& isCOLMesh, _fmatrix PivotMatrix)
{
	CVTFModel* pInstance = new CVTFModel(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(strFilePath, isCOLMesh, PivotMatrix)))
	{
		MSG_BOX("Failed to Create : CVTFModel");
	}

	return pInstance;
}

CComponent* CVTFModel::Clone(void* pArg)
{
	CVTFModel* pInstance = new CVTFModel(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CVTFModel");
	}

	return pInstance;
}

void CVTFModel::Free()
{
	__super::Free();

	for (auto& pBone : m_Bones)
	{
		Safe_Release(pBone);
	}
	m_Bones.clear();

	for (auto& pAinmation : m_Animations)
	{
		Safe_Release(pAinmation);
	}
	m_Animations.clear();

	for (auto& pMesh : m_Meshes)
	{
		Safe_Release(pMesh);
	}
	m_Meshes.clear();

	for (auto& Material : m_Materials)
	{
		for (auto& pTexture : Material.pMaterials)
		{
			Safe_Release(pTexture);
		}
	}
	m_Materials.clear();

	Safe_Release(m_pTexture);
	Safe_Release(m_pSRV);
}
