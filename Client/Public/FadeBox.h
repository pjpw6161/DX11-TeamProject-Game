#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CFadeBox final : public COrthographicObject
{
public:
	typedef struct tagFadeDesc
	{
		_float fIn_Duration{};
		_float fMid_Duration{};
		_float fOut_Duration{};
		_vec4 vColor{};
		_float fMaxAlpha{ 1.f };
		_bool isInfiniteLoop{};
		_bool* phasFadeCompleted{};
	}FADE_DESC;

private:
	CFadeBox(_dev pDevice, _context pContext);
	CFadeBox(const CFadeBox& rhs);
	virtual ~CFadeBox() = default;

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
	//CTexture* m_pTextureCom{ nullptr };

private:
	_float m_fAlpha{ 0.f };
	_float m_fTime{ 0.f };
	FADE_DESC m_Desc{ 0.f };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CFadeBox* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END