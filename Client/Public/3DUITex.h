#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class C3DUITex final : public COrthographicObject
{
public:
	typedef struct tagNameTagDesc
	{
		wstring strTexture;
		_vec2   vSize;
		_vec3	vPosition;
		class CTransform* pParentTransform;
		LEVEL_ID eLevelID;

		_float	fFontSize{};
		_vec2	vTextPosition{};
		wstring strText{TEXT("")};
		_vec4	vTextColor{ _vec4(1.f, 1.f, 1.f, 1.f) };

	}UITEX_DESC;
private:
	C3DUITex(_dev pDevice, _context pContext);
	C3DUITex(const C3DUITex& rhs);
	virtual ~C3DUITex() = default;

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
	LEVEL_ID		m_eLevel{};
	VTPass			m_ePass{ VTPass_UI };
	wstring			m_strTexture;
	_vec2			m_vSize;
	_vec3			m_vPosition{};
	CTransform*		m_pParentTransform{ nullptr };

	_vec2			m_vTextPos{};
	_float			m_fFontSize{};
	_vec2			m_vTextPosition{};
	wstring			m_strText{ TEXT("") };
	_vec4			m_vTextColor{ _vec4(1.f, 1.f, 1.f, 1.f) };

	RECT				m_rcRect{};

public:
	const _vec2 Get_Size() const { return _vec2(m_fSizeX, m_fSizeY); };
	void Set_Size(_float fSizeX, _float fSizeY);
	void Set_Pass(VTPass ePass) { m_ePass = ePass; }
	void Set_Position(_vec3 vPos);
	const RECT& Get_Rect() const { return m_rcRect; }

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static C3DUITex* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END