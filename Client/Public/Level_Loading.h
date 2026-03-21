#pragma once
#include "Client_Define.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Loading final : public CLevel
{
private:
	CLevel_Loading(_dev pDevice, _context pContext);
	virtual ~CLevel_Loading() = default;

public:
	virtual HRESULT Init(LEVEL_ID eNextLevel);
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	LEVEL_ID m_eNextLevel{ LEVEL_END };
	class CLoader* m_pLoader{ nullptr };

private:
	HRESULT Ready_Layer_BackGround(const wstring& strLayerTag);
	HRESULT Ready_Layer_LoadingBar(const wstring& strLayerTag);
	HRESULT Ready_Layer_Icon(const wstring& strLayerTag);

public:
	static CLevel_Loading* Create(_dev pDevice, _context pContext, LEVEL_ID eNextLevel);
	virtual void Free() override;
};

END