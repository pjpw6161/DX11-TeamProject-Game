#pragma once
#include "Base.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
private:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	LIGHT_DESC* Get_LightDesc(_uint iLevelIndex, const wstring& strLightTag);
	HRESULT Add_Light(_uint iLevelIndex, const wstring& strLightTag, const LIGHT_DESC& LightDesc);
	HRESULT Delete_Light(_uint iLevelIndex, const wstring& strLightTag, _float fDimmerDuration);
	HRESULT Bind_ViewProjMatrix(_uint iLevelIndex, const wstring& strLightTag, class CShader* pShader, const _char* pViewVariableName, const _char* pProjVariableName);

public:
	HRESULT Init(_uint iNumLevels);
	void Tick(_uint iLevelIndex, _float fTimeDelta);
	void Clear(_uint iLevelIndex);
	HRESULT Render(_uint iLevelIndex, class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);

private:
	_uint m_iNumLevels{};
	map<const wstring,class CLight*>* m_pLights{};

private:
	class CLight* Find_Light(_uint iLevelIndex, const wstring& strLightTag);

public:
	static CLight_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END