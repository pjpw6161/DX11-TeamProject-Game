#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CPop_QuestIn final : public COrthographicObject
{
public:
	typedef struct tagQuestInDesc
	{
		_bool	isMain;
		_float	fExp;
		_uint	iMoney;
		wstring strQuestTitle;
		wstring strText;

	}QUESTIN_DESC;
private:
	CPop_QuestIn(_dev pDevice, _context pContext);
	CPop_QuestIn(const CPop_QuestIn& rhs);
	virtual ~CPop_QuestIn() = default;

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
	_bool			m_isMain{ false };
	_uint			m_iMoney{ 0 };
	_float			m_fExp{ 0.f };
	_float			m_fDir{ -1.f };

	wstring			m_strQuestTitle;
	wstring			m_strText;


	_float			m_fTime{};
	_float			m_fDeadTime{};
	_float			m_fButtonTime{};
	_float2			m_fStartButtonPos{};


	CGameObject*	m_pBackground{ nullptr };
	CGameObject*	m_pBorder{ nullptr };
	CGameObject*	m_pButton{ nullptr };
	CGameObject*	m_pExclamationMark{ nullptr };
	CGameObject*	m_pMoney{ nullptr };
	CGameObject*	m_pExp{ nullptr };

private:
	HRESULT Add_Parts();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CPop_QuestIn* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END