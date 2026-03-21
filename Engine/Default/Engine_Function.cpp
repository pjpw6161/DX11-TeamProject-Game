#include "Engine_Defines.h"

const PxVec3 Engine::VectorToPxVec3(FXMVECTOR vVector)
{
	return PxVec3(vVector.m128_f32[0], vVector.m128_f32[1], vVector.m128_f32[2]);
}

const XMVECTOR Engine::PxExVec3ToVector(PxExtendedVec3 Src, float w)
{
	return XMVectorSet(static_cast<float>(Src.x), static_cast<float>(Src.y), static_cast<float>(Src.z), w);
}

const PxVec3 Engine::PxExVec3ToPxVec3(PxExtendedVec3 Src)
{
	return PxVec3(static_cast<float>(Src.x), static_cast<float>(Src.y), static_cast<float>(Src.z));
}

const XMVECTOR Engine::PxVec3ToVector(PxVec3 Src, float w)
{
	return XMVectorSet(Src.x, Src.y, Src.z, w);
}

const PxExtendedVec3 Engine::PxVec3ToPxExVec3(PxVec3 Src)
{
	return PxExtendedVec3(static_cast<double>(Src.x), static_cast<double>(Src.y), static_cast<double>(Src.z));
}

const _float3 Engine::PxVec3ToFloat3(PxVec3 Src)
{
	return _float3(Src.x, Src.y, Src.z);
}
