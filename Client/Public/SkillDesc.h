#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)
class CItem;
class CSkillDesc final : public COrthographicObject
{
private:
	enum TYPE { LOCK, UNLOCK, TYPE_END};
public:
	typedef struct tagItemSlotDesc
	{
		_float					fDepth;
		_float2					vPosition;
		SKILLINFO				tSkillInfo;
	}SKILLBOOK_DESC;

private:
	CSkillDesc(_dev pDevice, _context pContext);
	CSkillDesc(const CSkillDesc& rhs);
	virtual ~CSkillDesc() = default;

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
	CTexture* m_pTextureCom[TYPE_END];


private:
	_bool					m_isPrototype{ false };
	_bool					m_bSelect{ false };
	_bool					m_bSkillIn{ false };

	TYPE					m_eCurType{ LOCK };
	wstring					m_strDetailTexture{};
	wstring					m_strTexture{};
	RECT					m_rcRect{};

	_float					m_fAlpha{};

	SKILLINFO				m_tSkillInfo{};
	CGameObject*			m_pSelect{ nullptr };
	class CTextButtonColor*	m_pDetail{ nullptr };

public:
	const SKILLINFO& Get_SkillInfo() const;
	void Set_Position(_vec2 vPos);
	const RECT& Get_Rect() const { return m_rcRect; }

	void Unlock_Skill();
	void Select_Skill(_bool isSelect);
	_bool Is_Selected() { return m_bSelect; }
	_bool Is_SkillIn() { return m_bSkillIn; }
	_bool Is_UnLocked() { return (_uint)m_eCurType; }

private:
	HRESULT Set_Type();
	HRESULT Add_Parts();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSkillDesc* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END