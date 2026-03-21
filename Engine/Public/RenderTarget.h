#pragma once
#include "Base.h"

BEGIN(Engine)

class CRenderTarget final : public CBase
{
private:
	CRenderTarget(_dev pDevice, _context pContext);
	virtual ~CRenderTarget() = default;

public:
	ID3D11RenderTargetView* Get_RenderTargetView();
	ID3D11Texture2D* Get_Texture2D();

public:
	HRESULT Init(_uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vColor, const _bool isMultiSample);
	void Clear();
	HRESULT Bind_ShaderResourceView(class CShader* pShader, const _char* pVariableName);

	HRESULT Copy_Texture(ID3D11Resource* pTexture);

#ifdef _DEBUG
	HRESULT Ready_Debug(_float2 vPos, _float2 vSize);
	HRESULT Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif // _DEBUG

	//¿ø¸í
public:
	ID3D11ShaderResourceView* Get_SRV() const {
		return m_pShaderResourceView;
	}

private:
	_dev m_pDevice{ nullptr };
	_context m_pContext{ nullptr };

	ID3D11Texture2D* m_pTexture{ nullptr };
	ID3D11RenderTargetView* m_pRenderTargetView{ nullptr };
	ID3D11ShaderResourceView* m_pShaderResourceView{ nullptr };

	_float4 m_vClearColor{};

#ifdef _DEBUG
	_float44 m_WorldMatrix{};
#endif // _DEBUG


public:
	static CRenderTarget* Create(_dev pDevice, _context pContext, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vColor, const _bool isMultiSample);
	virtual void Free() override;
};

END