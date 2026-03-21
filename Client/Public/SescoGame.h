#pragma once

#include "GameObject.h"
#include "Client_Define.h"

BEGIN(Client)

class CSescoGame final : public CGameObject
{
private:
	CSescoGame(_dev pDevice, _context pContext);
	CSescoGame(const CSescoGame& rhs);
	virtual ~CSescoGame() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg = nullptr) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void Late_Tick(_float fTimeDelta) override;

private:
	_randNum m_RandomNumber;
	list<class CVTFMonster*> m_Monsters;
	vector<_vec3> m_SpawnPositions;
	map<_int, _vec3> m_Void19Positions;
	_float m_fMonsterSpawnTime{};
	
	_float m_fTimeLimit{ 1209999999.f };
	_uint m_iMonsterLimit{ 200 };

private:
	void Release_DeadObjects();

public:
	static CSescoGame* Create(_dev pDevice, _context pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END