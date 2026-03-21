#pragma once

#include "Shader_Define.h"
#include "Level.h"

BEGIN(ShaderEdit)

class CShader_Loading final :
    public CLevel
{
private:
	CShader_Loading(_dev pDevice, _context pContext);
	virtual ~CShader_Loading() = default;

public:
	virtual HRESULT Init(LEVEL_ID eNextLevel);
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	LEVEL_ID m_eNextLevel{ LEVEL_END };
	class CShader_Loader* m_pLoader{ nullptr };

public:
	static CShader_Loading* Create(_dev pDevice, _context pContext, LEVEL_ID eNextLevel);
	virtual void Free() override;
};

END