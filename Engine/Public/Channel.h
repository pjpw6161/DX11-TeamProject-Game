#pragma once
#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	const _char* Get_ChannelName() const {
		return m_szName;
	}

	KEYFRAME Get_CurrentKeyFrame(_float fCurrentAnimPos) const;

public:
	HRESULT Init(ifstream& ModelFile);
	void Update_TransformationMatrix(const vector<class CBone*>& Bones, _float& fCurrentAnimPos, _bool& isAnimChanged, _float fInterpolationTime);

	void Prepare_Transformation(const vector<class CBone*>& Bones, _uint iKeyFrame);

	const vector<KEYFRAME>& Get_KeyFrames() const {
		return m_KeyFrames;
	}

private:
	_char m_szName[MAX_PATH]{};

	_uint m_iNumkeyFrames{};
	_uint m_iCurrentKeyFrame{};
	vector<KEYFRAME> m_KeyFrames{};
	_uint m_iBoneIndex{};
	_float m_fPreAnimPos = { 0.f };

public:
	static CChannel* Create(ifstream& ModelFile);
	CChannel* Clone();
	virtual void Free() override;
};

END