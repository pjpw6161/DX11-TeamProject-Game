#include "Channel.h"
#include "Bone.h"

CChannel::CChannel()
{
}

KEYFRAME CChannel::Get_CurrentKeyFrame(_float fCurrentAnimPos) const
{
	if (fCurrentAnimPos == 0.f)
	{
		return m_KeyFrames.front();
	}
	else if (fCurrentAnimPos >= m_KeyFrames.back().fTime)
	{
		return m_KeyFrames.back();
	}
	else
	{
		for (size_t i = 0; i < m_KeyFrames.size(); i++)
		{
			if (fCurrentAnimPos < m_KeyFrames[i].fTime)
			{
				continue;
			}

			_vec4 vSrcScaling{}, vDstScaling{};
			_vec4 vSrcRotation{}, vDstRotation{};
			_vec4 vSrcPotition{}, vDstPosition{};

			_float fRatio = (fCurrentAnimPos - m_KeyFrames[i].fTime) / (m_KeyFrames[i + 1].fTime - m_KeyFrames[i].fTime);

			vSrcScaling = m_KeyFrames[i].vScaling;
			vDstScaling = m_KeyFrames[i + 1].vScaling;

			vSrcRotation = m_KeyFrames[i].vRotation;
			vDstRotation = m_KeyFrames[i + 1].vRotation;

			vSrcPotition = m_KeyFrames[i].vPosition;
			vDstPosition = m_KeyFrames[i + 1].vPosition;

			_vec4 vScaling{}, vRotation{}, vPosition{};

			vScaling = XMVectorLerp(vSrcScaling, vDstScaling, fRatio);
			vRotation = XMQuaternionSlerp(vSrcRotation, vDstRotation, fRatio);
			vPosition = XMVectorLerp(vSrcPotition, vDstPosition, fRatio);

			KEYFRAME KeyFrame{};

			KeyFrame.vScaling = vScaling;
			KeyFrame.vRotation = vRotation;
			KeyFrame.vPosition = vPosition;
	
			return KeyFrame;
		}
	}

	MSG_BOX("Failed to Find KeyFrame");
	return KEYFRAME{};
}

HRESULT CChannel::Init(ifstream& ModelFile)
{
	_uint iNameSize{};
	ModelFile.read(reinterpret_cast<_char*>(&iNameSize), sizeof _uint);
	if (iNameSize >= sizeof(m_szName))
	{
		MSG_BOX("Name Is Too Long!");
	}
	ModelFile.read(m_szName, iNameSize);

	ModelFile.read(reinterpret_cast<_char*>(&m_iBoneIndex), sizeof _uint);

	ModelFile.read(reinterpret_cast<_char*>(&m_iNumkeyFrames), sizeof _uint);

	KEYFRAME KeyFrame{};

	for (size_t i = 0; i < m_iNumkeyFrames; i++)
	{
		ModelFile.read(reinterpret_cast<_char*>(&KeyFrame.vScaling), sizeof _float4);
		ModelFile.read(reinterpret_cast<_char*>(&KeyFrame.vRotation), sizeof _float4);
		ModelFile.read(reinterpret_cast<_char*>(&KeyFrame.vPosition), sizeof _float4);
		ModelFile.read(reinterpret_cast<_char*>(&KeyFrame.fTime), sizeof _float);

		m_KeyFrames.push_back(KeyFrame);
	}

	return S_OK;
}

void CChannel::Update_TransformationMatrix(const vector<class CBone*>& Bones, _float& fCurrentAnimPos, _bool& isAnimChanged, _float fInterpolationTime)
{
	if (fCurrentAnimPos == 0.f && !isAnimChanged)
	{
		m_iCurrentKeyFrame = 0;
		m_fPreAnimPos = 0.f;
	}
	else if (fCurrentAnimPos == m_KeyFrames.back().fTime && !isAnimChanged)
	{
		m_iCurrentKeyFrame = m_KeyFrames.size() - 1;
		m_fPreAnimPos = fCurrentAnimPos;
	}

	KEYFRAME LastKeyFrame = m_KeyFrames.back();
	KEYFRAME FirstKeyFrame = m_KeyFrames.front();

	_vec4 vScaling{};
	_vec4 vRotation{};
	_vec4 vPosition{};

	/*if (isAnimChanged)
	{
		m_iCurrentKeyFrame = 0;
		if (m_PrevTransformation.m[3][3] == 0.f)
		{
			m_PrevTransformation = Bones[m_iBoneIndex]->Get_Transformation();
		}
		_mat PrevTransformation = XMLoadFloat4x4(&m_PrevTransformation);
		_vec4 vSrcScaling{}, vDstScaling{};
		_vec4 vSrcRotation{}, vDstRotation{};
		_vec4 vSrcPotition{}, vDstPosition{};
		_float fRatio = fCurrentAnimPos / fInterpolationTime;

		vSrcScaling.x = PrevTransformation.Right().Length();
		vSrcScaling.y = PrevTransformation.Up().Length();
		vSrcScaling.z = PrevTransformation.Look().Length();
		vSrcScaling.w = 0.f;
		vDstScaling = XMLoadFloat4(&m_KeyFrames[0].vScaling);
		vScaling = XMVectorLerp(vSrcScaling, vDstScaling, fRatio);

		vSrcRotation = XMQuaternionRotationMatrix(PrevTransformation);
		vDstRotation = XMLoadFloat4(&m_KeyFrames[0].vRotation);
		vRotation = XMQuaternionSlerp(vSrcRotation, vDstRotation, fRatio);

		vSrcPotition = _vec4(&PrevTransformation._41);
		vDstPosition = m_KeyFrames[0].vPosition;
		vPosition = XMVectorLerp(vSrcPotition, vDstPosition, fRatio);
		
		if (fCurrentAnimPos >= fInterpolationTime)
		{
			isAnimChanged = false;
			fCurrentAnimPos = 0.f;
		}
	}
	else*/ if (fCurrentAnimPos >= LastKeyFrame.fTime)
	{
		vScaling = XMLoadFloat4(&LastKeyFrame.vScaling);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vPosition = XMLoadFloat4(&LastKeyFrame.vPosition);
	}
	else if (fCurrentAnimPos <= FirstKeyFrame.fTime)
	{
		vScaling = XMLoadFloat4(&FirstKeyFrame.vScaling);
		vRotation = XMLoadFloat4(&FirstKeyFrame.vRotation);
		vPosition = XMLoadFloat4(&FirstKeyFrame.vPosition);
	}
	else
	{
		if (fCurrentAnimPos >= m_fPreAnimPos)
		{
			while (fCurrentAnimPos > m_KeyFrames[m_iCurrentKeyFrame + 1].fTime)
			{
				m_iCurrentKeyFrame++;
			}
		}
		else if (fCurrentAnimPos < m_fPreAnimPos)
		{
			while (fCurrentAnimPos < m_KeyFrames[m_iCurrentKeyFrame].fTime)
			{
				m_iCurrentKeyFrame--;
			}
		}

		_float fRatio = 0.f;

		fRatio = (fCurrentAnimPos - m_KeyFrames[m_iCurrentKeyFrame].fTime) / (m_KeyFrames[m_iCurrentKeyFrame + 1].fTime - m_KeyFrames[m_iCurrentKeyFrame].fTime);

		_vec4 vSrcScaling{}, vDstScaling{};
		_vec4 vSrcRotation{}, vDstRotation{};
		_vec4 vSrcPotition{}, vDstPosition{};

		vSrcScaling = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame].vScaling);
		vDstScaling = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame + 1].vScaling);

		vSrcRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame].vRotation);
		vDstRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame + 1].vRotation);

		vSrcPotition = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame].vPosition);
		vDstPosition = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame + 1].vPosition);

		vScaling = XMVectorLerp(vSrcScaling, vDstScaling, fRatio);
		vRotation = XMQuaternionSlerp(vSrcRotation, vDstRotation, fRatio);
		vPosition = XMVectorLerp(vSrcPotition, vDstPosition, fRatio);

		m_fPreAnimPos = fCurrentAnimPos;

		/*m_PrevTransformation = {};
		while (fCurrentAnimPos >= m_KeyFrames[m_iCurrentKeyFrame + 1].fTime)
		{
			m_iCurrentKeyFrame++;
		}

		_float fRatio = (fCurrentAnimPos - m_KeyFrames[m_iCurrentKeyFrame].fTime) / (m_KeyFrames[m_iCurrentKeyFrame + 1].fTime - m_KeyFrames[m_iCurrentKeyFrame].fTime);

		_vec4 vSrcScaling{}, vDstScaling{};
		_vec4 vSrcRotation{}, vDstRotation{};
		_vec4 vSrcPotition{}, vDstPosition{};

		vSrcScaling = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame].vScaling);
		vDstScaling = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame + 1].vScaling);
		vScaling = XMVectorLerp(vSrcScaling, vDstScaling, fRatio);

		vSrcRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame].vRotation);
		vDstRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame + 1].vRotation);
		vRotation = XMQuaternionSlerp(vSrcRotation, vDstRotation, fRatio);

		vSrcPotition = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame].vPosition);
		vDstPosition = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame + 1].vPosition);
		vPosition = XMVectorLerp(vSrcPotition, vDstPosition, fRatio);*/
	}
	
	_mat Transformation = XMMatrixAffineTransformation(vScaling, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	Bones[m_iBoneIndex]->Set_Transformation(Transformation);
}

void CChannel::Prepare_Transformation(const vector<class CBone*>& Bones, _uint iKeyFrame)
{
	_vec4 vScale = _vec4(1.f, 1.f, 1.f, 0.f);
	_vec4 vRotation = _vec4(0.f, 0.f, 0.f, 0.f);
	_vec4 vPosition = _vec4(0.f, 0.f, 0.f, 1.f);

	if (iKeyFrame < m_KeyFrames.size()) {
		vScale = m_KeyFrames[iKeyFrame].vScaling;
		vRotation = m_KeyFrames[iKeyFrame].vRotation;
		vPosition = m_KeyFrames[iKeyFrame].vPosition;
	}
	else {
		vScale = m_KeyFrames.back().vScaling;
		vRotation = m_KeyFrames.back().vRotation;
		vPosition = m_KeyFrames.back().vPosition;
	}

	_mat TransformationMatrix = XMMatrixAffineTransformation(vScale, _vec4(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	Bones[m_iBoneIndex]->Set_Transformation(TransformationMatrix);
}

CChannel* CChannel::Create(ifstream& ModelFile)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Init(ModelFile)))
	{
		MSG_BOX("Failed to Create : CChannel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CChannel* CChannel::Clone()
{
	return new CChannel(*this);
}

void CChannel::Free()
{
}
