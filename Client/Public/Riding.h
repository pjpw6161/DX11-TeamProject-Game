#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

enum Riding_Type
{
	Bird,
	Wyvern,
	Falar,
	Horse,
	Tiger,
	Nihilir,
	Type_End
};
struct Riding_Desc
{
	Riding_Type Type{ Type_End };
	_vec4 vSummonPos{};
	_bool bGlide{};
	_uint iMode{ 0 };
	_bool bLanding{};
};
enum Riding_State
{
	Riding_Landing,
	Riding_Idle,
	Riding_Run,
	Riding_Walk,
	Riding_Attack,
	Riding_Jump_Start,
	Riding_Jump,
	Riding_Jump_Run,
	Riding_Jump_End,
	Riding_Sky,
	Riding_Glide,
	Riding_End

};
enum Bird_Anim
{
	Bird_1005_Fly,
	Bird_1005_Fly_Add_L,
	Bird_1005_Fly_Add_R,
	Bird_1005_Fly_L,
	Bird_1005_Fly_R,
	Bird_1005_Landing,
	Bird_1005_Start,
	Bird_1005_Takeoff,
	Bird_2005_Fly,
	Bird_2005_Fly_Add_L,
	Bird_2005_Fly_Add_R,
	Bird_2005_Fly_L,
	Bird_2005_Fly_R,
	Bird_2005_Landing,
	Bird_2005_Start,
	Bird_2005_Takeoff
};
enum Wyvern_Anim
{
	Wyvern_3004_Fly,
	Wyvern_3004_Landing,
	Wyvern_3004_Start,
	Wyvern_3004_TakeOff
};
enum Falar_Anim
{
	Falar_5002_fly,
	Falar_5002_landing,
	Falar_5002_roar,
	Falar_5002_start,
	Falar_5002_TakeOff
};
enum Horse_Anim
{

	Horse_1004_Attack01,
	Horse_1004_Idle,
	Horse_1004_jump_End,
	Horse_1004_jump_End_Run,
	Horse_1004_jump_loop,
	Horse_1004_jump_start,
	Horse_1004_Run_Add_L,
	Horse_1004_Run_Add_R,
	Horse_1004_Run_F,
	Horse_1004_Run_F_start,
	Horse_1004_Run_L,
	Horse_1004_Run_R,
	Horse_1004_walk,
	Horse_2004_Attack01,
	Horse_2004_Idle,
	Horse_2004_jump_End,
	Horse_2004_jump_End_Run,
	Horse_2004_jump_loop,
	Horse_2004_jump_start,
	Horse_2004_Run_Add_L,
	Horse_2004_Run_Add_R,
	Horse_2004_Run_F,
	Horse_2004_Run_F_start,
	Horse_2004_Run_L,
	Horse_2004_Run_R,
	Horse_4004_Attack01,
	Horse_4004_Attack02,
	Horse_4004_Attack03,
	Horse_4004_Idle,
	Horse_4004_jump_End,
	Horse_4004_jump_End_Run,
	Horse_4004_jump_loop,
	Horse_4004_jump_start,
	Horse_4004_Run_Add_L,
	Horse_4004_Run_Add_R,
	Horse_4004_Run_F,
	Horse_4004_Run_F_start,
	Horse_4004_Run_L,
	Horse_4004_Run_R
};
enum Tiger_Anim
{
	Tiger_1003_Attack01,
	Tiger_1003_Idle,
	Tiger_1003_Jump_End,
	Tiger_1003_Jump_End_Run,
	Tiger_1003_Jump_loop,
	Tiger_1003_Jump_Start,
	Tiger_1003_Run,
	Tiger_1003_Run_Add_L,
	Tiger_1003_Run_Add_R,
	Tiger_1003_Run_L,
	Tiger_1003_Run_R,
	Tiger_1003_Start,
	Tiger_1003_Walk,
	Tiger_2003_Attack01,
	Tiger_2003_Idle,
	Tiger_2003_Jump_End,
	Tiger_2003_Jump_End_Run,
	Tiger_2003_Jump_loop,
	Tiger_2003_Jump_Start,
	Tiger_2003_Run,
	Tiger_2003_Run_Add_L,
	Tiger_2003_Run_Add_R,
	Tiger_2003_Run_L,
	Tiger_2003_Run_R,
	Tiger_2003_Start,
	Tiger_2003_Walk
};
enum Nihilir
{
	Nihilir_VC_Nihilir_5002_Attack01,
	Nihilir_VC_Nihilir_5002_Attack01_End,
	Nihilir_VC_Nihilir_5002_Attack01_Loop,
	Nihilir_VC_Nihilir_5002_Attack01_Start,
	Nihilir_VC_Nihilir_5002_Attack02,
	Nihilir_VC_Nihilir_5002_Attack03,
	Nihilir_VC_Nihilir_5002_Idle,
	Nihilir_VC_Nihilir_5002_Jump_End,
	Nihilir_VC_Nihilir_5002_Jump_End_Run,
	Nihilir_VC_Nihilir_5002_Jump_Loop,
	Nihilir_VC_Nihilir_5002_Jump_Start,
	Nihilir_VC_Nihilir_5002_Roar,
	Nihilir_VC_Nihilir_5002_Run,
	Nihilir_VC_Nihilir_5002_Run_add_L,
	Nihilir_VC_Nihilir_5002_Run_add_R,
	Nihilir_VC_Nihilir_5002_Run_L,
	Nihilir_VC_Nihilir_5002_Run_R,
	Nihilir_VC_Nihilir_5002_Walk
};

class CRiding final : public CGameObject
{
public:
	enum MODE { PLAYER, VEHICLEBOOK, MODE_END };

private:
	CRiding(_dev pDevice, _context pContext);
	CRiding(const CRiding& rhs);
	virtual ~CRiding() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() override;

public:
	_mat Get_Mat();
	_vec4 Get_Pos();
	void Init_State();
	void Delete_Riding();
	void Update_Collider();
	void Move(_float fTimeDelta);
	void Tick_State(_float fTimeDelta);
	_bool Get_Delete() { return m_bDelete; }
	Riding_State Get_State() { return m_eState; }
	Riding_Type Get_RidingType() { return m_CurrentIndex; }

private:
	CModel* m_pModelCom = { nullptr };
	CShader* m_pShaderCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CCollider* m_pBodyColliderCom = { nullptr };
	CCollider* m_pAttackColliderCom = { nullptr };
	class CCamera_Manager* m_pCam_Manager{ nullptr };
private:
	_bool m_bDelete{};
	_float m_fJumpPower{};
	_bool m_hasJumped{};
	_mat m_Worldmatrix{};
	Riding_State m_eState{ Riding_End };
	MODE	m_eCurMode{ PLAYER };
	ANIM_DESC m_Animation{};
	_float m_fDissolveRatio{};
	_float m_fRunSpeed{ 7.f };
	_float m_fWalkSpeed{ 3.f };
	Riding_State m_ePrevState{ Riding_End };
	wstring m_strPrototypeTag{};
	CTexture* m_pDissolveTextureCom{};
	Riding_Type m_CurrentIndex{ Type_End };

public:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CRiding* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END