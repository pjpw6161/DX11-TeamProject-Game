#pragma once
#include "Base.h"

BEGIN(Engine)

class CFrustum final : public CBase
{
private:
	CFrustum();
	virtual ~CFrustum() = default;

public:
	HRESULT Init();
	void Tick();

public:
	_bool IsIn_Fov_World(_fvector vPos, _float fRange);
	void Transform_ToLocalSpace(_fmatrix vWorldInversed);
	_bool IsIn_Fov_Local(_fvector vPos, _float fRange);

private:
	class CGameInstance* m_pGameInstance{ nullptr };

	_float3 m_pPoints[8]{};
	_float3 m_pPoints_World[8]{};

	_float4 m_pPlanes_World[6]{};
	_float4 m_pPlanes_Local[6]{};

private:
	HRESULT Make_Planes(const _float3* pPoints, _Inout_ _float4* pPlanes);

public:
	static CFrustum* Create();
	virtual void Free() override;
};

END