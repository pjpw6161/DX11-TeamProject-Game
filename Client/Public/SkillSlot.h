#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)
class CSkill;
class CSkillSlot final : public COrthographicObject
{
public:
	enum SLOTMODE { SCREEN, SKILLBOOK, SLOT_END };

	typedef struct tagSkillSlotDesc
	{
		SLOTMODE eSlotMode;
		_uint iSlotIdx;
		_float fDepth;
		_float2 vSize;
		_float2 vPosition;
	}SKILLSLOT_DESC;
private:
	CSkillSlot(_dev pDevice, _context pContext);
	CSkillSlot(const CSkillSlot& rhs);
	virtual ~CSkillSlot() = default;

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
	SLOTMODE		m_eSlotMode{ SLOT_END };

	wstring			m_strTexture;
	_uint			m_iSlotIdx{};
	_bool			m_isFull{};
	_float			m_fTime{};
	_float			m_fCoolTime{};
	CSkill*			m_pSkill{ nullptr };
	RECT			m_rcRect;

public:
	const _bool& Is_Full() const { return m_isFull; }
	HRESULT Set_Skill(SKILLINFO tSkillInfo);


	void Delete_Skill();
	//void Set_FullSlot(CItem* pItem, _int* iNum = nullptr);
	const CSkill* Get_SkillOject() { return m_pSkill; }
	SKILLINFO Get_SkillInfo();
	const RECT& Get_Rect() const { return m_rcRect; }
	const _vec2& Get_Position()const { return _vec2(m_fX, m_fY); }
	const _vec2& Get_Size() const { return _vec2(m_fSizeX, m_fSizeY); }
	_bool Use_Skill();

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSkillSlot* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END