#include "Effect_Manager.h"
#include "Effect_Dummy.h"

IMPLEMENT_SINGLETON(CEffect_Manager)

CEffect_Manager::CEffect_Manager()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

void CEffect_Manager::Tick(_float fTimeDelta)
{
	_uint iCurrLevel = m_pGameInstance->Get_CurrentLevelIndex();

	for (auto iter = m_Effects[iCurrLevel].begin(); iter != m_Effects[iCurrLevel].end();)
	{
		iter->second->Tick(fTimeDelta);

		if (iter->second->isDead())
		{
			Safe_Release(iter->second);
			iter = m_Effects[iCurrLevel].erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void CEffect_Manager::Late_Tick(_float fTimeDelta)
{
	_uint iCurrLevel = m_pGameInstance->Get_CurrentLevelIndex();

	for (auto& pEffect : m_Effects[iCurrLevel])
	{
		pEffect.second->Late_Tick(fTimeDelta);
	}
}

_bool CEffect_Manager::Has_Created(const void* pMatrixKey)
{
	_uint iCurrLevel = m_pGameInstance->Get_CurrentLevelIndex();

	auto iter = m_Effects[iCurrLevel].find(pMatrixKey);
	if (iter == m_Effects[iCurrLevel].end())
	{
		return false;
	}
	return true;
}

EffectInfo CEffect_Manager::Get_EffectInformation(const wstring& strEffectTag)
{
	auto iter = m_EffectInfos.find(strEffectTag);
	if (iter == m_EffectInfos.end())
	{
		MSG_BOX("Failed to Find Effect Information");
		return EffectInfo();
	}

	return iter->second;
}

HRESULT CEffect_Manager::Add_Layer_Effect(EffectInfo& pInfo, const _bool isStaticLevel)
{
	_uint iLevel = LEVEL_STATIC;
	if (not isStaticLevel)
	{
		iLevel = m_pGameInstance->Get_CurrentLevelIndex();
	}
	return m_pGameInstance->Add_Layer(iLevel, L"Layer_Effect", L"Prototype_GameObject_EffectDummy", &pInfo);
}

CEffect_Dummy* CEffect_Manager::Clone_Effect(EffectInfo& pInfo)
{
	return dynamic_cast<CEffect_Dummy*>(m_pGameInstance->Clone_Object(L"Prototype_GameObject_EffectDummy", &pInfo));
}

void CEffect_Manager::Create_Effect(const wstring& strEffectTag, _mat* pMatrix, const _bool& isFollow)
{
	_uint iCurrLevel = m_pGameInstance->Get_CurrentLevelIndex();

	EffectInfo Info = Get_EffectInformation(strEffectTag);
	Info.pMatrix = pMatrix;
	Info.isFollow = isFollow;

	if (not Info.isFollow)
	{
		Add_Layer_Effect(Info);
	}
	else
	{
		auto iter = m_Effects[iCurrLevel].find(pMatrix);
		if (iter == m_Effects[iCurrLevel].end())
		{
			CEffect_Dummy* pEffect = Clone_Effect(Info);

			m_Effects[iCurrLevel].emplace(pMatrix, pEffect);
		}
	}
}

void CEffect_Manager::Delete_Effect(const void* pMatrix)
{
	_uint iCurrLevel = m_pGameInstance->Get_CurrentLevelIndex();

	auto iter = m_Effects[iCurrLevel].find(pMatrix);
	if (iter == m_Effects[iCurrLevel].end())
	{
		return;
	}

	Safe_Release(iter->second);
	m_Effects[iCurrLevel].erase(iter);
}

void CEffect_Manager::Clear(_uint iLevelIndex)
{
	for (auto& Pair : m_Effects[iLevelIndex])
	{
		Safe_Release(Pair.second);
	}
	m_Effects[iLevelIndex].clear();
}

void CEffect_Manager::Register_Callback()
{
	CGameInstance::Func_CreateFX func_Create = [this](auto... args) { return Create_Effect(args...); };
	m_pGameInstance->Register_CreateEffect_Callback(func_Create);

	CGameInstance::Func_DeleteFX func_Delete = [this](auto... args) { return Delete_Effect(args...); };
	m_pGameInstance->Register_DeleteEffect_Callback(func_Delete);

	CGameInstance::Func_TickFX func_Tick = [this](auto... args) { return Tick(args...); };
	CGameInstance::Func_TickFX func_LateTick = [this](auto... args) { return Late_Tick(args...); };
	m_pGameInstance->Register_Tick_LateTick_Callback(func_Tick, func_LateTick);

	CGameInstance::Func_HasCreatedFX func_HasCreated = [this](auto... args) { return Has_Created(args...); };
	m_pGameInstance->Register_HasCreated_Callback(func_HasCreated);

	CGameInstance::Func_ClearFX func_Clear = [this](auto... args) { return Clear(args...); };
	m_pGameInstance->Register_Clear_Callback(func_Clear);
}

HRESULT CEffect_Manager::Read_EffectFile()
{
	string strInputFilePath = "../../Client/Bin/EffectData/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		EffectInfo Info{};
		if (entry.is_regular_file())
		{
			wstring extension = entry.path().extension().wstring();
			if (entry.path().extension().wstring() != L".effect")
			{
				continue;
			}

			ifstream File(entry.path().c_str(), ios::binary);

			if (File.is_open())
			{
				File.read(reinterpret_cast<_char*>(&Info.iType), sizeof Info.iType);
				File.read(reinterpret_cast<_char*>(&Info.isSprite), sizeof Info.isSprite);
				File.read(reinterpret_cast<_char*>(&Info.vNumSprites), sizeof Info.vNumSprites);
				File.read(reinterpret_cast<_char*>(&Info.fSpriteDuration), sizeof Info.fSpriteDuration);
				File.read(reinterpret_cast<_char*>(&Info.PartiDesc), sizeof Info.PartiDesc);
				File.read(reinterpret_cast<_char*>(&Info.iNumInstances), sizeof Info.iNumInstances);
				File.read(reinterpret_cast<_char*>(&Info.fLifeTime), sizeof Info.fLifeTime);
				File.read(reinterpret_cast<_char*>(&Info.vColor), sizeof Info.vColor);
				File.read(reinterpret_cast<_char*>(&Info.iPassIndex), sizeof Info.iPassIndex);
				File.read(reinterpret_cast<_char*>(&Info.vSize), sizeof Info.vSize);
				File.read(reinterpret_cast<_char*>(&Info.vPosOffset), sizeof Info.vPosOffset);
				File.read(reinterpret_cast<_char*>(&Info.vSizeDelta), sizeof Info.vSizeDelta);
				File.read(reinterpret_cast<_char*>(&Info.bApplyGravity), sizeof Info.bApplyGravity);
				File.read(reinterpret_cast<_char*>(&Info.vGravityDir), sizeof Info.vGravityDir);
				File.read(reinterpret_cast<_char*>(&Info.fDissolveDuration), sizeof Info.fDissolveDuration);
				File.read(reinterpret_cast<_char*>(&Info.bSkipBloom), sizeof Info.bSkipBloom);
				File.read(reinterpret_cast<_char*>(&Info.fUnDissolveDuration), sizeof Info.fUnDissolveDuration);
				File.read(reinterpret_cast<_char*>(&Info.vUVInit), sizeof Info.vUVInit);
				File.read(reinterpret_cast<_char*>(&Info.vUVDelta), sizeof Info.vUVDelta);
				File.read(reinterpret_cast<_char*>(&Info.isRandomSprite), sizeof Info.isRandomSprite);
				File.read(reinterpret_cast<_char*>(&Info.hasLight), sizeof Info.hasLight);
				File.read(reinterpret_cast<_char*>(&Info.Light_Desc), sizeof Info.Light_Desc);
				File.read(reinterpret_cast<_char*>(&Info.isFixedIndex), sizeof Info.isFixedIndex);
				File.read(reinterpret_cast<_char*>(&Info.iFixedSpriteIndex), sizeof Info.iFixedSpriteIndex);
				File.read(reinterpret_cast<_char*>(&Info.isUVLoop), sizeof Info.isUVLoop);
				File.read(reinterpret_cast<_char*>(&Info.fAlphaInit), sizeof Info.fAlphaInit);
				File.read(reinterpret_cast<_char*>(&Info.fAlphaDelta), sizeof Info.fAlphaDelta);
				File.read(reinterpret_cast<_char*>(&Info.fRectRotationAngle), sizeof Info.fRectRotationAngle);
				File.read(reinterpret_cast<_char*>(&Info.isBillboard), sizeof Info.isBillboard);
				File.read(reinterpret_cast<_char*>(&Info.vBillboardRotation), sizeof Info.vBillboardRotation);
				File.read(reinterpret_cast<_char*>(&Info.fPartiDissolveRatio), sizeof Info.fPartiDissolveRatio);
				File.read(reinterpret_cast<_char*>(&Info.fPartiAppearRatio), sizeof Info.fPartiAppearRatio);

				size_t iNameSize{};

				{
					_tchar* pBuffer{};

					File.read(reinterpret_cast<_char*>(&iNameSize), sizeof size_t);
					pBuffer = new _tchar[iNameSize / sizeof(_tchar)];
					File.read(reinterpret_cast<_char*>(pBuffer), iNameSize);
					Info.strDiffuseTexture = pBuffer;
					Safe_Delete_Array(pBuffer);
					iNameSize = {};

					File.read(reinterpret_cast<_char*>(&iNameSize), sizeof size_t);
					pBuffer = new _tchar[iNameSize / sizeof(_tchar)];
					File.read(reinterpret_cast<_char*>(pBuffer), iNameSize);
					Info.strMaskTexture = pBuffer;
					Safe_Delete_Array(pBuffer);
					iNameSize = {};

					File.read(reinterpret_cast<_char*>(&iNameSize), sizeof size_t);
					pBuffer = new _tchar[iNameSize / sizeof(_tchar)];
					File.read(reinterpret_cast<_char*>(pBuffer), iNameSize);
					Info.strDissolveTexture = pBuffer;
					Safe_Delete_Array(pBuffer);
					iNameSize = {};

					File.read(reinterpret_cast<_char*>(&iNameSize), sizeof size_t);
					pBuffer = new _tchar[iNameSize / sizeof(_tchar)];
					File.read(reinterpret_cast<_char*>(pBuffer), iNameSize);
					Info.strUnDissolveTexture = pBuffer;
					Safe_Delete_Array(pBuffer);
					iNameSize = {};
				}

				{
					_char* pBuffer{};
					File.read(reinterpret_cast<_char*>(&iNameSize), sizeof size_t);
					pBuffer = new _char[iNameSize];
					File.read(pBuffer, iNameSize);
					Info.strModel = pBuffer;
					Safe_Delete_Array(pBuffer);
					iNameSize = {};
				}

				m_EffectInfos.emplace(entry.path().stem().wstring(), Info);

				File.close();
			}
			else
			{
				return E_FAIL;
			}
		}
	}

	return S_OK;
}

void CEffect_Manager::Free()
{
	for (size_t i = 0; i < LEVEL_END; i++)
	{
		for (auto& pEffect : m_Effects[i])
		{
			Safe_Release(pEffect.second);
		}
		m_Effects[i].clear();
	}

	Safe_Release(m_pGameInstance);
}
