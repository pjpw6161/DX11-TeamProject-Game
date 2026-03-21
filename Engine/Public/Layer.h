#pragma once
#include "Base.h"

BEGIN(Engine)

class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	class CComponent* Get_Component(const wstring& strComponentTag, _uint iIndex);

public:
	HRESULT Add_GameObejct(class CGameObject* pGameObejct);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
	void Release_DeadObjects();
	_uint Get_LayerSize();


private:
	list<class CGameObject*> m_GameObjects{};

public:
	static CLayer* Create();
	virtual void Free() override;
};

END