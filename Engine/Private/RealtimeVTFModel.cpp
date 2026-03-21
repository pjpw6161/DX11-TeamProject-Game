#include "RealtimeVTFModel.h"
#include "Bone.h"
#include "Mesh.h"
#include "Texture.h"
#include "Animation.h"
#include "Shader.h"
#include "Part_Model.h"

//트리거
#include "GameInstance.h"

CRealtimeVTFModel::CRealtimeVTFModel(_dev pDevice, _context pContext)
	: CComponent(pDevice, pContext)
{
}

CRealtimeVTFModel::CRealtimeVTFModel(const CRealtimeVTFModel& rhs)
	: CComponent(rhs)
	, m_eType(rhs.m_eType)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Materials(rhs.m_Materials)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_iNumAnimations(rhs.m_iNumAnimations)
	, m_iNumTriggersEffect(rhs.m_iNumTriggersEffect)
	, m_TriggerEffects(rhs.m_TriggerEffects)
	, m_iNumTriggersSound(rhs.m_iNumTriggersSound)
	, m_TriggerSounds(rhs.m_TriggerSounds)
{
	for (auto& pPrototypeBone : rhs.m_Bones) {
		CBone* pBone = pPrototypeBone->Clone();

		m_Bones.push_back(pBone);
	}

	for (auto& pPrototypeAnimation : rhs.m_Animations)
	{
		CAnimation* pAnimation = pPrototypeAnimation->Clone();

		m_Animations.push_back(pAnimation);
	}

	for (auto& pPrototypeMesh : rhs.m_Meshes) {
		CMesh* pMesh = reinterpret_cast<CMesh*>(pPrototypeMesh->Clone());

		m_Meshes.push_back(pMesh);
	}

	for (auto& Material : m_Materials)
		for (auto& pTexture : Material.pMaterials)
			Safe_AddRef(pTexture);

	//for (auto& PrototypePartVector : rhs.m_Parts) {
	//	vector<CPart_Model*> Parts;
	//	for (auto& pPrototypePart : PrototypePartVector.second) {
	//		CPart_Model* pPart = reinterpret_cast<CPart_Model*>(pPrototypePart->Clone());

	//		Parts.push_back(pPart);
	//	}
	//	m_Parts.emplace(PrototypePartVector.first, Parts);
	//}

	for (auto& PrototypePartPair : rhs.m_PrototypeParts) {
		CPart_Model* pPart = reinterpret_cast<CPart_Model*>(PrototypePartPair.second->Clone());
		m_PrototypeParts.emplace(PrototypePartPair.first, pPart);
	}
}

const _uint& CRealtimeVTFModel::Get_NumAnim() const
{
	return m_iNumAnimations;
}

const _uint CRealtimeVTFModel::Get_NumBones() const
{
	return m_Bones.size();
}

vector<class CAnimation*>& CRealtimeVTFModel::Get_Animations()
{
	return m_Animations;
}
_mat CRealtimeVTFModel::Get_PivotMatrix()
{
	return XMLoadFloat4x4(&m_PivotMatrix);
}

CAnimation* CRealtimeVTFModel::Get_Animation(_uint iAnimIndex)
{
	return m_Animations[iAnimIndex];
}

vector<class CBone*>& CRealtimeVTFModel::Get_Bones()
{
	return m_Bones;
}

const _uint CRealtimeVTFModel::Get_NumTriggerEffect() const
{
	return m_iNumTriggersEffect;
}

TRIGGEREFFECT_DESC* CRealtimeVTFModel::Get_TriggerEffect(_uint iTriggerEffectIndex)
{
	return &m_TriggerEffects[iTriggerEffectIndex];
}

vector<TRIGGEREFFECT_DESC>& CRealtimeVTFModel::Get_TriggerEffects()
{
	return m_TriggerEffects;
}
void CRealtimeVTFModel::Add_TriggerEffect(TRIGGEREFFECT_DESC TriggerEffectDesc)
{
	m_iNumTriggersEffect++;
	m_TriggerEffects.push_back(TriggerEffectDesc);
	_mat* pMatrix = new _mat{};
	m_EffectMatrices.push_back(pMatrix);
}

void CRealtimeVTFModel::Delete_TriggerEffect(_uint iTriggerEffectIndex)
{
	m_iNumTriggersEffect--;
	auto Effect_iter = m_TriggerEffects.begin();
	auto Matrix_iter = m_EffectMatrices.begin();
	for (_uint i = 0; i < iTriggerEffectIndex; i++)
	{
		Effect_iter++;
		Matrix_iter++;
	}
	m_pGameInstance->Delete_Effect((*Matrix_iter));
	Safe_Delete((*Matrix_iter));
	m_TriggerEffects.erase(Effect_iter);
	m_EffectMatrices.erase(Matrix_iter);
}

void CRealtimeVTFModel::Reset_TriggerEffects()
{
	m_iNumTriggersEffect = 0;
	m_TriggerEffects.clear();
	for (auto& pEffectMatrix : m_EffectMatrices)
	{
		m_pGameInstance->Delete_Effect(pEffectMatrix);
		Safe_Delete(pEffectMatrix);
	}
	m_EffectMatrices.clear();
}

const _uint CRealtimeVTFModel::Get_NumTriggerSound() const
{
	return m_iNumTriggersSound;
}

TRIGGERSOUND_DESC* CRealtimeVTFModel::Get_TriggerSound(_uint iTriggerSoundIndex)
{
	return &m_TriggerSounds[iTriggerSoundIndex];
}

vector<TRIGGERSOUND_DESC>& CRealtimeVTFModel::Get_TriggerSounds()
{
	return m_TriggerSounds;
}

void CRealtimeVTFModel::Add_TriggerSound(TRIGGERSOUND_DESC TriggerSoundDesc)
{
	m_iNumTriggersSound++;
	m_TriggerSounds.push_back(TriggerSoundDesc);
}

void CRealtimeVTFModel::Delete_TriggerSound(_uint iTriggerSoundIndex)
{
	m_iNumTriggersSound--;
	auto Sound_iter = m_TriggerSounds.begin();
	for (_uint i = 0; i < iTriggerSoundIndex; i++)
	{
		Sound_iter++;
	}
	m_TriggerSounds.erase(Sound_iter);
}

void CRealtimeVTFModel::Reset_TriggerSounds()
{
	m_iNumTriggersSound = 0;
	m_TriggerSounds.clear();
}

HRESULT CRealtimeVTFModel::Init_Prototype(const string& strFilePath, _fmatrix PivotMatrix)
{
	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	m_eType = ModelType::End;

	_char szDirectory[MAX_PATH]{};
	_char szFileName[MAX_PATH]{};
	_char szTriggerExt[MAX_PATH] = ".animtrigger";
	_char szExt[MAX_PATH]{};

	_splitpath_s(strFilePath.c_str(), nullptr, 0, szDirectory, MAX_PATH, szFileName, MAX_PATH, szExt, MAX_PATH);
	if (!strcmp(szExt, ".hyuntraanimmesh"))
	{
		m_eType = ModelType::Anim;
	}
	else
	{
		m_eType = ModelType::Static;
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

		if (m_eType == ModelType::Anim)
		{
			if (FAILED(Read_TriggerEffects(strFilePath)))
			{
				return E_FAIL;
			}

			if (FAILED(Read_TriggerSounds(strFilePath)))
			{
				return E_FAIL;
			}
		}
	}
	else
	{
		MSG_BOX("Failed to Open File!");
		return E_FAIL;
	}



	return S_OK;
}

HRESULT CRealtimeVTFModel::Init(void* pArg, const CRealtimeVTFModel& rhs)
{
	if (FAILED(CreateVTF()))
		return E_FAIL;

	if (pArg != nullptr)
	{
		m_pOwnerTransform = reinterpret_cast<CTransform*>(pArg);
		Safe_AddRef(m_pOwnerTransform);
	}

	for (size_t i = 0; i < m_TriggerEffects.size(); i++)
	{
		_mat* pMatrix = new _mat{};
		m_EffectMatrices.push_back(pMatrix);
	}

	random_device rand;
	m_RandomNumber = _randNum(rand());

	return S_OK;
}

HRESULT CRealtimeVTFModel::Place_Parts(PART_DESC& ePartDesc, _bool isRender)
{

	auto Protoiter = m_PrototypeParts.find(ePartDesc.FileName);

	if (m_PrototypeParts.end() == Protoiter) {
		MSG_BOX("그런 파일 이름 없어용");
		return E_FAIL;
	}

	auto iter = m_Parts.find(ePartDesc.ePartType);

	if (m_Parts.end() == iter) {
		vector<CPart_Model*> PartVec;
		PartVec.push_back(Protoiter->second);

		m_Parts.emplace(ePartDesc.ePartType, PartVec);
	}
	else
		iter->second.push_back(Protoiter->second);

	Protoiter->second->Set_isRender(isRender);

	return S_OK;
}

HRESULT CRealtimeVTFModel::Play_Animation(_float fTimeDelta, _bool OnClientTrigger)
{
	if (true == m_isUsingMotionBlur)
		m_pContext->CopyResource(m_pOldBoneTexture, m_pBoneTexture);
	//트리거 루프
	if ((m_Animations[m_AnimDesc.iAnimIndex]->Get_CurrentAnimPos() + fTimeDelta * m_AnimDesc.fAnimSpeedRatio * m_Animations[m_AnimDesc.iAnimIndex]->Get_TickPerSec()) >=
		(m_Animations[m_AnimDesc.iAnimIndex]->Get_Duration() * m_AnimDesc.fDurationRatio) &&
		m_Animations[m_AnimDesc.iAnimIndex]->Get_CurrentAnimPos() <= (m_Animations[m_AnimDesc.iAnimIndex]->Get_Duration() * m_AnimDesc.fDurationRatio) ||
		m_isAnimChanged)
	{
		m_IsResetTriggers = true;
	}

	m_Animations[m_AnimDesc.iAnimIndex]->Update_TransformationMatrix(m_Bones, fTimeDelta * m_AnimDesc.fAnimSpeedRatio, m_isAnimChanged, m_AnimDesc.isLoop,
		m_AnimDesc.bSkipInterpolation, m_AnimDesc.fInterpolationTime, m_AnimDesc.fDurationRatio, m_AnimDesc.fStartAnimPos, m_AnimDesc.bRewindAnimation);


	vector<_mat> CombinedBones;

	for (auto& pBone : m_Bones) {
		pBone->Update_CombinedMatrix(m_Bones);
		_mat BoneMatrix = pBone->Get_OffsetMatrix() * *(pBone->Get_CombinedMatrix()) * m_PivotMatrix;
		CombinedBones.push_back(BoneMatrix);
	}

	if (FAILED(UpdateBoneTexture(CombinedBones)))
		return E_FAIL;

	CombinedBones.clear();

#pragma region Trigger_Effect
	for (size_t i = 0; i < m_TriggerEffects.size(); i++)
	{
		//이펙트 위치 갱신
		if (m_TriggerEffects[i].IsFollow && m_pGameInstance->Has_Created_Effect(m_EffectMatrices[i]))
		{
			if (m_TriggerEffects[i].IsDeleteRotateToBone)
			{
				_mat BoneMatrix = *m_Bones[m_TriggerEffects[i].iBoneIndex]->Get_CombinedMatrix();
				*m_EffectMatrices[i] = m_TriggerEffects[i].OffsetMatrix * BoneMatrix.Get_RotationRemoved() * m_PivotMatrix * m_pOwnerTransform->Get_World_Matrix();
			}
			else
			{
				*m_EffectMatrices[i] = m_TriggerEffects[i].OffsetMatrix * *m_Bones[m_TriggerEffects[i].iBoneIndex]->Get_CombinedMatrix() * m_PivotMatrix * m_pOwnerTransform->Get_World_Matrix();
			}
		}
		if (m_AnimDesc.iAnimIndex == m_TriggerEffects[i].iStartAnimIndex &&
			m_Animations[m_AnimDesc.iAnimIndex]->Get_CurrentAnimPos() >= m_TriggerEffects[i].fStartAnimPos &&
			m_Animations[m_AnimDesc.iAnimIndex]->Get_CurrentAnimPos() <= m_Animations[m_AnimDesc.iAnimIndex]->Get_Duration() * m_AnimDesc.fDurationRatio &&
			not m_TriggerEffects[i].HasCreated)
		{
			//초기 매트릭스 세팅
			if (m_TriggerEffects[i].IsDeleteRotateToBone)
			{
				_mat BoneMatrix = *m_Bones[m_TriggerEffects[i].iBoneIndex]->Get_CombinedMatrix();
				*m_EffectMatrices[i] = m_TriggerEffects[i].OffsetMatrix * BoneMatrix.Get_RotationRemoved() * m_PivotMatrix * m_pOwnerTransform->Get_World_Matrix();
			}
			else
			{
				*m_EffectMatrices[i] = m_TriggerEffects[i].OffsetMatrix * *m_Bones[m_TriggerEffects[i].iBoneIndex]->Get_CombinedMatrix() * m_PivotMatrix * m_pOwnerTransform->Get_World_Matrix();
			}
			//이펙트 생성
			if (m_TriggerEffects[i].IsClientTrigger)
			{
				if (OnClientTrigger)
				{
					m_pGameInstance->Create_Effect(m_TriggerEffects[i].strEffectName, m_EffectMatrices[i], m_TriggerEffects[i].IsFollow);
					m_TriggerEffects[i].HasCreated = true;
				}
			}
			else
			{
				m_pGameInstance->Create_Effect(m_TriggerEffects[i].strEffectName, m_EffectMatrices[i], m_TriggerEffects[i].IsFollow);
				m_TriggerEffects[i].HasCreated = true;
			}
		}

		//이펙트 제거
		for (size_t j = 0; j < m_TriggerEffects[i].iEndAnimIndices.size(); j++)
		{
			if (m_AnimDesc.iAnimIndex == m_TriggerEffects[i].iEndAnimIndices[j] &&
				m_Animations[m_AnimDesc.iAnimIndex]->Get_CurrentAnimPos() >= m_TriggerEffects[i].fEndAnimPoses[j] &&
				m_pGameInstance->Has_Created_Effect(m_EffectMatrices[i]))
			{
				m_pGameInstance->Delete_Effect(m_EffectMatrices[i]);
			}
		}
	}
#pragma endregion

#pragma region Trigger_Sound
	for (size_t i = 0; i < m_TriggerSounds.size(); i++)
	{	//사운드 생성
		if (m_AnimDesc.iAnimIndex == m_TriggerSounds[i].iStartAnimIndex &&
			m_Animations[m_AnimDesc.iAnimIndex]->Get_CurrentAnimPos() >= m_TriggerSounds[i].fStartAnimPos &&
			m_Animations[m_AnimDesc.iAnimIndex]->Get_CurrentAnimPos() <= m_Animations[m_AnimDesc.iAnimIndex]->Get_Duration() * m_AnimDesc.fDurationRatio &&
			not m_TriggerSounds[i].HasPlayed)
		{
			if (m_TriggerSounds[i].iChannel != -1)
			{
				m_pGameInstance->StopSound(m_TriggerSounds[i].iChannel);
				m_TriggerSounds[i].IsEnding = false;
			}

			if (m_TriggerSounds[i].IsClientTrigger)
			{
				if (OnClientTrigger)
				{
					_int iMaxSound = m_TriggerSounds[i].strSoundNames.size() - 1;
					_randInt RandomSound(0, iMaxSound);
					m_TriggerSounds[i].iChannel = m_pGameInstance->Play_Sound(m_TriggerSounds[i].strSoundNames[RandomSound(m_RandomNumber)], m_TriggerSounds[i].fInitVolume, false, m_TriggerSounds[i].fStartPosRatio);
					if (m_TriggerSounds[i].IsFadeinSound)
					{
						m_pGameInstance->FadeinSound(m_TriggerSounds[i].iChannel, fTimeDelta, m_TriggerSounds[i].fFadeinSecond);
					}
					m_TriggerSounds[i].HasPlayed = true;
				}
			}
			else
			{
				_int iMaxSound = m_TriggerSounds[i].strSoundNames.size() - 1;
				_randInt RandomSound(0, iMaxSound);
				m_TriggerSounds[i].iChannel = m_pGameInstance->Play_Sound(m_TriggerSounds[i].strSoundNames[RandomSound(m_RandomNumber)], m_TriggerSounds[i].fInitVolume, false, m_TriggerSounds[i].fStartPosRatio);
				if (m_TriggerSounds[i].IsFadeinSound)
				{
					m_pGameInstance->FadeinSound(m_TriggerSounds[i].iChannel, fTimeDelta, m_TriggerSounds[i].fFadeinSecond);
				}
				m_TriggerSounds[i].HasPlayed = true;
			}
		}
		//채널 갱신
		if (m_TriggerSounds[i].iChannel != -1)
		{
			if (not m_pGameInstance->Get_IsPlayingSound(m_TriggerSounds[i].iChannel))
			{
				m_TriggerSounds[i].iChannel = -1;
				m_TriggerSounds[i].IsEnding = false;
			}
		}
		//사운드 제거
		if (m_TriggerSounds[i].iChannel != -1)
		{
			for (size_t j = 0; j < m_TriggerSounds[i].iEndAnimIndices.size(); j++)
			{
				if (m_AnimDesc.iAnimIndex == m_TriggerSounds[i].iEndAnimIndices[j] &&
					m_Animations[m_AnimDesc.iAnimIndex]->Get_CurrentAnimPos() >= m_TriggerSounds[i].fEndAnimPoses[j])
				{
					m_TriggerSounds[i].IsEnding = true;
					break;
				}
			}

			if (m_TriggerSounds[i].IsEnding)
			{
				m_pGameInstance->FadeoutSound(m_TriggerSounds[i].iChannel, fTimeDelta, m_TriggerSounds[i].fFadeoutSecond, false);
			}
		}
	}
#pragma endregion

	if (m_IsResetTriggers)
	{
		m_IsResetTriggers = false;

		for (size_t i = 0; i < m_TriggerEffects.size(); i++)
		{
			m_TriggerEffects[i].HasCreated = false;
		}
		for (size_t i = 0; i < m_TriggerSounds.size(); i++)
		{
			m_TriggerSounds[i].HasPlayed = false;
		}
	}

	return S_OK;
}

void CRealtimeVTFModel::Set_Animation(ANIM_DESC Animation_Desc)
{
	if (m_AnimDesc.iAnimIndex != Animation_Desc.iAnimIndex or
		Animation_Desc.bRestartAnimation or
		(m_AnimDesc.bRewindAnimation != Animation_Desc.bRewindAnimation and
			m_Animations[m_AnimDesc.iAnimIndex]->IsFinished()))
	{
		m_isAnimChanged = true;

		m_Animations[m_AnimDesc.iAnimIndex]->ResetFinished();

		if (Animation_Desc.iAnimIndex >= m_iNumAnimations)
			Animation_Desc.iAnimIndex = m_iNumAnimations - 1;
	}

	while (Animation_Desc.fStartAnimPos >= m_Animations[Animation_Desc.iAnimIndex]->Get_Duration())
	{
		Animation_Desc.fStartAnimPos -= m_Animations[Animation_Desc.iAnimIndex]->Get_Duration();
	}

	m_AnimDesc = Animation_Desc;
}

const _bool& CRealtimeVTFModel::IsAnimationFinished(_uint iAnimIndex) const
{
	return m_Animations[iAnimIndex]->IsFinished();
}

const _uint& CRealtimeVTFModel::Get_CurrentAnimationIndex() const
{
	return m_AnimDesc.iAnimIndex;
}

const _float CRealtimeVTFModel::Get_CurrentAnimPos() const
{
	return m_Animations[m_AnimDesc.iAnimIndex]->Get_CurrentAnimPos();
}

const _mat* CRealtimeVTFModel::Get_BoneMatrix(const _char* pBoneName) const
{
	auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&pBoneName](CBone* pBone) {
		if (!strcmp(pBone->Get_BoneName(), pBoneName))
			return true;
		return false;
	});

	if (m_Bones.end() == iter) {
		MSG_BOX("Can't Find Bone");
		return nullptr;
	}

	return (*iter)->Get_CombinedMatrix();
}

const _bool CRealtimeVTFModel::Get_PartIsRender(_uint iPartType, _uint iPartID)
{
	auto iter = m_Parts.find(iPartType);

	if (m_Parts.end() == iter) {
		MSG_BOX("그런 파츠 타입 없어용");
		return false;
	}

	if (0 > iPartID or iter->second.size() <= iPartID) {
		MSG_BOX("해당 파츠는 없어용");
		return false;
	}

	return iter->second[iPartID]->Get_isRender();
}

const _uint CRealtimeVTFModel::Get_Num_PartMeshes(_uint iPartType, _uint iPartID) const
{
	auto iter = m_Parts.find(iPartType);

	if (m_Parts.end() == iter) {
		MSG_BOX("그 파츠 타입은 없구연");
		return 0;
	}

	return iter->second[iPartID]->Get_NumMeshes();
}

const _uint CRealtimeVTFModel::Get_NumPart(_uint iPartType) const
{
	auto iter = m_Parts.find(iPartType);

	if (m_Parts.end() == iter) {
		MSG_BOX("그 파츠 타입은 없구연");
		return 0;
	}

	return iter->second.size();
}

HRESULT CRealtimeVTFModel::Seting_Render(_uint iPartType, _uint iPartID)
{
	auto iter = m_Parts.find(iPartType);

	if (m_Parts.end() == iter) {
		MSG_BOX("그 파츠 타입은 없구연");
		return E_FAIL;
	}

	if (0 > iPartID or iter->second.size() <= iPartID) {
		MSG_BOX("그 파츠는 없구연");
		return E_FAIL;
	}

	for (auto& pPart : iter->second) {
		pPart->Set_isRender(false);
	}

	iter->second[iPartID]->Set_isRender(true);

	return S_OK;
}

HRESULT CRealtimeVTFModel::Bind_Material(CShader* pShader, const _char* pVariableName, _uint iMeshIndex, TextureType eTextureType)
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

HRESULT CRealtimeVTFModel::Bind_Bone(CShader* pShader)
{
	if (FAILED(pShader->Bind_ShaderResourceView("g_BoneTexture", m_pBoneSRV)))
		return E_FAIL;

	if (true == m_isUsingMotionBlur) {
		if (FAILED(pShader->Bind_ShaderResourceView("g_OldBoneTexture", m_pOldBoneSRV)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CRealtimeVTFModel::Render(_uint iMeshIndex)
{
	if (iMeshIndex >= m_iNumMeshes)
		return E_FAIL;

	if (FAILED(m_Meshes[iMeshIndex]->Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRealtimeVTFModel::Bind_Part_Material(CShader* pShader, const _char* pVariableName, TextureType eTextureType, _uint iPartType, _uint iPartID, _uint iPartMeshIndex)
{
	auto iter = m_Parts.find(iPartType);

	if (m_Parts.end() == iter) {
		MSG_BOX("그 파츠 타입은 없구연");
		return E_FAIL;
	}

	return iter->second[iPartID]->Bind_Material(pShader, pVariableName, iPartMeshIndex, eTextureType);
}

HRESULT CRealtimeVTFModel::Render_Part(_uint iPartType, _uint iPartID, _uint iPartMeshIndex)
{
	auto iter = m_Parts.find(iPartType);

	if (m_Parts.end() == iter) {
		MSG_BOX("그 파츠 타입은 없구연");
		return E_FAIL;
	}

	return iter->second[iPartID]->Render(iPartMeshIndex);
}

HRESULT CRealtimeVTFModel::Seting_Parts(const string& strFilePath)
{
	CPart_Model* pPart = CPart_Model::Create(m_pDevice, m_pContext, strFilePath);

	if (nullptr == pPart)
		return E_FAIL;

	if (FAILED(pPart->Get_Bone_Offset(m_Bones)))
		return E_FAIL;

	_char szFileName[MAX_PATH]{};

	_splitpath_s(strFilePath.c_str(), nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0);

	m_PrototypeParts.emplace(szFileName, pPart);

	return S_OK;
}

HRESULT CRealtimeVTFModel::Read_Bones(ifstream& File)
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

HRESULT CRealtimeVTFModel::Read_Meshes(ifstream& File, const ModelType& eType, _fmatrix PivotMatrix)
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

HRESULT CRealtimeVTFModel::Read_Animations(ifstream& File)
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

HRESULT CRealtimeVTFModel::Read_Materials(ifstream& File, const string& strFilePath)
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

HRESULT CRealtimeVTFModel::CreateVTF()
{
	D3D11_TEXTURE2D_DESC Desc = {};
	Desc.Width = static_cast<_uint>(m_Bones.size() * 4);
	Desc.Height = 1;
	Desc.MipLevels = 1;
	Desc.ArraySize = 1;
	Desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	Desc.Usage = D3D11_USAGE_DYNAMIC;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	Desc.SampleDesc.Count = 1;

	if (FAILED(m_pDevice->CreateTexture2D(&Desc, nullptr, &m_pBoneTexture)))
		return E_FAIL;

	Desc.Usage = D3D11_USAGE_DEFAULT;
	Desc.CPUAccessFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&Desc, nullptr, &m_pOldBoneTexture)))
		return E_FAIL;
	
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.Format = Desc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	SRVDesc.Texture2DArray.MipLevels = 1;
	SRVDesc.Texture2DArray.ArraySize = 1;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pBoneTexture, &SRVDesc, &m_pBoneSRV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pOldBoneTexture, &SRVDesc, &m_pOldBoneSRV)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRealtimeVTFModel::UpdateBoneTexture(vector<_mat>& CombinedBones)
{
	D3D11_MAPPED_SUBRESOURCE TexData = {};
	if (FAILED(m_pContext->Map(m_pBoneTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &TexData)))
		return E_FAIL;

	memcpy(TexData.pData, CombinedBones.data(), m_Bones.size() * sizeof(_mat));

	m_pContext->Unmap(m_pBoneTexture, 0);

	return S_OK;
}

HRESULT CRealtimeVTFModel::Read_TriggerEffects(const string& strFilePath)
{
	_char szTriggerFilePath[MAX_PATH]{};
	_char szDirectory[MAX_PATH]{};
	_char szFileName[MAX_PATH]{};
	_char szExt[MAX_PATH] = ".effecttrigger";
	_splitpath_s(strFilePath.c_str(), nullptr, 0, szDirectory, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);
	strcpy_s(szTriggerFilePath, MAX_PATH, szDirectory);
	strcat_s(szTriggerFilePath, MAX_PATH, szFileName);
	strcat_s(szTriggerFilePath, MAX_PATH, szExt);

	ifstream TriggerFile(szTriggerFilePath, ios::binary);
	if (TriggerFile.is_open())
	{
		_uint iNumTriggerEffect = { 0 };
		TriggerFile.read(reinterpret_cast<char*>(&iNumTriggerEffect), sizeof _uint);
		for (_uint i = 0; i < iNumTriggerEffect; i++)
		{
			TRIGGEREFFECT_DESC EffectDesc{};
			TriggerFile.read(reinterpret_cast<_char*>(&EffectDesc.iStartAnimIndex), sizeof(_int));
			TriggerFile.read(reinterpret_cast<_char*>(&EffectDesc.fStartAnimPos), sizeof(_float));
			_uint iNumEnd{};
			TriggerFile.read(reinterpret_cast<_char*>(&iNumEnd), sizeof(_uint));
			for (_uint i = 0; i < iNumEnd; i++)
			{
				_int iEndAnimIndex{};
				TriggerFile.read(reinterpret_cast<_char*>(&iEndAnimIndex), sizeof(_int));
				EffectDesc.iEndAnimIndices.push_back(iEndAnimIndex);
				_float fEndAnimPos{};
				TriggerFile.read(reinterpret_cast<_char*>(&fEndAnimPos), sizeof(_float));
				EffectDesc.fEndAnimPoses.push_back(fEndAnimPos);
			}
			TriggerFile.read(reinterpret_cast<_char*>(&EffectDesc.IsFollow), sizeof(_bool));

			size_t iNameSize{};
			_tchar* pBuffer{};
			TriggerFile.read(reinterpret_cast<_char*>(&iNameSize), sizeof size_t);
			pBuffer = new _tchar[iNameSize / sizeof(_tchar)];
			TriggerFile.read(reinterpret_cast<_char*>(pBuffer), iNameSize);
			EffectDesc.strEffectName = pBuffer;
			Safe_Delete_Array(pBuffer);

			TriggerFile.read(reinterpret_cast<_char*>(&EffectDesc.iBoneIndex), sizeof(_uint));
			TriggerFile.read(reinterpret_cast<_char*>(&EffectDesc.OffsetMatrix), sizeof(_mat));
			TriggerFile.read(reinterpret_cast<_char*>(&EffectDesc.IsDeleteRotateToBone), sizeof(_bool));
			TriggerFile.read(reinterpret_cast<_char*>(&EffectDesc.IsClientTrigger), sizeof(_bool));

			m_TriggerEffects.push_back(EffectDesc);
			m_iNumTriggersEffect++;
		}
		TriggerFile.close();
	}

	return S_OK;
}

HRESULT CRealtimeVTFModel::Read_TriggerSounds(const string& strFilePath)
{
	_char szTriggerFilePath[MAX_PATH]{};
	_char szDirectory[MAX_PATH]{};
	_char szFileName[MAX_PATH]{};
	_char szExt[MAX_PATH] = ".soundtrigger";
	_splitpath_s(strFilePath.c_str(), nullptr, 0, szDirectory, MAX_PATH, szFileName, MAX_PATH, nullptr, 0);
	strcpy_s(szTriggerFilePath, MAX_PATH, szDirectory);
	strcat_s(szTriggerFilePath, MAX_PATH, szFileName);
	strcat_s(szTriggerFilePath, MAX_PATH, szExt);

	ifstream TriggerFile(szTriggerFilePath, ios::binary);
	if (TriggerFile.is_open())
	{
		_uint iNumTriggerSound = { 0 };
		TriggerFile.read(reinterpret_cast<char*>(&iNumTriggerSound), sizeof _uint);
		for (_uint i = 0; i < iNumTriggerSound; i++)
		{
			TRIGGERSOUND_DESC SoundDesc{};

			TriggerFile.read(reinterpret_cast<_char*>(&SoundDesc.iStartAnimIndex), sizeof(_int));
			TriggerFile.read(reinterpret_cast<_char*>(&SoundDesc.fStartAnimPos), sizeof(_float));

			_uint iNumEnd{};
			TriggerFile.read(reinterpret_cast<_char*>(&iNumEnd), sizeof(_uint));
			for (_uint i = 0; i < iNumEnd; i++)
			{
				_int iEndAnimIndex{};
				TriggerFile.read(reinterpret_cast<_char*>(&iEndAnimIndex), sizeof(_int));
				SoundDesc.iEndAnimIndices.push_back(iEndAnimIndex);
				_float fEndAnimPos{};
				TriggerFile.read(reinterpret_cast<_char*>(&fEndAnimPos), sizeof(_float));
				SoundDesc.fEndAnimPoses.push_back(fEndAnimPos);
			}

			_uint iNumName{};
			TriggerFile.read(reinterpret_cast<_char*>(&iNumName), sizeof(_uint));
			for (size_t i = 0; i < iNumName; i++)
			{
				size_t iNameSize{};
				_tchar* pBuffer{};
				TriggerFile.read(reinterpret_cast<_char*>(&iNameSize), sizeof size_t);
				pBuffer = new _tchar[iNameSize / sizeof(_tchar)];
				TriggerFile.read(reinterpret_cast<_char*>(pBuffer), iNameSize);
				SoundDesc.strSoundNames.push_back(pBuffer);
				Safe_Delete_Array(pBuffer);
			}

			TriggerFile.read(reinterpret_cast<_char*>(&SoundDesc.fInitVolume), sizeof(_float));
			TriggerFile.read(reinterpret_cast<_char*>(&SoundDesc.fFadeoutSecond), sizeof(_float));
			TriggerFile.read(reinterpret_cast<_char*>(&SoundDesc.IsClientTrigger), sizeof(_bool));
			TriggerFile.read(reinterpret_cast<_char*>(&SoundDesc.fStartPosRatio), sizeof(_float));
			TriggerFile.read(reinterpret_cast<_char*>(&SoundDesc.IsFadeinSound), sizeof(_bool));
			TriggerFile.read(reinterpret_cast<_char*>(&SoundDesc.fFadeinSecond), sizeof(_float));

			m_TriggerSounds.push_back(SoundDesc);
			m_iNumTriggersSound++;
		}

		TriggerFile.close();
	}

	return S_OK;
}

CRealtimeVTFModel* CRealtimeVTFModel::Create(_dev pDevice, _context pContext, const string& strFilePath, _fmatrix PivotMatrix)
{
	CRealtimeVTFModel* pInstance = new CRealtimeVTFModel(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(strFilePath, PivotMatrix)))
	{
		MSG_BOX("Failed to Create : CRealtimeVTFModel");
	}

	return pInstance;
}

CComponent* CRealtimeVTFModel::Clone(void* pArg)
{
	CRealtimeVTFModel* pInstance = new CRealtimeVTFModel(*this);

	if (FAILED(pInstance->Init(pArg, *this)))
	{
		MSG_BOX("Failed to Clone : CRealtimeVTFModel");
	}

	return pInstance;
}

void CRealtimeVTFModel::Free()
{
	if (m_hasCloned)
	{
		for (size_t i = 0; i < m_TriggerEffects.size(); i++)
		{
			if (m_TriggerEffects[i].IsFollow)
			{
				m_pGameInstance->Delete_Effect(m_EffectMatrices[i]);
			}
		}
	}

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

	for (auto& pPart : m_PrototypeParts)
		Safe_Release(pPart.second);

	m_PrototypeParts.clear();

	m_Parts.clear();

	Safe_Release(m_pOldBoneTexture);
	Safe_Release(m_pOldBoneSRV);

	Safe_Release(m_pBoneTexture);
	Safe_Release(m_pBoneSRV);

	for (auto& pEffectMatrix : m_EffectMatrices)
	{
		Safe_Delete(pEffectMatrix);
	}

	Safe_Release(m_pOwnerTransform);
}
