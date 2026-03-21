#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"


BEGIN(Client)
class CBlurTexture final : public COrthographicObject
{
public:
	typedef struct tagBlurTexDesc
	{
		_float  fDepth{1.f};
		_vec2	vSize;
		_vec2	vPosition;
		_vec4	vColor{ _vec4(1.f, 1.f, 1.f, 1.f) };

		_float	fFontSize;
		_vec2	vTextPosition;
		wstring strText;
		_vec4	vTextColor{ _vec4(1.f, 1.f, 1.f, 1.f) };

		wstring strTexture;
		wstring strMaskTexture;

		LEVEL_ID eLevelID;
	}BLURTEX_DESC;
private:
	CBlurTexture(_dev pDevice, _context pContext);
	CBlurTexture(const CBlurTexture& rhs);
	virtual ~CBlurTexture() = default;

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
	_float	m_fAmount{ 0.04f };

	_float	m_fFontSize;
	_vec2	m_vSize;
	_vec2	m_vPosition;


	_vec2	m_vTextPosition;
	wstring m_strText;
	_vec4	m_vTextColor;
	_vec4	m_vColor;

	wstring m_strTexture;

	LEVEL_ID m_eLevel;

	RECT				m_rcRect{};

public:
	const RECT& Get_Rect() const { return m_rcRect; }
	const _vec2& Get_Position() const { return m_vPosition; }
	void Set_Position(_vec2 vPos);
	void Set_Size(_float fSizeX, _float fSizeY, _float fFontSize = -1.f);
	void Set_Amount(_float fAmount) { m_fAmount = fAmount; }
public:
	static CBlurTexture* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END