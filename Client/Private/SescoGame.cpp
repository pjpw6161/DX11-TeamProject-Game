#include "SescoGame.h"
#include "VTFMonster.h"

CSescoGame::CSescoGame(_dev pDevice, _context pContext)
	:CGameObject(pDevice, pContext)
{
}

CSescoGame::CSescoGame(const CSescoGame& rhs)
	:CGameObject(rhs)
{
}

HRESULT CSescoGame::Init_Prototype()
{
	return S_OK;
}

HRESULT CSescoGame::Init(void* pArg)
{
	//UP
	m_SpawnPositions.push_back(_vec3(-3000.f, 1.f, 30.f));
	//DOWN
	m_SpawnPositions.push_back(_vec3(-3000.f, 1.f, -30.f));
	//RIGHT
	m_SpawnPositions.push_back(_vec3(-2970.f, 1.f, 0.f));
	//LEFT
	m_SpawnPositions.push_back(_vec3(-3030.f, 1.f, 0.f));

	random_device rand;
	m_RandomNumber = _randNum(rand());

	return S_OK;
}

void CSescoGame::Tick(_float fTimeDelta)
{
	if (m_pGameInstance->Key_Down(DIK_9))
	{
		m_fTimeLimit = 0.f;
	}

	m_fTimeLimit -= fTimeDelta;

	if (m_fTimeLimit <= -1.f)
	{
		Kill();
		return;
	}

#pragma region SpawnVoid19

	if (m_fMonsterSpawnTime == 0.f)
	{
		CVTFMonster::VTFMONSTER_DESC VTFMonsterDesc{};
		VTFMonsterDesc.strModelTag = TEXT("Prototype_Model_VTFMonster_Void19");

		_int iNumSpawn{};
		_randInt RandomDir(0, 3);
		_randInt RandomPos(5, 20);
		_randInt RandomSymbol(0, 1);

		while (iNumSpawn <= 5)
		{
			_int iSymbol = RandomSymbol(m_RandomNumber);
			if (iSymbol == 0)
			{
				iSymbol = -1;
			}

			_vec3 vPos{};

			_int iRandom = RandomDir(m_RandomNumber);
			if (iRandom <= 1)
			{
				vPos = m_SpawnPositions[iRandom];
				vPos.x += RandomPos(m_RandomNumber) * iSymbol;
				for (auto& Pair : m_Void19Positions)
				{
					if (Pair.second == vPos)
					{
						continue;
					}
				}
			}
			else
			{
				vPos = m_SpawnPositions[iRandom];
				vPos.z += RandomPos(m_RandomNumber) * iSymbol;
				for (auto& Pair : m_Void19Positions)
				{
					if (Pair.second == vPos)
					{
						continue;
					}
				}
			}

			VTFMonsterDesc.vPosition = vPos;
			CVTFMonster* pMonster = reinterpret_cast<CVTFMonster*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Void19_Object"), &VTFMonsterDesc));
			m_Monsters.push_back(pMonster);
			m_Void19Positions.emplace(pMonster->Get_ID(), vPos);
			iNumSpawn++;
		}
	}

#pragma endregion

	m_fMonsterSpawnTime += fTimeDelta;

	for (auto& pMonster : m_Monsters)
	{
		pMonster->Tick(fTimeDelta);
	}
	//실패 조건
	if (m_Monsters.size() > m_iMonsterLimit || m_fTimeLimit <= 0.f)
	{
		for (auto& pMonster : m_Monsters)
		{
			pMonster->Set_Damage(pMonster->Get_HP());
		}
	}

	Release_DeadObjects();
}

void CSescoGame::Late_Tick(_float fTimeDelta)
{
	for (auto& pMonster : m_Monsters)
	{
		pMonster->Late_Tick(fTimeDelta);
	}
}

void CSescoGame::Release_DeadObjects()
{
	for (auto& it = m_Monsters.begin(); it != m_Monsters.end();)
	{
		if ((*it)->isDead())
		{
			if ((*it)->Get_ModelTag() == TEXT("Prototype_Model_VTFMonster_Void19"))
			{
				m_Void19Positions.erase((*it)->Get_ID());
			}
			Safe_Release(*it);
			it = m_Monsters.erase(it);
		}
		else
		{
			++it;
		}
	}
}

CSescoGame* CSescoGame::Create(_dev pDevice, _context pContext)
{
	CSescoGame* pInstance = new CSescoGame(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CSescoGame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSescoGame::Clone(void* pArg)
{
	CSescoGame* pInstance = new CSescoGame(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Clone : CSescoGame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSescoGame::Free()
{
	__super::Free();

	for (auto& pMonster : m_Monsters)
	{
		Safe_Release(pMonster);
	}
	m_Monsters.clear();

	m_Void19Positions.clear();
}
