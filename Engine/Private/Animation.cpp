#include "Animation.h"
#include "Channel.h"
#include "Bone.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_fDuration(rhs.m_fDuration)
	, m_fTickPerSec(rhs.m_fTickPerSec)
	, m_iNumChannels(rhs.m_iNumChannels)
	, m_iNumTriggers(rhs.m_iNumTriggers)
	, m_Triggers(rhs.m_Triggers)
	, m_iMaxFrame(rhs.m_iMaxFrame)
	//, m_Channels(rhs.m_Channels)
	//, m_CurrentKeyFrames(rhs.m_CurrentKeyFrames)
	//, m_PrevTransformations(rhs.m_PrevTransformations)
	, m_pPrevTransformation(rhs.m_pPrevTransformation)
	, m_hasCloned(true)
{
	strcpy_s(m_szName, rhs.m_szName);
	//for (auto& pChannel : m_Channels)
	//{
	//	Safe_AddRef(pChannel);
	//}
	for (auto& pPrototypeChannel : rhs.m_Channels)
	{
		CChannel* pChannel = pPrototypeChannel->Clone();

		m_Channels.push_back(pChannel);
	}
}

const _char* CAnimation::Get_Name() const
{
	return m_szName;
}

const _bool& CAnimation::IsFinished() const
{
	return m_isFinished;
}

const _float CAnimation::Get_CurrentAnimPos() const
{
	return m_fCurrentAnimPos;
}

const _float CAnimation::Get_Duration() const
{
	return m_fDuration;
}

void CAnimation::ResetFinished()
{
	m_isFinished = false;
	//선형보간 중에 애니메이션 바뀌면 선형보간을 처음부터 하기 위해
	m_isInterpolating = false;
}

void CAnimation::Set_CurrentAnimPos(_float fCurrentAnimPos)
{
	m_fCurrentAnimPos = fCurrentAnimPos;
}

HRESULT CAnimation::Init(ifstream& ModelFile, const vector<class CBone*>& Bones)
{
	_uint iNameSize{};
	ModelFile.read(reinterpret_cast<_char*>(&iNameSize), sizeof _uint);
	if (iNameSize >= sizeof(m_szName))
	{
		MSG_BOX("Name Is Too Long!");
	}
	ModelFile.read(m_szName, iNameSize);

	ModelFile.read(reinterpret_cast<_char*>(&m_fDuration), sizeof _float);
	ModelFile.read(reinterpret_cast<_char*>(&m_fTickPerSec), sizeof _float);

	ModelFile.read(reinterpret_cast<_char*>(&m_iNumChannels), sizeof _uint);

	//m_CurrentKeyFrames.resize(m_iNumChannels);
	//m_PrevTransformations.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(ModelFile);
		if (!pChannel)
		{
			return E_FAIL;
		}
		m_Channels.push_back(pChannel);

		m_iMaxFrame = max(m_iMaxFrame, pChannel->Get_KeyFrames().size());
	}

	m_pPrevTransformation = new _mat[Bones.size()];

	return S_OK;
}

void CAnimation::Update_TransformationMatrix(const vector<class CBone*>& Bones, _float fTimeDelta, _bool& isAnimChanged, const _bool& isLoop, const _bool& bSkipInterpolation, _float fInterpolationTime, _float fDurationRatio, _float fStartAnimPos, _bool bRewindAnimation)
{
	if (bRewindAnimation)
	{
		if (fStartAnimPos == 0.f && not m_isFinished)
		{
			fStartAnimPos = m_fDuration;
		}

		fDurationRatio = 1.f - fDurationRatio;
	}

	if (isAnimChanged)
	{
		if (bSkipInterpolation)
		{
			m_fCurrentAnimPos = fStartAnimPos;
			isAnimChanged = false;
		}
		else if (!m_isInterpolating)
		{
			m_fCurrentAnimPos = fStartAnimPos;
			m_fLerpAnimPos = 0.f;
			m_isInterpolating = true;
		}
		else
		{
			m_fLerpAnimPos += fTimeDelta;
		}
	}
	else
	{
		if (not bRewindAnimation)
		{
			if (m_fCurrentAnimPos == 0.f)
			{
				m_fCurrentAnimPos = fStartAnimPos;
			}
		}
		
		if (not m_isFinished)
		{
			if (bRewindAnimation)
			{
				m_fCurrentAnimPos -= m_fTickPerSec * fTimeDelta;
			}
			else
			{
				m_fCurrentAnimPos += m_fTickPerSec * fTimeDelta;
			}
		}

		if (bRewindAnimation)
		{
			if (m_fCurrentAnimPos <= m_fDuration * fDurationRatio)
			{
				if (isLoop)
				{
					m_fCurrentAnimPos = fStartAnimPos;
				}
				else
				{
					m_isFinished = true;
				}
			}
			else
			{
				m_isFinished = false;
			}
		}
		else
		{
			if (m_fCurrentAnimPos >= m_fDuration * fDurationRatio)
			{
				if (isLoop)
				{
					m_fCurrentAnimPos = fStartAnimPos;
	
				}
				else
				{
					m_isFinished = true;
				}
			}
			else
			{
				m_isFinished = false;
			}
		}

	}

	if (isAnimChanged)
	{
		Update_Lerp_TransformationMatrix(Bones, isAnimChanged, fInterpolationTime);
	}
	else// if (not m_isFinished)
	{
		for (size_t i = 0; i < m_iNumChannels; i++)
		{
			m_Channels[i]->Update_TransformationMatrix(Bones, m_fCurrentAnimPos, isAnimChanged, fInterpolationTime);
		}
	}
}

void CAnimation::Update_Lerp_TransformationMatrix(const vector<class CBone*>& Bones, _bool& isAnimChanged, _float fInterpolationTime)
{
	_vec4 vScaling{};
	_vec4 vRotation{};
	_vec4 vPosition{};

	_uint iNumPrevTransformation{};

	if (m_fLerpAnimPos == 0.f)
	{
		for (auto& pBone : Bones)
		{
			m_pPrevTransformation[iNumPrevTransformation] = pBone->Get_Transformation();
			++iNumPrevTransformation;/*
			if (m_pPrevTransformation[iNumPrevTransformation].m[3][3] == 0.f)
			{
			}*/
		}

		iNumPrevTransformation = 0;
	}

	for (auto& pBone : Bones)
	{
		_mat PrevTransformation;
		PrevTransformation = m_pPrevTransformation[iNumPrevTransformation];
		const _char* szBoneName = pBone->Get_BoneName();

		_vector vTmpScaling{}, vSrcRotation{}, vTmpPosition{};
		_vec4 vSrcScaling{}, vDstScaling{};
		_vec4 vDstRotation{};
		_vec4 vSrcPotition{}, vDstPosition{};
		_float fRatio = m_fLerpAnimPos / fInterpolationTime;

		XMMatrixDecompose(&vTmpScaling, &vSrcRotation, &vTmpPosition, PrevTransformation);
		vSrcScaling.x = PrevTransformation.Right().Length();
		vSrcScaling.y = PrevTransformation.Up().Length();
		vSrcScaling.z = PrevTransformation.Look().Length();
		vSrcScaling.w = 0.f;
		//vSrcRotation = XMQuaternionRotationMatrix(PrevTransformation);
		//vSrcPotition = _vec4(&PrevTransformation._41);
		vSrcPotition = PrevTransformation.Position();

		KEYFRAME DestKeyFrame = {};

		auto iter = find_if(m_Channels.begin(), m_Channels.end(), [&](CChannel* pChannel)->_bool {
			if (0 == strcmp(pChannel->Get_ChannelName(), szBoneName))
			{
				DestKeyFrame = pChannel->Get_CurrentKeyFrame(m_fCurrentAnimPos);
				return true;
			}
			DestKeyFrame.vScaling = vSrcScaling;
			DestKeyFrame.vRotation = vSrcRotation;
			DestKeyFrame.vPosition = vSrcPotition;
			return false;
			});

		vDstScaling = XMLoadFloat4(&DestKeyFrame.vScaling);
		vDstRotation = XMLoadFloat4(&DestKeyFrame.vRotation);
		vDstPosition = XMLoadFloat4(&DestKeyFrame.vPosition);

		vScaling = XMVectorLerp(vSrcScaling, vDstScaling, fRatio);
		vRotation = XMQuaternionSlerp(vSrcRotation, vDstRotation, fRatio);
		vPosition = XMVectorLerp(vSrcPotition, vDstPosition, fRatio);

		_matrix TransformationMatrix = XMMatrixAffineTransformation(vScaling, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

		pBone->Set_Transformation(TransformationMatrix);

		++iNumPrevTransformation;
	}

	if (m_fLerpAnimPos >= fInterpolationTime)
	{
		isAnimChanged = false;
		m_fLerpAnimPos = 0.f;
		m_isInterpolating = false;
	}
}

HRESULT CAnimation::Prepare_Animation(const vector<class CBone*>& Bones, _uint iFrame)
{
	for (auto& pChannel : m_Channels)
		pChannel->Prepare_Transformation(Bones, iFrame);
	return S_OK;
}

CAnimation* CAnimation::Create(ifstream& ModelFile, const vector<class CBone*>& Bones, const _mat& PivotMatrix)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Init(ModelFile, Bones)))
	{
		MSG_BOX("Failed to Create : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
	{
		Safe_Release(pChannel);
	}
	m_Channels.clear();

	m_Triggers.clear();

	if (not m_hasCloned)
	{
		Safe_Delete_Array(m_pPrevTransformation);
	}
}
