#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
private:
	CTexture(_dev pDevice, _context pContext);
	CTexture(const CTexture& rhs);
	~CTexture() = default;

public:
	virtual HRESULT Init_Prototype(const wstring& strTextureFilePath, _uint iNumTextures);
	virtual HRESULT Init(void* pArg) override;

public:
	ID3D11ShaderResourceView* Get_SRV(_uint iTextureIndex = 0);

public:
	HRESULT Bind_ShaderResource(class CShader* pShader, const _char* pVariableName, _uint iTextureIndex = 0);
	HRESULT Bind_ShaderResourceArray(class CShader* pShader, const _char* pVariableName);

private:
	_uint m_iNumTextures{};
	vector<ID3D11ShaderResourceView*> m_SRVs{};
public:
	static CTexture* Create(_dev pDevice, _context pContext, const wstring& strTextureFilePath, _uint iNumTextures = 1);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END