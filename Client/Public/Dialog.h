#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CDialog final : public COrthographicObject
{
public:
	typedef struct tagNameTagDesc
	{
		wstring strText;
		_vec3	vPosition;
		class CTransform* pParentTransform;
		LEVEL_ID eLevelID;
	}DIALOG_DESC;

private:
	CDialog(_dev pDevice, _context pContext);
	CDialog(const CDialog& rhs);
	virtual ~CDialog() = default;

public:
	void Set_Text(const wstring& strText);

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
	CTexture* m_pTextureTailCom{ nullptr };

private:
	LEVEL_ID		m_eLevel{};
	wstring			m_strText{};

	_vec2			m_vTextPos{};
	_vec2			m_vTextSize{};
	_vec3			m_vPosition{};
	CTransform*		m_pParentTransform{ nullptr };

private:
	HRESULT Render_Texture();

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CDialog* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END