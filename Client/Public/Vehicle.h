#pragma once
#include "Client_Define.h"
#include "OrthographicObject.h"
#include "Riding.h"

BEGIN(Client)
class CTextButtonColor;
class CTextButton;
class CNineSlice;
class CVehicle final : public COrthographicObject
{
public:
	typedef struct tagVehicleInfoDesc
	{
		VEHICLE_TYPE eType;
		Riding_Type	eRidingType;
		wstring strDescName;
	}VEHICLE_INFO;
	typedef struct tagVehicleDesc
	{
		Riding_Type				eRidingType;
		_float					fDepth;
		_float2					vSize;
		_float2					vPosition;
	}VEHICLE_DESC;

private:
	CVehicle(_dev pDevice, _context pContext);
	CVehicle(const CVehicle& rhs);
	virtual ~CVehicle() = default;

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
	Riding_Type				m_eRidingType;
	Riding_Desc				m_tRidingDesc{};
	_bool					m_isSelected{ false };
	_bool					m_isEquip{ false };

	_float					m_fAlpha{};
	_float					m_fDir{1.f};

	VEHICLE_INFO			m_tVehicleInfo{};
	RECT					m_rcRect{};

	wstring					m_strTexture{};

	CTextButton*			m_pSelected{ nullptr };
	CNineSlice*				m_pEquip{ nullptr };
	CTextButtonColor*		m_pDetail{ nullptr };
	CTextButtonColor*		m_pEquipButton{ nullptr };

	class CRiding*			m_pRiding{ nullptr };

public:
	const VEHICLE_INFO& Get_VehicleInfo() const { return m_tVehicleInfo; }
	void Set_Position(_vec2 vPos);
	const RECT& Get_Rect() const { return m_rcRect; }
	void Set_SelectVehicle(_bool isSelect);
	void Set_Equip(_bool isEquip) { m_isEquip = isEquip; }
	const _bool& Is_Selected () const { return m_isSelected; }
	const _bool& Is_Equiped () const { return m_isEquip; }

private:
	HRESULT Init_Info();
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CVehicle* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END