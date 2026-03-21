#pragma once

#include "GameObject.h"
#include "Client_Define.h"

BEGIN(Client)

class CLake final : public CGameObject
{

public:
	typedef struct tagWaterDesc {
		_vec3 vPos{};
		_vec2 vSize{};
		_float fReflectionScale = 0.f;
		_float fRefractionScale = 0.f;
		_float fWaterPos = 0.f;
		_float fWaterSpeed = 0.f;
	}WATER_DESC;
private:
	CLake(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLake(const CLake& rhs);
	virtual ~CLake() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg)override;
	virtual void Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render()override;

private:
	// 객체가 컴포넌트를 사용할 때 용이하게끔 멤버변수로 보관한다.
	CRenderer* m_pRendererCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Rect_LookZ* m_pVIBufferCom = { nullptr };


private:
	_mat m_ReflectionViewMatrix{};

	WATER_DESC m_Desc{};

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

public:
	// 원형 객체를 생성할 때
	static CLake* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	// 실사용객체를 생성할 때 : 원형객체가 호출하여 사본(실사용객체)을 리턴한다 
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END
