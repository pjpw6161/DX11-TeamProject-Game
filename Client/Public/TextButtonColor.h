#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"


BEGIN(Client)
class CTextButtonColor final : public COrthographicObject
{
public:
	typedef struct tagTextButtonDesc
	{
		_float  fDepth{1.f};
		_float  fAlpha{1.f};

		_vec2	vSize;
		_vec2	vPosition;
		_vec4	vColor;

		_float	fFontSize;
		_vec2	vTextPosition;
		wstring strText;
		_vec4	vTextColor{ _vec4(1.f, 1.f, 1.f, 1.f) };

		wstring strTexture;
		wstring strTexture2;

		LEVEL_ID eLevelID;
	}TEXTBUTTON_DESC;
private:
	CTextButtonColor(_dev pDevice, _context pContext);
	CTextButtonColor(const CTextButtonColor& rhs);
	virtual ~CTextButtonColor() = default;

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
	CTexture* m_pMaskTextureCom{ nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	VTPass				m_ePass{ VTPass_Mask_Color };
	_float				m_fDuration{ 0.f };
	_float				m_fAlpha{ 1.f };
	_float				m_fMoveX{ 0.f };
	_float				m_fMoveY{ 0.f };
	_float				m_fFactor{ 1.f };
	_float				m_fIndex{ 1.f };

	_int2				m_vNumSprite{};
	_vec2	m_vSize;
	_vec2	m_vPosition;
	_vec4	m_vColor;

	_float	m_fFontSize;
	_vec2	m_vTextPosition;
	wstring m_strText;
	_vec4	m_vTextColor;

	wstring m_strTexture;
	wstring m_strTexture2;

	LEVEL_ID m_eLevel;

	RECT				m_rcRect{};
	RECT				m_rcInitialRect{};

public:
	const RECT& Get_Rect() const { return m_rcRect; }
	const RECT& Get_InitialRect() const { return m_rcInitialRect; }
	void Set_Size(_float fSizeX, _float fSizeY, _float fFontSize = -1.f);
	void Set_Color(_vec4 vColor) { m_vColor = vColor; }
	const _vec2& Get_Position() const { return m_vPosition; }
	const _vec2 Get_TransPosition() const { return _vec2(m_fX, m_fY); }
	const _vec2 Get_Size() const { return _vec2(m_fSizeX, m_fSizeY); }
	const _vec4& Get_Color() const { return m_vColor; }
	const _float& Get_Alpha() const { return m_fAlpha; }
	void Set_Position(_vec2 vPos);
	void Set_Text(const wstring strText) { m_strText = strText; }
	void Set_Alpha(const _float fAlpha) { m_fAlpha = fAlpha; }
	void Set_Pass(VTPass ePass) { m_ePass = ePass; }
	void Set_UV(_float fX, _float fY) { m_fMoveX = fX; m_fMoveY = fY; }
	void Set_Factor(_float fFactor) { m_fFactor = fFactor; }
	void Set_Index(_float fIndex) { m_fIndex = fIndex; }
	void Set_Sprite(_int2 vNumSprite) { m_vNumSprite = vNumSprite; }
	void Rotate_Button(_vec4 vAxis, _float fAngle);

public:
	static CTextButtonColor* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END