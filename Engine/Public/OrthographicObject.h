#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL COrthographicObject abstract : public CGameObject
{
protected:
	COrthographicObject(_dev pDevice, _context pContext);
	COrthographicObject(const COrthographicObject& rhs);
	virtual ~COrthographicObject() = default;

public:
	const _float& Get_Depth() const;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	void Apply_Orthographic(_uint iWinSizeX, _uint iWinSizeY);
	const _vec2 Convert_To_2D(class CTransform* pTransform) const;

protected:
	_float m_fSizeX{}, m_fSizeY{};
	_float m_fX{}, m_fY{};
	_float m_fDepth{};
	_float44 m_ViewMatrix{}, m_ProjMatrix{};

public:
	virtual CGameObject* Clone(void*) = 0;
	virtual void Free() override;
};

END