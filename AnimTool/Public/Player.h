#pragma once
#include "AnimTool_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CAnimation;
END

BEGIN(AnimTool)

class CPlayer final : public CGameObject
{
public:
	enum TYPE { TYPE_MONSTER, TYPE_SELECT, TYPE_PLAYER, TYPE_END };

private:
	CPlayer(_dev pDevice, _context pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	void Set_ModelIndex(_uint iModelIndex) {
		m_iCurrentIndex = iModelIndex;
	}

	_uint Get_ModelIndex() {
		return m_iCurrentIndex;
	}

	void Set_ModelType(TYPE eType) {
		m_eType = eType;
	}

	TYPE Get_ModelType() {
		return m_eType;
	}

	CModel* Get_CurrentModel();
	CRealtimeVTFModel* Get_CurrentPlayerModel();

	CAnimation* Get_CurrentAnim();

public:
	virtual HRESULT Init_Prototype(_uint iNumMonsterModels, _uint iNumSelectModels);
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	HRESULT Place_PartModels();

private:
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CModel** m_pMonsterModelCom{ nullptr };
	CModel** m_pSelectModelCom{ nullptr };
	CModel* m_pModelCom{ nullptr };

	CShader* m_pPlayerShaderCom{ nullptr };
	CRealtimeVTFModel* m_pPlayerModelCom{ nullptr };

private:
	_float4 m_vPos{};
	_float m_fGravity{};
	_uint m_iNumMonsterModels = { 0 };
	_uint m_iNumSelectModels = { 0 };
	wstring* m_pMonsterModelTag = {};
	wstring* m_pSelectModelTag = {};
	_uint m_iCurrentIndex = { 0 };
	TYPE m_eType = { TYPE_END };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CPlayer* Create(_dev pDevice, _context pContext, _uint iNumMonsterModels, _uint iNumSelectModels);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END