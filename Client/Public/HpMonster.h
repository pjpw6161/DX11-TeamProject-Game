#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CHPMonster final : public COrthographicObject
{
public:
	typedef struct tagNameTagDesc
	{
		_uint iMaxHp;
		_vec3	vPosition; //parent transform 기준으로 얼마나  옮길지
		class CTransform* pParentTransform;
		LEVEL_ID eLevelID;
	}HP_DESC;
private:
	CHPMonster(_dev pDevice, _context pContext);
	CHPMonster(const CHPMonster& rhs);
	virtual ~CHPMonster() = default;

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
	CTexture* m_pBGTex{ nullptr };

private:
	LEVEL_ID		m_eLevel{};

	_int			m_iCurHp{};
	_uint			m_iMaxHp{};

	_vec3			m_vPosition{};
	CTransform* m_pParentTransform{ nullptr };

public:
	void Set_HP(const _int iHp) { m_iCurHp = iHp; }
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CHPMonster* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END