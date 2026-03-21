#pragma once
#include "MapEditor_Define.h"
#include "GameObject.h"
#include "CutScene_Point.h"
#include "ImGui_Manager.h"


BEGIN(MapEditor)

class CCutScene_Curve final : public CGameObject
{
public:
	enum SECTIONTYPE
	{
		SECTION_TYPE_EYE
		, SECTION_TYPE_AT
		, SECTION_TYPE_END
	};


private:
	CCutScene_Curve(_dev pDevice, _context pContext);
	CCutScene_Curve(const CCutScene_Curve& rhs);
	virtual ~CCutScene_Curve() = default;
public:
	virtual HRESULT Init_Prototype();
	virtual HRESULT Init(void* pArg);
	virtual void Tick(_float TimeDelta) override;
	virtual void Late_Tick(_float TimeDelta) override;
	virtual HRESULT Render() override;
	HRESULT Set_ControlPoints(_mat& Points);
	void	Set_Points(SectionInfo Info);
	void	Set_SectionSpeed(_float fSpeed);

	void Get_ControlPoints(_mat** ppOutPoints);
	string Get_SectionName();
	_uint Get_CurveSize();
	_vec4	Get_CurvePos(_uint iIndex);
	_float	Get_SectionSpeed();

	//void Set_Dead();

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	CRenderer* m_pRendererCom{};
	CShader* m_pShaderCom{};
	CModel* m_pModelCom {};
	CVIBuffer_Curve* m_pVIBuffer{};

	_uint m_iShaderPass{};
	_uint m_iOutLineShaderPass{};
	SECTIONTYPE m_iSectionType = { SECTION_TYPE_END };

private:

	CCutScene_Point* m_pStartPoint{};
	CCutScene_Point* m_pEndPoint{};

	_vec4 m_vStartPoint{};
	_vec4 m_vEndPoint{};

	SectionInfo Info{};
	_mat m_matPoint{};
	wstring m_strSectionName{};
	_float	m_fSectionSpeed = { 0.f };

public:
	static CCutScene_Curve* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
END