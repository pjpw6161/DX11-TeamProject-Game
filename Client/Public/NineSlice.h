#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"


BEGIN(Client)
class CNineSlice final : public COrthographicObject
{
public:
	typedef struct tagSliceDesc
	{
		_float  fDepth{1.f};
		_vec2	vSize;
		_vec2	vPosition;

		_float	fFontSize;
		_vec2	vTextPosition;
		wstring strText;
		_vec4	vTextColor{ _vec4(1.f, 1.f, 1.f, 1.f) };
		_vec4	vTextBorderColor{ _vec4(0.f, 0.f, 0.f, 1.f) };

		wstring strTexture;

		LEVEL_ID eLevelID;
	}SLICE_DESC;
private:
	CNineSlice(_dev pDevice, _context pContext);
	CNineSlice(const CNineSlice& rhs);
	virtual ~CNineSlice() = default;

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
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	_float	m_fDuration{ 0.f };
	_float	m_fFontSize;

	_vec2	m_vSize;
	_vec2	m_vPosition;
	_vec2	m_vTextPosition;

	_vec4	m_vTextColor;
	_vec4	m_vTextBorderColor;

	wstring m_strText;
	wstring m_strTexture;

	LEVEL_ID m_eLevel;

	RECT				m_rcRect{};
	RECT				m_rcInitialRect{};

public:
	const RECT& Get_Rect() const { return m_rcRect; }
	const RECT& Get_InitialRect() const { return m_rcInitialRect; }
	const _vec2 Get_Position() const { return  m_vPosition; }
	const _vec2 Get_TransPosition() const { return _vec2(m_fX, m_fY); }
	void Set_Position(_vec2 vPos);
	void Set_Size(_float fSizeX, _float fSizeY, _float fFontSize = -1.f);
	void Set_Text(const wstring strText) { m_strText = strText; }
	CTransform* Get_Transform() { return m_pTransformCom; }

private:
	HRESULT Render_Texture();

public:
	static CNineSlice* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END