#pragma once
#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CSky final : public CGameObject
{
	CSky(_dev pDevice, _context pContext);
	CSky(const CSky& rhs);
	virtual ~CSky() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Reflection(_float4 vClipPlane);

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CModel* m_pModelCom{ nullptr };
	CTexture* m_Textures[19]{};

private:
	_float m_fLightning_Time{};
	_uint m_iTextureIndex{};


private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSky* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END