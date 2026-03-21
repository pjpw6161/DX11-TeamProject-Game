#pragma once
#include "Base.h"

BEGIN(Engine)

class CLight final : public CBase
{
	CLight();
	virtual ~CLight() = default;

public:
	LIGHT_DESC* Get_LightDesc();
	void Dim(_float fDuration);
	const _bool& Is_Dead();

public:
	HRESULT Init(const LIGHT_DESC& LightDeesc);
	void Tick(_float fTimeDelta);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
	HRESULT Bind_ViewProjMatrix(class CShader* pShader, const _char* pViewVariableName, const _char* pProjVariableName);

private:
	LIGHT_DESC m_LightDesc{};
	_bool m_shouldDim{};
	_float m_fDimmerTimer{};
	_float m_fDimmerDuration{};
	_bool m_isDead{};

	_vec4 vOriginDiffuse{};
	_vec4 vOriginAmbient{};
	_vec4 vOriginSpecular{};

public:
	static CLight* Create(const LIGHT_DESC& LightDesc);
	virtual void Free() override;
};

END