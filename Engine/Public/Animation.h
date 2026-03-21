#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;
	
public:
	const _char* Get_Name() const;
	const _bool& IsFinished() const;
	const _float Get_CurrentAnimPos() const;
	const _float Get_Duration() const;

	void ResetFinished();
	void Set_CurrentAnimPos(_float fCurrentAnimPos);

	// 원명
	const _uint Get_MaxFrame() const {
		return m_iMaxFrame;
	}

public:
	HRESULT Init(ifstream& ModelFile, const vector<class CBone*>& Bones);
	void Update_TransformationMatrix(const vector<class CBone*>& Bones, _float fTimeDelta, _bool& isAnimChanged, const _bool& isLoop, const _bool& bSkipInterpolation, _float fInterpolationTime, _float fDurationRatio, _float fStartAnimPos = 0.f, _bool bRewindAnimation = false);

	HRESULT Prepare_Animation(const vector<class CBone*>& Bones, _uint iFrame);

	const _float Get_TickPerSec() const {
		return m_fTickPerSec;
	}

private:
	_char m_szName[MAX_PATH]{};
	
	_float m_fDuration{};
	_float m_fTickPerSec{};
	_float m_fCurrentAnimPos{};
	_float m_fLerpAnimPos{};
	_mat* m_pPrevTransformation{};
	//vector<_uint> m_CurrentKeyFrames{};
	//vector<_float4x4> m_PrevTransformations{};

	_uint m_iNumChannels{};
	vector<class CChannel*> m_Channels{};

	_bool m_isFinished{};
	_bool m_isInterpolating{};

	_uint m_iNumTriggers{};
	vector<_float> m_Triggers; // 트리거

	_uint m_iMaxFrame = 0;

	_bool m_hasCloned{};

private:
	static _uint m_iEffectID;

private:
	void Update_Lerp_TransformationMatrix(const vector<class CBone*>& Bones, _bool& isAnimChanged, _float fInterpolationTime);

public:
	static CAnimation* Create(ifstream& ModelFile, const vector<class CBone*>& Bones, const _mat& PivotMatrix);
	CAnimation* Clone();
	virtual void Free() override;
};

END