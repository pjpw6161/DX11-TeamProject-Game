#pragma once
#include "Base.h"
#include "Effect_Define.h"

BEGIN(Engine)
class CGameInstance;
class CRenderer;
END

BEGIN(Effect)

class CEffectApp final : public CBase
{
private:
	CEffectApp();
	virtual ~CEffectApp() = default;

public:
	HRESULT Init();
	void Tick(_float fTimeDelta);
	HRESULT Render();

private:
	_dev m_pDevice{ nullptr };
	_context m_pContext{ nullptr };

private:
	class CImgui_Manager* m_pImguiMgr{ nullptr };
	CGameInstance* m_pGameInstance{ nullptr };
	CRenderer* m_pRenderer{ nullptr };
	_float m_fTimeAcc{};
	_uint m_iFrameCount{};
	vector<string> m_TextureList{};
	vector<string> m_ModelList{};

private:
	HANDLE m_hThread{};
	CRITICAL_SECTION m_Critical_Section{};
	_bool m_bLoadComplete{};
	wstring m_strLoadingText{};
	_uint m_iNumDots{};
	_float m_fLoadingTime{};

private:
	HRESULT Ready_Prototype_Component_For_Static();
	HRESULT Ready_Loading_Screen();

public:
	void Begin_Thread();
	HRESULT Ready_Prototype_GameObject();
	void End_Thread();

public:
	static CEffectApp* Create();
	virtual void Free() override;
};

END