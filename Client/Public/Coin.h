#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CCoin final : public COrthographicObject
{
private:
	CCoin(_dev pDevice, _context pContext);
	CCoin(const CCoin& rhs);
	virtual ~CCoin() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CVIBuffer_Rect* m_pVIBufferCom{ nullptr };
	CTexture* m_pTextureCom{ nullptr };

private:
	_uint			m_iCoin{};
	_float			m_fTime{};
	_float2			m_vRatio{};

public:
	const _uint& Get_Coin() const { return m_iCoin; }
	void Set_Coin(_uint iCoin) { m_iCoin += iCoin; }

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CCoin* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END