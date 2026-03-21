#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Instancing abstract : public CVIBuffer
{
protected:
	struct ParticleParams
	{
		_uint iNumInstances;
		_uint iNumUse;
		_float fTimeDelta;
		_float fAppearRatio;
		
		_vec4 vGravityDir;

		float fDissolveRatio;
		int isLoop;
		int bApplyGravity;
		int isFirstUpdate;

		_mat WorldMatrix;

		_vec4 padding;
	};

protected:
	CVIBuffer_Instancing(_dev pDevice, _context pContext);
	CVIBuffer_Instancing(const CVIBuffer_Instancing& rhs);
	virtual ~CVIBuffer_Instancing() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Update(_float fTimeDelta, _mat WorldMatrix, _int iNumUse = -1, _bool bApplyGravity = false, _vec3 vGravityDir = _vec3(), _float fAppearRatio = 0.f, _float fDissolveRatio = 0.8f);
	virtual HRESULT Render() override;

protected:
	D3D11_BUFFER_DESC m_InstancingBufferDesc{};
	D3D11_SUBRESOURCE_DATA m_InstancingInitialData{};

	ID3D11Buffer* m_pVBInstance{ nullptr };

	_uint m_iNumInstances{};
	_uint m_iIndexCountPerInstance{};
	_uint m_iInstanceStride{};

	_bool m_isFirstUpdate{ true };
	_bool m_isLoop{};

protected: // compute
	ID3D11Buffer* m_pVSRB{ nullptr };
	ID3D11Buffer* m_pVUAVB{ nullptr };
	class CCompute_Shader* m_pComputeShader{ nullptr };
	ID3D11ShaderResourceView* m_pSRV{ nullptr };
	ID3D11UnorderedAccessView* m_pUAV{ nullptr };


public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END