#include "pch.h"
#include "Shader_Loading.h"

#include "Shader_Loader.h"
#include "Level_Test.h"

CShader_Loading::CShader_Loading(_dev pDevice, _context pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CShader_Loading::Init(LEVEL_ID eNextLevel)
{
	m_pGameInstance->Set_CurrentLevelIndex(LEVEL_LOADING);

	m_eNextLevel = eNextLevel;

	m_pLoader = CShader_Loader::Create(m_pDevice, m_pContext, m_eNextLevel);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

void CShader_Loading::Tick(_float fTimeDelta)
{
	m_pLoader->Show_LoadingText();

	if (m_pLoader->isFinished()) {
		CLevel* pLevel = nullptr;

		switch (m_eNextLevel)
		{
		case ShaderEdit::LEVEL_ShaderTest:
			pLevel = CLevel_Test::Create(m_pDevice, m_pContext);
			break;
		}

		if (nullptr == pLevel) {
			MSG_BOX("레벨 Create 생성 실패 ㅅㄱ");
			return;
		}

		if (FAILED(m_pGameInstance->Open_Level(m_eNextLevel, pLevel))) {
			MSG_BOX("오픈 레벨 실패 ㅅㄱ");
		}
		else
			return;
	}
}

HRESULT CShader_Loading::Render()
{
	return S_OK;
}

CShader_Loading* CShader_Loading::Create(_dev pDevice, _context pContext, LEVEL_ID eNextLevel)
{
	CShader_Loading* pInstance = new CShader_Loading(pDevice, pContext);

	if (FAILED(pInstance->Init(eNextLevel)))
	{
		MSG_BOX("Failed to Create : CShader_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CShader_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
