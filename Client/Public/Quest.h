#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"
#include "QuestBox.h"

BEGIN(Client)

class CQuest final : public COrthographicObject
{

private:
	CQuest(_dev pDevice, _context pContext);
	CQuest(const CQuest& rhs);
	virtual ~CQuest() = default;

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
	_bool										m_bNewQuestIn{ false };
	_bool										m_isActive{ false };

	_float										m_fTime{ 0.f };
	_float										m_fMin{ 0.f };
	_float										m_fMax{ 0.f };

	CGameObject*								m_pSymbol{ nullptr };
	CGameObject*								m_pNotify{ nullptr };
	CGameObject*								m_pButton{ nullptr };
	//순서때매 map으로 하면 안되겠다....
	//map <const wstring, class CQuestBox*>		m_QuestMap;
	vector<class CQuestBox*>					m_vecQuest;

public:
	HRESULT	 Set_Quest(CQuestBox::QUESTBOX_DESC& QuestBoxDesc);
	void	 Sort_Quest();
	_bool	 Update_Quest(const wstring& strQuest);

private:
	HRESULT Add_Parts();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CQuest* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END