#include "Sound_Manager.h"

CSound_Manager::CSound_Manager()
{
}

HRESULT CSound_Manager::Init()
{
	FMOD::System_Create(&m_pSystem);

	m_pSystem->init(FMOD_MAX_CHANNEL_WIDTH, FMOD_INIT_NORMAL, NULL);

	if (FAILED(LoadSoundFile()))
	{
		return E_FAIL;
	}

	m_pChannelArr = new FMOD::Channel*[FMOD_MAX_CHANNEL_WIDTH]{};

	m_SoundDescs.reserve(FMOD_MAX_CHANNEL_WIDTH);
	for (_uint i = 0; i < FMOD_MAX_CHANNEL_WIDTH; i++)
	{
		m_SoundDescs.push_back(SOUND_DESC());
	}

	return S_OK;
}

_int CSound_Manager::Play_Sound(const wstring& strSoundTag, _float fVolume, _bool isLoop, _float fStartPosRatio)
{
	FMOD::Sound* pSound = Find_Sound(strSoundTag);

	if (!pSound)
	{
		return -1;
	}

	for (_int i = 1; i < FMOD_MAX_CHANNEL_WIDTH; i++)
	{
		_bool bPlay = false;
		m_pChannelArr[i]->isPlaying(&bPlay);
		if (not m_SoundDescs[i].IsPlayingSound && not bPlay)
		{
			m_pSystem->playSound(pSound, 0, false, &m_pChannelArr[i]);

			if (isLoop)
			{
				m_pChannelArr[i]->setMode(FMOD_LOOP_NORMAL);
			}
			else
			{
				m_pChannelArr[i]->setMode(FMOD_LOOP_OFF);
			}

			m_pChannelArr[i]->setVolume(fVolume);
			//이전 사운드 저장
			m_SoundDescs[i].fStartVolume = fVolume;
			
			_uint iSoundLength{};
			pSound->getLength(&iSoundLength, FMOD_TIMEUNIT_MS);
			fStartPosRatio *= static_cast<_float>(iSoundLength);
			_uint iStartPos = static_cast<_uint>(fStartPosRatio);
			m_pChannelArr[i]->setPosition(iStartPos, FMOD_TIMEUNIT_MS);

			m_pSystem->update();

			return i;
		}
	}

	return -1;
}

void CSound_Manager::PlayBGM(const wstring& strSoundTag, _float fVolume)
{
	FMOD::Sound* pSound = Find_Sound(strSoundTag);
	if (!pSound)
	{
		return;
	}
	
	m_pSystem->playSound(pSound, 0, false, &m_pChannelArr[0]);

	m_pChannelArr[0]->setMode(FMOD_LOOP_NORMAL);

	m_pChannelArr[0]->setVolume(fVolume);
	//이전 사운드 저장
	m_SoundDescs[0].fStartVolume = fVolume;

	m_pSystem->update();
}

void CSound_Manager::StopSound(_uint iChannel)
{
	m_pChannelArr[iChannel]->stop();
}

void CSound_Manager::StopAll()
{
	for (int i = 0; i < FMOD_MAX_CHANNEL_WIDTH; ++i)
	{
		m_pChannelArr[i]->stop();
	}
}

void CSound_Manager::SetChannelVolume(_uint iChannel, _float fVolume)
{
	m_pChannelArr[iChannel]->setVolume(fVolume);

	m_pSystem->update();
}

void CSound_Manager::SetChannelStartVolume(_uint iChannel)
{
	m_pChannelArr[iChannel]->setVolume(m_SoundDescs[iChannel].fStartVolume);

	m_pSystem->update();
}

void CSound_Manager::Update()
{
	for (int i = 0; i < FMOD_MAX_CHANNEL_WIDTH; ++i)
	{
		_bool bPlay = false;
		m_pChannelArr[i]->isPlaying(&bPlay);
		m_SoundDescs[i].IsPlayingSound = bPlay;

		if (m_SoundDescs[i].IsFadingout)
		{
			if (Get_IsPlayingSound(i))
			{
				FadingoutSound(i);
	
				if (GetChannelVolume(i) <= (m_SoundDescs[i].fStartVolume * m_SoundDescs[i].fFadeSoundRatio))
				{
					m_SoundDescs[i].IsFadingout = false;
					if (not m_SoundDescs[i].IsReusable)
					{
						m_SoundDescs[i].IsReusable = true;
						m_pChannelArr[i]->stop();
					}
					else
					{
						SetChannelVolume(i, (m_SoundDescs[i].fStartVolume * m_SoundDescs[i].fFadeSoundRatio));
					}
				}
			}
			else
			{
				m_SoundDescs[i].IsFadingout = false;
			}
		}
		else if (m_SoundDescs[i].IsFadingin)
		{
			if (Get_IsPlayingSound(i))
			{
				FadinginSound(i);

				if (GetChannelVolume(i) >= (m_SoundDescs[i].fStartVolume * m_SoundDescs[i].fFadeSoundRatio))
				{
					m_SoundDescs[i].IsFadingin = false;
					SetChannelVolume(i, (m_SoundDescs[i].fStartVolume * m_SoundDescs[i].fFadeSoundRatio));
				}
			}
			else
			{
				m_SoundDescs[i].IsFadingin = false;
			}
		}
	}
}

HRESULT CSound_Manager::FadeoutSound(_uint iChannel, _float fTimeDelta, _float fFadeoutSecond, _bool IsReusable, _float fFadeSoundRatio)
{
	if (iChannel < 0)
	{
		return E_FAIL;
	}

	if (not m_SoundDescs[iChannel].IsFadingout)
	{
	}

	m_SoundDescs[iChannel].IsFadingin = false;
	m_SoundDescs[iChannel].IsFadingout = true;
	m_fFadeTimeDelta = fTimeDelta;
	m_SoundDescs[iChannel].fFadeSecond = fFadeoutSecond;
	m_SoundDescs[iChannel].IsReusable = IsReusable;
	m_SoundDescs[iChannel].fFadeSoundRatio = fFadeSoundRatio;

	return S_OK;
}

HRESULT CSound_Manager::FadeinSound(_uint iChannel, _float fTimeDelta, _float fFadeinSecond, _float fFadeSoundRatio)
{
	if (iChannel < 0)
	{
		return E_FAIL;
	}

	if (not m_SoundDescs[iChannel].IsFadingin && GetChannelVolume(iChannel) == m_SoundDescs[iChannel].fStartVolume)
	{
		SetChannelVolume(iChannel, 0.f);
	}

	m_SoundDescs[iChannel].IsFadingout = false;
	m_SoundDescs[iChannel].IsFadingin = true;
	m_fFadeTimeDelta = fTimeDelta;
	m_SoundDescs[iChannel].fFadeSecond = fFadeinSecond;
	m_SoundDescs[iChannel].fFadeSoundRatio = fFadeSoundRatio;

	return S_OK;
}

_bool CSound_Manager::Get_IsPlayingSound(_uint eChannel)
{
	_bool bPlay = false;
	m_pChannelArr[eChannel]->isPlaying(&bPlay);

	return bPlay;
}

_float CSound_Manager::GetChannelVolume(_uint iChannel)
{
	_float fVolume{};
	m_pChannelArr[iChannel]->getVolume(&fVolume);

	return fVolume;
}

_bool CSound_Manager::Get_IsLoopingSound(_uint iChannel)
{
	FMOD_MODE Mode{};
	m_pChannelArr[iChannel]->getMode(&Mode);

	if (FMOD_LOOP_NORMAL & Mode)
	{
		return true;
	}
	else
	{
		return false;
	}
}

_float CSound_Manager::Get_CurPosRatio(_uint iChannel)
{
	FMOD::Sound* pSound{};
	m_pChannelArr[iChannel]->getCurrentSound(&pSound);
	_uint iSoundLength{};
	pSound->getLength(&iSoundLength, FMOD_TIMEUNIT_MS);
	_uint iCurPos{};
	m_pChannelArr[iChannel]->getPosition(&iCurPos, FMOD_TIMEUNIT_MS);

	_float fCurPosRatio = static_cast<_float>(iCurPos) / static_cast<_float>(iSoundLength);

	return fCurPosRatio;
}

HRESULT CSound_Manager::LoadSoundFile()
{
	string FilePath = "../../Client/Bin/Resources/Sound/";

	for (const auto& entry : filesystem::recursive_directory_iterator(FilePath))
	{
		if (entry.is_regular_file())
		{
			FMOD::Sound* pSound = nullptr;

			FMOD_RESULT eRes = m_pSystem->createSound(entry.path().string().c_str(), FMOD_DEFAULT, 0, &pSound);

			if (eRes == FMOD_OK)
			{
				m_Sound.emplace(entry.path().stem().wstring(), pSound);
			}
		}
	}

	return S_OK;
}

FMOD::Sound* CSound_Manager::Find_Sound(const wstring& strSoundTag)
{
	auto& it = m_Sound.find(strSoundTag);

	if (it == m_Sound.end())
	{
		return nullptr;
	}

	return it->second;
}

void CSound_Manager::FadingoutSound(_uint iChannel)
{
	_float fVolume = GetChannelVolume(iChannel);
	fVolume -= (m_fFadeTimeDelta / (m_SoundDescs[iChannel].fFadeSecond / m_SoundDescs[iChannel].fStartVolume));
		//(m_SoundDescs[iChannel].fStartVolume * (1.f - m_SoundDescs[iChannel].fFadeSoundRatio))));
	
	SetChannelVolume(iChannel, fVolume);
}

void CSound_Manager::FadinginSound(_uint iChannel)
{
	_float fVolume = GetChannelVolume(iChannel);
	fVolume += (m_fFadeTimeDelta / (m_SoundDescs[iChannel].fFadeSecond / m_SoundDescs[iChannel].fStartVolume));
		//(m_SoundDescs[iChannel].fStartVolume * m_SoundDescs[iChannel].fFadeSoundRatio)));

	SetChannelVolume(iChannel, fVolume);
}

CSound_Manager* CSound_Manager::Create()
{
	CSound_Manager* pInstance = new CSound_Manager();

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Create : CSound_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSound_Manager::Free()
{
	Safe_Delete_Array(m_pChannelArr);
	
	for (auto& Mypair : m_Sound)
	{
		Mypair.second->release();
	}
	m_Sound.clear();

	m_pSystem->release();
	m_pSystem->close();
}
