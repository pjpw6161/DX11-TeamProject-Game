#pragma once
#include "Base.h"

BEGIN(Engine)

class CCompute_RenderTarget final :
    public CBase
{
private:
	CCompute_RenderTarget(_dev pDevice, _context pContext);
	virtual ~CCompute_RenderTarget() = default;

public:
	HRESULT Init(_uint2 iSize, DXGI_FORMAT ePixelFormat);

public:
	ID3D11ShaderResourceView* Get_SRV() const {
		return m_pSRV;
	}

	ID3D11UnorderedAccessView* Get_UAV() const {
		return m_pUAV;
	}

private:
	_dev m_pDevice{ nullptr };
	_context m_pContext{ nullptr };

	_uint2 m_iSize{};

	ID3D11Texture2D* m_pTexture{ nullptr };
	ID3D11ShaderResourceView* m_pSRV{ nullptr };
	ID3D11UnorderedAccessView* m_pUAV = nullptr;

public:
	static CCompute_RenderTarget* Create(_dev pDevice, _context pContext, _uint2 iSize, DXGI_FORMAT ePixelFormat);
	virtual void Free() override;
};

END