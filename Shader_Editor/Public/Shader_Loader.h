#pragma once

#include "Shader_Define.h"
#include "Base.h"


BEGIN(Engine)
class CGameInstance;
END

BEGIN(ShaderEdit)


class CShader_Loader final :
    public CBase
{
private:
	CShader_Loader(_dev pDevice, _context pContext);
	virtual ~CShader_Loader() = default;

public:
	HRESULT Init(LEVEL_ID eNextLevel);
	HRESULT Begin_Thread();
	HRESULT End_Thread();
	HRESULT Show_LoadingText();
	_bool isFinished();

public:
	HRESULT Loading_LevelResources();

private:
	_dev m_pDevice{ nullptr };
	_context m_pContext{ nullptr };
	CGameInstance* m_pGameInstance{ nullptr };
	LEVEL_ID m_eNextLevel{ LEVEL_END };
	wstring m_strLoadingText{};
	_bool m_isFinished{ false };

private:
	HANDLE m_hThread{};
	CRITICAL_SECTION m_Critical_Section{};

private:
	HRESULT Load_Test();

public:
	static CShader_Loader* Create(_dev pDevice, _context pContext, LEVEL_ID eNextLevel);
	virtual void Free() override;
};

END