#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)

class CTower final : public COrthographicObject
{
public:
	typedef struct tagTowerDesc
	{
		_bool isBossRoom;
		_vec2* vDefaultPoint;
		_vec2  vPosition;
	}TOWER_DESC;
private:
	CTower(_dev pDevice, _context pContext);
	CTower(const CTower& rhs);
	virtual ~CTower() = default;

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
	_bool	m_isBossRoom{ false };
	_bool	m_isSelected{};
	_bool	m_isCleared{};

	_float	m_fTime{ 1.f };

	_vec2	m_vPosition{};
	_vec2	m_vInitialPosition{};
	_vec2*	m_vDefaultPoint{ nullptr };

	RECT	m_rcRect{};
	class CTextButtonColor* m_pSelectMask{ nullptr };
	CTextButtonColor* m_pDoor{ nullptr };

public:	
	const RECT& Get_Rect() const { return m_rcRect; }
	const _vec2 Get_Position();
	void Select_Object(_bool isSelect);

private:
	HRESULT Add_Parts();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CTower* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END