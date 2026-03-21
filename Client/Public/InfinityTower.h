#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"

BEGIN(Client)
enum TOWER { BRICK, MINI2, BOSS1, MINI3, BOSS2, TOWER_END };

class CInfinityTower final : public COrthographicObject
{

private:
	CInfinityTower(_dev pDevice, _context pContext);
	CInfinityTower(const CInfinityTower& rhs);
	virtual ~CInfinityTower() = default;

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
	TOWER						m_iCurIndex{};

	_bool						m_isActive{};
	_bool						m_isPlaying{};
	_bool						m_isPrototype{};
	_bool						m_bLightening[3];

	_float						m_fDir{ 1.f };
	_float						m_fTime{ 0.f };
	_float						m_fAlpha{ 0.f };

	_vec2						m_vInitialPoint{};
	_vec2						m_vDefaultPoint{};

	CGameObject*				m_pTitle{nullptr};
	CGameObject*				m_pCloud{nullptr};
	CGameObject*				m_pCloudBottom{nullptr};
	class CTextButton*			m_pExitButton{nullptr};
	CTextButton*				m_pStartButton{nullptr};

	class CTextButtonColor*		m_pRoundEffect{ nullptr };
	CTextButtonColor*			m_pLeftBg{nullptr};
	CTextButtonColor*			m_pLightening[3]{};

	class CTower*				m_pTowers[TOWER_END]{};

private:
	HRESULT Ready_Tower();
	HRESULT Add_Parts();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	void Set_Active(_bool isActive);

private:
	void Effect_Tick(_float fTimeDelta);
	void Tower_Tick(_float fTimeDelta, POINT& ptMouse);
	void Exit_Tower();

public:
	static CInfinityTower* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END