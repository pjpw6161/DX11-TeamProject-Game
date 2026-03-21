#pragma once
#include "Client_Define.h"
#include "BlendObject.h"

BEGIN(Client)

struct SURFACETRAIL_DESC
{
	_color vColor{};
	_uint iNumVertices{};
	wstring strMaskTextureTag{};
	_uint iPassIndex{ 1 };
};

class CCommonSurfaceTrail final : public CBlendObject
{
private:
	CCommonSurfaceTrail(_dev pDevice, _context pContext);
	CCommonSurfaceTrail(const CCommonSurfaceTrail& rhs);
	virtual ~CCommonSurfaceTrail() = default;

public:
	void On();

	void Set_Color(const _color vColor);

public:
	HRESULT Init_Prototype() override;
	HRESULT Init(void* pArg) override;
	void Tick(_vec3 vTopPos, _vec3 vBottomPos);
	void Late_Tick(_float fTimeDelta) override;
	HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };
	CVIBuffer_Trail_Surface* m_pTrailBufferCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CTexture* m_pMaskTextureCom{ nullptr };

private:
	_bool m_bNoRender{};
	_float m_fDissolveRatio{};

	list<_vec3> m_TopPosList{};
	list<_vec3> m_BottomPosList{};

	SURFACETRAIL_DESC m_Info{};
	_vec3* m_TopPosArray{};
	_vec3* m_BottomPosArray{};
	_float* m_AlphaArray{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CCommonSurfaceTrail* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END