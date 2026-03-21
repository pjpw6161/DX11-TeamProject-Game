#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CQuestBox final : public COrthographicObject
{
public:
	typedef struct tagQuestConditionDesc
	{
		_bool isMain;
		_uint iNum;
		_float fMin;
		_float fMax;
		_float2 vPosition;
		wstring strQuestTitle;
		wstring strText;
	}QUESTBOX_DESC;

private:
	CQuestBox(_dev pDevice, _context pContext);
	CQuestBox(const CQuestBox& rhs);
	virtual ~CQuestBox() = default;

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
	_uint			m_iNum{};
	_uint			m_iOriginNum{};
	_bool			m_isMain{ false };
	_float			m_fMin{};
	_float			m_fMax{};
	_float2			m_vPosition;
	wstring			m_strQuestTitle;
	wstring			m_strText;

	CGameObject*	m_pArrow{ nullptr };

public:
	_bool Update_Quest();
	const wstring& Get_QuestName() const { return m_strQuestTitle; }
	const _vec2 Get_Position() const { return  _vec2(m_fX, m_fY); }
	void Set_Position(_vec2 vPos);
	_bool IsMain() { return m_isMain; }

private:
	HRESULT Add_Parts();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CQuestBox* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END