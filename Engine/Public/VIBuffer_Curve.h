#pragma once
#include "VIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Curve : public CVIBuffer
{
private:
	CVIBuffer_Curve(_dev pDevice, _context pContext);
	CVIBuffer_Curve(const CVIBuffer_Curve& rhs);
	virtual ~CVIBuffer_Curve() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	HRESULT Render();
	void		Get_ControlPoints(_mat* pOutPoints);
	void		Set_ControlPoints(_mat& Points);
	_float3		Get_CurvePos(_uint iIndex);

public:
	void Modify_Line();

private:
	_mat m_matControlPoints{};
	_float3* m_pCurvePos{};

public:
	static CVIBuffer_Curve* Create(_dev pDevice, _context pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};
END
