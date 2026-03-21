#include "Bone.h"

CBone::CBone()
{
}

const _char* CBone::Get_BoneName() const
{
	return m_szName;
}

const _mat* CBone::Get_CombinedMatrix() const
{
	return &m_CombindTransformationMatrix;
}

const _mat& CBone::Get_Transformation() const
{
	return m_TransformationMatrix;
}

const _mat& CBone::Get_OffsetMatrix() const
{
	return m_OffsetMatrix;
}

void CBone::Set_OffsetMatrix(_mat OffsetMatrix)
{
	m_OffsetMatrix = OffsetMatrix;
}

void CBone::Set_Transformation(_mat TransformationMatrix)
{
	m_TransformationMatrix = TransformationMatrix;
}

HRESULT CBone::Init(ifstream& ModelFile)
{
	_uint iNameSize{};
	ModelFile.read(reinterpret_cast<char*>(&iNameSize), sizeof _uint);
	if (iNameSize >= sizeof(m_szName))
	{
		MSG_BOX("Name Is Too Long!");
	}
	ModelFile.read(m_szName, iNameSize);
	ModelFile.read(reinterpret_cast<char*>(&m_TransformationMatrix), sizeof _mat);
	ModelFile.read(reinterpret_cast<char*>(&m_iParentIndex), sizeof _int);

	return S_OK;
}

void CBone::Update_CombinedMatrix(const vector<CBone*>& Bones)
{
	if (m_iParentIndex < 0)
	{
		m_CombindTransformationMatrix = m_TransformationMatrix;
		return;
	}
	
	m_CombindTransformationMatrix = m_TransformationMatrix * *Bones[m_iParentIndex]->Get_CombinedMatrix();
}

CBone* CBone::Create(ifstream& ModelFile)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Init(ModelFile)))
	{
		MSG_BOX("Failed to Create : CBone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CBone* CBone::Clone()
{
	return new CBone(*this);
}

void CBone::Free()
{
}
