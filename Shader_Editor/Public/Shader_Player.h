#pragma once
#include "GameObject.h"
#include "Shader_Define.h"

BEGIN(ShaderEdit)

struct BODYPART_DESC
{
	PART_TYPE eType{};
	_uint iNumVariations{};
	ANIM_DESC* Animation{};
	CTransform* pParentTransform{ nullptr };
};


class CShader_Player final :
    public CGameObject
{
private:
	CShader_Player(_dev pDevice, _context pContext);
	CShader_Player(const CShader_Player& rhs);
	virtual ~CShader_Player() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Place_PartModels();
	HRESULT Add_Info();
	HRESULT Render_Parts(PART_TYPE Parts, _uint Index);

public:
	void Change_Parts(PART_TYPE PartsType, _int ChangeIndex);

private:
	CRealtimeVTFModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom{ nullptr };

	CRenderer* m_pRendererCom{ nullptr };

private:
	ANIM_DESC m_Animation{};
	_int m_Body_CurrentIndex{ -1 };
	_int m_Hair_CurrentIndex{ -1 };
	_int m_Face_CurrentIndex{ -1 };

	_mat m_OldWorldMatrix{};

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CShader_Player* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END