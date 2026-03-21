#pragma once
#include "Client_Define.h"
#include "Objects.h"
#include "Trigger_Manager.h"

BEGIN(Client)

class CInteraction_NonAnim final : public CGameObject
{
private:
	CInteraction_NonAnim(_dev pDevice, _context pContext);
	CInteraction_NonAnim(const CInteraction_NonAnim& rhs);
	virtual ~CInteraction_NonAnim() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	HRESULT Render_Shadow();
	HRESULT Render_Instance();
	_bool Get_Collision() { return m_isCollision; }

private:
	PlaceType m_ePlaceType{ PLACE_END };
	ObjectInfo m_Info{};
	_uint m_iShaderPass{0};

	_float m_fCollectTime{};
	_bool m_isCollision{ false };
	_bool m_isWideCollision{ false };
	_bool m_isCollect{};

private:
	CTrigger_Manager* m_pTrigger_Manager{ nullptr };
	CRenderer* m_pRendererCom{ nullptr };
	CShader* m_pShaderCom{ nullptr };
	CModel* m_pModelCom{ nullptr };
	CCollider* m_pColliderCom{ nullptr };
	CCollider* m_pWideColliderCom{ nullptr };
	CGameObject* m_pNameTag{ nullptr };
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CInteraction_NonAnim* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END