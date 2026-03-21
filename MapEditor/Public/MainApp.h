#pragma once
#include "Base.h"
#include "MapEditor_Define.h"

BEGIN(Engine)
class CGameInstance;
class CRenderer;
END

BEGIN(MapEditor)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Init();
	void Tick(_float fTimeDelta);
	HRESULT Render();

private:
	ID3D11Device* m_pDevice{ nullptr };
	ID3D11DeviceContext* m_pContext{ nullptr };

private:
	CGameInstance* m_pGameInstance{ nullptr };
	CRenderer* m_pRenderer{ nullptr };
	_float m_fTimeAcc{};
	_uint m_iFrameCount{};

	class CImGui_Manager* m_pImGui_Manager{ nullptr };

private:
	HRESULT Open_Level(LEVEL_ID eLevelID);
	HRESULT Ready_Prototype_Component_For_Static();
	HRESULT Ready_Prototype_For_Loading();

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END