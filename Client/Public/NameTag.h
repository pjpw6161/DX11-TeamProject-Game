#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"


BEGIN(Client)
class CNameTag final : public COrthographicObject
{
public:
	typedef struct tagNameTagDesc
	{
		_float	fFontSize;
		_vec2	vTextPosition; //parent transform 기준으로 얼마나  옮길지
		_vec4	vColor{ _vec4(1.f, 1.f, 1.f, 1.f) };
		class CTransform* pParentTransform;
		
		wstring strNameTag;
		
		LEVEL_ID eLevelID;
	}NAMETAG_DESC;
private:
	CNameTag(_dev pDevice, _context pContext);
	CNameTag(const CNameTag& rhs);
	virtual ~CNameTag() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	_bool				m_isCoord{};
	LEVEL_ID			m_eLevel;

	_float				m_fFontSize{ 0.f };
	_vec2				m_vTextPosition;
	_vec4				m_vColor{ _vec4(1.f, 1.f, 1.f, 1.f) };
	_vec4				m_vPos{};

	CTransform*			m_pParentTransform{ nullptr };
						
	wstring				m_strNameTag{};

public:
	void Set_IsCoord(_bool isCoord) { m_isCoord = isCoord; }
	void Set_Transform(_vec4 vPos) { m_vPos = vPos; }
	void Set_Text(const wstring strText) { m_strNameTag = strText; }

public:
	static CNameTag* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END