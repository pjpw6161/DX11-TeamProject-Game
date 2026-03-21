#pragma once
#include "Base.h"

BEGIN(Engine)

class CSound_Manager final : public CBase
{
public:
	typedef struct tagSoundDesc {

		_bool IsPlayingSound{};
		_float fStartVolume = 0.5f;
		_bool IsFadingout{};
		_bool IsFadingin{};
		_float fFadeSecond = 1.f;
		_bool IsReusable{};
		_float fFadeSoundRatio{};
		_float fDurationRatio = 1.f;

	}SOUND_DESC;

private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	HRESULT Init();

public:
	_int Play_Sound(const wstring& strSoundTag, _float fVolume, _bool isLoop, _float fStartPosRatio);
	void PlayBGM(const wstring& strSoundTag, _float fVolume);
	void StopSound(_uint iChannel);
	void StopAll();
	void SetChannelVolume(_uint iChannel, _float fVolume);
	void SetChannelStartVolume(_uint iChannel);
	void Update();
	HRESULT FadeoutSound(_uint iChannel, _float fTimeDelta, _float fFadeoutSecond, _bool IsReusable, _float fFadeSoundRatio);
	HRESULT FadeinSound(_uint iChannel, _float fTimeDelta, _float fFadeinSecond, _float fFadeSoundRatio);
	_bool Get_IsPlayingSound(_uint iChannel);
	_float GetChannelVolume(_uint iChannel);
	_bool Get_IsLoopingSound(_uint iChannel);
	_float Get_CurPosRatio(_uint iChannel);

private:
	_float m_fFadeTimeDelta{};
	map<const wstring, FMOD::Sound*> m_Sound;
	FMOD::Channel** m_pChannelArr{};
	FMOD::System* m_pSystem{ nullptr };
	vector<SOUND_DESC> m_SoundDescs;
	
private:
	HRESULT LoadSoundFile();
	FMOD::Sound* Find_Sound(const wstring& strSoundTag);
	void FadingoutSound(_uint iChannel);
	void FadinginSound(_uint iChannel);

public:
	static CSound_Manager* Create();
	virtual void Free() override;
};

END