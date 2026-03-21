#pragma once

#include "GameObject.h"
#include "MapEditor_Define.h"
#include "ImGui_Manager.h"

BEGIN(MapEditor)

// ui 형태로 그리는 직교투영을 통해서 그린다.

class CTerrain final : public CGameObject
{

public:
	enum TEXTURE { TYPE_DIFFUSE, TYPE_MASK, TYPE_BRUSH, TYPE_END };


private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg)override;
	virtual void Tick(_float fTimeDelta)override;
	virtual void Late_Tick(_float fTimeDelta)override;
	virtual HRESULT Render()override;

	void Set_MousePos(_float4 MousePos) { m_MousePos = MousePos; }
	void Set_BrushSize(_float iBrushSize) { m_iBrushSize = iBrushSize; }
	_bool Get_Mesh_Terrain() { return m_Info.isMesh; }
	void Mode(_bool isMode) { m_isMode = isMode; }

private:
	// 객체가 컴포넌트를 사용할 때 용이하게끔 멤버변수로 보관한다.
	CRenderer* m_pRendererCom = { nullptr };
	CTexture* m_pTextureCom[TYPE_END] = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CVIBuffer_Terrain* m_pVIBufferCom = { nullptr };


private:
	_float				m_fIndex = { 0.f };
	_float				m_fSizeX, m_fSizeY;
	_float				m_fX, m_fY;
	_float44			m_ViewMatrix, m_ProjMatrix;
	_uint				m_iTextureNum = { 0 };
	TERRAIN_INFO_MAPTOOL m_Info;

	_float4				m_MousePos{};
	_float				m_iBrushSize{ 1.f };
	_bool				m_isMode{ false };

private:
	HRESULT Add_Component();
	HRESULT Bind_ShaderResources();

public:
	// 원형 객체를 생성할 때
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

	// 실사용객체를 생성할 때 : 원형객체가 호출하여 사본(실사용객체)을 리턴한다 
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END
