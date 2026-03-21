#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)
class CTextButtonColor;
class CTextButton;
class CSkill final : public COrthographicObject
{
public:
	typedef struct tagItemSlotDesc
	{
		_bool					isScreen{ false };
		_float					fDepth;
		_float2					vSize;
		_float2					vPosition;
		SKILLINFO				tSkillInfo;
	}SKILL_DESC;

private:
	CSkill(_dev pDevice, _context pContext);
	CSkill(const CSkill& rhs);
	virtual ~CSkill() = default;

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
	_bool					m_isScreen{ false };
	_bool					m_isUsingSkill{ false };
	_float					m_fCoolTime{};
	_float					m_fAlpha{};
	_float					m_fDir{1.f};

	SKILLINFO				m_tSkillInfo{};
	RECT					m_rcRect{};

	CTextButton*			m_pBackGround{ nullptr };
	CTextButtonColor*		m_pBorder{ nullptr };

public:
	const SKILLINFO& Get_SkillInfo() const { return m_tSkillInfo; }
	void Set_Position(_vec2 vPos);
	const RECT& Get_Rect() const { return m_rcRect; }
	_bool Use_Skill();
	_bool Is_Skill_Running() { return m_isUsingSkill; }

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSkill* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END