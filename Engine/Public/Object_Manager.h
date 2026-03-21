#pragma once
#include "Base.h"

BEGIN(Engine)

class CObject_Manager final : public CBase
{
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	HRESULT Init(_uint iNumLevels);
	HRESULT Add_Prototype(const wstring& strPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_Layer(_uint iLevelIndex, const wstring strLayerTag, const wstring& strPrototypeTag, void* pArg);
	_uint Get_LayerSize(_uint iLevelIndex, const wstring& strLayerTag);
	CGameObject* Clone_Object(const wstring& strPrototypeTag, void* pArg);
	void Tick(_float fTimeDelta);
	void Late_Tick(_float fTimeDelta);
	void Release_DeadObjects();
	void Clear(_uint iLevelIndex);

public:
	class CComponent* Get_Component(_uint iLevelIndex, const wstring& strLayerTag, const wstring& strComponentTag, _uint iIndex);

private:
	using LAYERS = map<const wstring, class CLayer*>;
	
	LAYERS* m_pLayers{ nullptr };
	map<const wstring, class CGameObject*> m_Prototype;
	_uint m_iNumLevels{0};

public:
	class CGameObject* Find_Prototype(const wstring& strPrototypeTag);
	class CLayer* Find_Layer(_uint iLevelIndex, const wstring& strLayerTag);

public:
	static CObject_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END