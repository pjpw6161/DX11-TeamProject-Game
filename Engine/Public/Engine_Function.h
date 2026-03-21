#pragma once

namespace Engine
{
	/**
	\brief 이넘 클래스를 인덱스로 쓸 수 있게 uint로 캐스팅 해주는 함수

	\param[in]  ENUM: 이넘 클래스의 원소를 넣습니다.
	\return unsigned int로 캐스팅된 값이 나옴.
	*/
	template<class T>
	inline constexpr unsigned int ToIndex(const T& ENUM)
	{
		return static_cast<unsigned int>(ENUM);
	}

	template<typename T>
	void Safe_Delete(T& Temp)
	{
		if (Temp)
		{
			delete Temp;
			Temp = nullptr;
		}
	}

	template<typename T>
	unsigned long Safe_AddRef(T& Temp)
	{
		unsigned long dwRefCnt = 0;

		if (nullptr != Temp)
		{
			dwRefCnt = Temp->AddRef();
		}

		return dwRefCnt;
	}

	template<typename T>
	unsigned long Safe_Release(T& Temp)
	{
		unsigned long dwRefCnt = 0;

		if (Temp)
		{
			dwRefCnt = Temp->Release();
			if (0 == dwRefCnt)
				Temp = nullptr;
		}

		return dwRefCnt;
	}

	template<typename T>
	void Safe_Delete_Array(T& Temp)
	{
		if (Temp)
		{
			delete[] Temp;
			Temp = nullptr;
		}
	}

	/**
	\brief 두개의 원소를 선형보간 합니다.
	\brief 당연히 float3같은건 안됨. 사실상 float, double용.
	\brief 벡터는 될거 같긴한데 굳이? XMVectorLerp를 이용 합시다.

	\param[in] A: 시작 값.
	\param[in] B: 끝 값.
	\param[in] Alpha: 0 ~ 1 사이의 값. 0에 가까울 수록 A에 가까운 값, 1에 가까울수록 B에 가까운 값이 나옵니다. Alpha 값은 0 에서 1 사이로 자동 클램프 됨.
	\return 선형 보간된 값.
	*/
	template<typename T>
	T Lerp(T A, T B, float Alpha)
	{
		if (Alpha < 0.f)
		{
			Alpha = 0.f;
		}
		if (Alpha > 1.f)
		{
			Alpha = 1.f;
		}
		return A * (1.f - Alpha) + B * Alpha;
	}

	/**
	\brief 0에서 1사이의 값으로 변환

	\param[in] Value: 해당 값이 0보다 작으면 0을 1보다 크면 1로 변환 됩니다.
	\return 변환된 값.
	*/
	template<typename T>
	T Saturate(T Value)
	{
		return clamp(Value, 0.f, 1.f);
		//if (Value < 0.f)
		//{
		//	Value = 0.f;
		//}
		//if (Value > 1.f)
		//{
		//	Value = 1.f;
		//}
		//return Value;
	}

	/**
	\brief XMVECTOR를 PxVec3로 변환합니다. w는 사라짐.

	\param[in]  vVector: 벡터를 넣으세요.
	\return  PxVec3로 변환된 값이 나옴.
	*/
	const PxVec3 ENGINE_DLL VectorToPxVec3(FXMVECTOR vVector);

	/**
	\brief PxExtendedVec3를 XMVECTOR로 변환합니다.

	\param[in]  Src: 변환하고 싶은 값을 넣으세요.
	\param[in]  w: w값을 넣고 싶으면 넣으세요. 기본인자 0.
	\return  XMVECTOR로 변환된 값이 나옴.
	*/
	const XMVECTOR ENGINE_DLL PxExVec3ToVector(PxExtendedVec3 Src, float w = 0.f);

	/**
	\brief PxExtendedVec3를 PxVec3로 변환합니다.

	\param[in]  Src: 변환하고 싶은 값을 넣으세요.
	\return  PxVec3로 변환된 값이 나옴.
	*/
	const PxVec3 ENGINE_DLL PxExVec3ToPxVec3(PxExtendedVec3 Src);

	/**
	\brief PxVec3를 XMVECTOR로 변환합니다.

	\param[in]  Src: 변환하고 싶은 값을 넣으세요.
	\param[in]  w: w값을 넣고 싶으면 넣으세요. 기본인자 0.
	\return  XMVECTOR로 변환된 값이 나옴.
	*/
	const XMVECTOR ENGINE_DLL PxVec3ToVector(PxVec3 Src, float w = 0.f);

	/**
	\brief PxVec3를 PxExtendedVec3로 변환합니다.

	\param[in]  Src: 변환하고 싶은 값을 넣으세요.
	\return  PxExtendedVec3로 변환된 값이 나옴.
	*/
	const PxExtendedVec3 ENGINE_DLL PxVec3ToPxExVec3(PxVec3 Src);

	/**
	\brief PxVec3를 float3로 변환합니다.

	\param[in]  Src: 변환하고 싶은 값을 넣으세요.
	\return  float3로 변환된 값이 나옴.
	*/
	const _float3 ENGINE_DLL PxVec3ToFloat3(PxVec3 Src);
}
