#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"
#include "Riding.h"
#define BRICKCOL 6
#define BRICKROW 6

BEGIN(Client)
class CBrickGame final : public CGameObject
{
private:
	CBrickGame(_dev pDevice, _context pContext);
	CBrickGame(const CBrickGame& rhs);
	virtual ~CBrickGame() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer* m_pRendererCom{ nullptr };

private:
	_bool										m_isPicking{ false };
	_bool										m_isPrototype{ false };
	_bool										m_isActive{ false };

	static const _uint							m_iRow;
	static const _uint							m_iCol;
	_uint										m_iCombo{};
	_uint										m_iCurIndex{};
	_int										m_iSoundChannel = -1;

	_float										m_fComboTime{};

	class CBalloon*								m_pBalloon[BRICKROW][BRICKCOL]{};
	LIGHT_DESC									m_Light_Desc{};

private:
	HRESULT Add_Parts();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CBrickGame* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END