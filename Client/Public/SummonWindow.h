#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"
#include "Player.h"
BEGIN(Client)

class CSummonWindow final : public COrthographicObject
{
public:
	typedef struct tagSummonWindowDesc
	{
		ITEM_TIER iItemTier;

	}SUMMON_DESC;
private:
	CSummonWindow(_dev pDevice, _context pContext);
	CSummonWindow(const CSummonWindow& rhs);
	virtual ~CSummonWindow() = default;

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
	Riding_Type		m_eRidingType{};
	_bool			m_bSetResult{ false };
	_bool			m_bStartShake{ false };
	_bool			m_bStartEffect{ false };

	_uint			m_iItemTier{};

	_float			m_fShakeAcc{3.f};
	_float			m_fShakeTime{};
	_float			m_fRotate{};
	_float			m_fSmokeSize{500.f};
	_float			m_fStop{};
	_float			m_fTime{};
	_float			m_fMoveY{};
	_float			m_fBright{6.f};
	_float			m_fBrightCard{6.f};

	
	class CTextButton*		m_pButton{ nullptr };

	class CTextButtonColor* m_pResultCard{ nullptr };
	CTextButtonColor*		m_pCard{ nullptr };
	CTextButtonColor*		m_pPattern{ nullptr };
	CTextButtonColor*		m_pSmoke{ nullptr };
	CTextButtonColor*		m_pResultEffect{ nullptr };
	CTextButtonColor*		m_pBackGround{ nullptr };
private:
	HRESULT Add_Parts();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSummonWindow* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END