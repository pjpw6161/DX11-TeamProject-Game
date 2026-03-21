#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CSurvivalGame final : public CGameObject
{
public:
	enum PATTERN
	{
		PATTERN_INIT,
		PATTERN_RANDOM_MISSILE,
		PATTERN_FLOOR,
		PATTERN_GUIDED_MISSILE,
		PATTERN_LASER,
		PATTERN_PIZZA,
		PATTERN_TANGHURU,
		PATTERN_SUICIDE_MONSTER,
		PATTERN_END
	};

private:
	CSurvivalGame(_dev pDevice, _context pContext);
	CSurvivalGame(const CSurvivalGame& rhs);
	virtual ~CSurvivalGame() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Init_Pattern(_float fTimeDelta);
	void Tick_Pattern(_float fTimeDelta);

private:
	PATTERN m_ePrePattern = PATTERN_END;
	PATTERN m_eCurPattern = PATTERN_END;

private:
	_float m_fTime = {};

private:
	_bool m_bGameStart = { false };

private:
	_uint m_iLauncherCount = {};


public:
	static CSurvivalGame* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END