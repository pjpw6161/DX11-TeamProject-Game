#include "Renderer.h"
#include "BlendObject.h"
#include "OrthographicObject.h"
#include "VIBuffer_Rect.h"
#include "Shader.h"
#include "Compute_Shader.h"
#include "Compute_RenderTarget.h"
#include "Texture.h"

// 그림자를 위해서..
#include "ShadowMap.h"

// 물을 위해서...
#include "Transform.h"

CRenderer::CRenderer(_dev pDevice, _context pContext)
	: CComponent(pDevice, pContext)
{
}

HRESULT CRenderer::Init_Prototype()
{
	_uint iNumViewPorts{ 1 };

	D3D11_VIEWPORT ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewPorts, &ViewportDesc);

	m_WinSize.x = static_cast<_uint>(ViewportDesc.Width);
	m_WinSize.y = static_cast<_uint>(ViewportDesc.Height);

#pragma region For_MRT_GameObject

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f), true)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal_Spec"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth_Velocity"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 0.f, 0.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Rim"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		return E_FAIL;
	}
	//

#pragma endregion

#pragma region For_MRT_Refraction

	/*if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Object_Refraction_Diffuse"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Object_Refraction_Normal"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Object_Refraction_Depth"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 0.f, 1.f))))
	{
		return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Refraction_Shade"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Refraction_Specular"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Refraction_Final"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		return E_FAIL;
	}*/

#pragma endregion

#pragma region For_MRT_Reflection

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Object_Reflection_Diffuse"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R8G8B8A8_UNORM, _float4(1.f, 1.f, 1.f, 0.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Object_Reflection_Normal_Spec"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Object_Reflection_Depth_Velocity"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 0.f, 1.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Reflection_Shade"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Reflection_Specular"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Reflection_Emissive"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Reflection_Final"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		return E_FAIL;
	}

#pragma endregion

#pragma region For_MRT_Water

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Water_Depth"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.f, 1.f, 0.f, 1.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Water"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		return E_FAIL;
	}

#pragma endregion


#pragma region For_MRT_Light

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		return E_FAIL;
	}

	if(FAILED(m_pGameInstance->Add_RenderTarget(L"Target_Emissive", static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

#pragma endregion


	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Bloom"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BlurTest"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		return E_FAIL;
	}

#pragma region For_MRT_SSAO

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_SSAOTEST"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_SSAOBlur"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		return E_FAIL;
	}

#pragma endregion

#pragma region For_Effect

	if (FAILED(m_pGameInstance->Add_RenderTarget(L"Target_Effect", static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(L"Target_Effect_Alpha", static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(1.f, 1.f, 1.f, 1.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(L"Target_Effect_Blur", static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_RenderTarget(L"Target_Distortion", static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	//	return E_FAIL;

#pragma endregion

#pragma region For_MRT_Deferred

	if (FAILED(m_pGameInstance->Add_RenderTarget(L"Target_Deferrd", static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

#pragma endregion

#pragma region For_MRT_HDR

	if (FAILED(m_pGameInstance->Add_RenderTarget(L"Target_HDR", static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_RenderTarget(L"Target_HDR_Sky", static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

#pragma endregion


#pragma region For_Outline

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Outline"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
	{
		return E_FAIL;
	}

#pragma endregion

#pragma region For_MRT_MotionBlur

	if (FAILED(m_pGameInstance->Add_RenderTarget(L"Target_MotionBlur", static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

#pragma endregion
	

#pragma region For_Distortion

	if (FAILED(m_pGameInstance->Add_RenderTarget(L"Target_Distortion", static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

#pragma endregion



#pragma region MRT_GameObjects

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal_Spec"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth_Velocity"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Rim"))))
	{
		return E_FAIL;
	}

#pragma endregion

#pragma region MRT_Refraction

	//if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Refraction"), TEXT("Target_Object_Refraction_Diffuse"))))
	//{
	//	return E_FAIL;
	//}

	//if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Refraction"), TEXT("Target_Object_Refraction_Normal"))))
	//{
	//	return E_FAIL;
	//}

	//if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Refraction"), TEXT("Target_Object_Refraction_Depth"))))
	//{
	//	return E_FAIL;
	//}

	//if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Refraction"), TEXT("Target_Object_Refraction_Specular"))))
	//{
	//	return E_FAIL;
	//}

#pragma endregion

#pragma region MRT_Reflection

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Reflection"), TEXT("Target_Object_Reflection_Diffuse"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Reflection"), TEXT("Target_Object_Reflection_Normal_Spec"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Reflection"), TEXT("Target_Object_Reflection_Depth_Velocity"))))
	{
		return E_FAIL;
	}

#pragma endregion

#pragma region MRT_Water

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Water"), TEXT("Target_Water_Depth"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Water"), TEXT("Target_Water"))))
	{
		return E_FAIL;
	}

#pragma endregion

#pragma region MRT_Lights

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Lights"), TEXT("Target_Shade"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Lights"), TEXT("Target_Specular"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Lights", L"Target_Emissive")))
		return E_FAIL;	

#pragma endregion

#pragma region MRT_Refraction_Light

	//if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Refraction_Light", L"Target_Refraction_Shade")))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Refraction_Light", L"Target_Refraction_Specular")))
	//	return E_FAIL;

#pragma endregion

#pragma region MRT_Reflection_Light

	if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Reflection_Light", L"Target_Reflection_Shade")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Reflection_Light", L"Target_Reflection_Specular")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Reflection_Light", L"Target_Reflection_Emissive")))
		return E_FAIL;

#pragma endregion

#pragma region MRT_Refraction_Final

	//if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Refraction_Final", L"Target_Refraction_Final")))
	//	return E_FAIL;

#pragma endregion

#pragma region MRT_Reflection_Final

	if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Reflection_Final", L"Target_Reflection_Final")))
		return E_FAIL;

#pragma endregion


	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Blur"), TEXT("Target_Bloom"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BlurTest"), TEXT("Target_BlurTest"))))
	{
		return E_FAIL;
	}

#pragma region MRT_SSAO

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_SSAOTEST"), TEXT("Target_SSAOTEST"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_SSAOBlur"), TEXT("Target_SSAOBlur"))))
	{
		return E_FAIL;
	}

	m_pNoiseNormal = CTexture::Create(m_pDevice, m_pContext, L"../../Reference/NoiseNormal/Noise_Normal.jpg");

#pragma endregion

#pragma region MRT_Deferred

	if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Deferred", L"Target_Deferrd")))
		return E_FAIL;

#pragma endregion

#pragma region MRT_HDR

	if (FAILED(m_pGameInstance->Add_MRT(L"MRT_HDR", L"Target_HDR")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(L"MRT_HDR_Sky", L"Target_HDR_Sky")))
		return E_FAIL;

#pragma endregion


#pragma region MRT_Outline

	if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Outline", L"Target_Outline")))
		return E_FAIL;

#pragma endregion

#pragma region MRT_Effect

	if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Effect", L"Target_Effect")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Effect", L"Target_Effect_Alpha")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Effect", L"Target_Effect_Blur")))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Effect", L"Target_Distortion")))
	//	return E_FAIL;

#pragma endregion

#pragma region MRT_Distortion

	if (FAILED(m_pGameInstance->Add_MRT(L"MRT_Distortion", L"Target_Distortion")))
		return E_FAIL;

#pragma endregion

#pragma region MRT_MotionBlur

	if (FAILED(m_pGameInstance->Add_MRT(L"MRT_MotionBlur", L"Target_MotionBlur")))
		return E_FAIL;

#pragma endregion

#pragma region VIBuffer & Shader & Shadow Ready


	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (not m_pVIBuffer)
	{
		return E_FAIL;
	}

	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (not m_pShader)
	{
		return E_FAIL;
	}


	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	m_WorldMatrix._11 = ViewportDesc.Width;
	m_WorldMatrix._22 = ViewportDesc.Height;
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));

	m_pShadowMap = CShadowMap::Create(m_pDevice, m_pContext, m_WinSize, _float4(0.f, 0.f, 0.f, 0.f));

#pragma endregion


	//if (FAILED(Ready_ShadowDSV()))
	//{
	//	return E_FAIL;
	//}

#ifdef _DEBUG
	if (FAILED(m_pGameInstance->Ready_Debug_RT(TEXT("Target_Diffuse"), _float2(50.f, 50.f), _float2(100.f, 100.f))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Ready_Debug_RT(TEXT("Target_Normal_Spec"), _float2(50.f, 150.f), _float2(100.f, 100.f))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Ready_Debug_RT(TEXT("Target_Depth_Velocity"), _float2(50.f, 250.f), _float2(100.f, 100.f))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Ready_Debug_RT(TEXT("Target_Rim"), _float2(50.f, 350.f), _float2(100.f, 100.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Ready_Debug_RT(TEXT("Target_Shade"), _float2(150.f, 50.f), _float2(100.f, 100.f))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Ready_Debug_RT(TEXT("Target_Specular"), _float2(150.f, 150.f), _float2(100.f, 100.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Ready_Debug_RT(TEXT("Target_Effect"), _float2(ViewportDesc.Width - 50.f, 50.f), _float2(100.f, 100.f))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Ready_Debug_RT(TEXT("Target_Effect_Alpha"), _float2(ViewportDesc.Width - 50.f, 150.f), _float2(100.f, 100.f))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Ready_Debug_RT(TEXT("Target_Effect_Blur"), _float2(ViewportDesc.Width - 50.f, 250.f), _float2(100.f, 100.f))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Ready_Debug_RT(TEXT("Target_Deferrd"), _float2(ViewportDesc.Width - 150.f, 50.f), _float2(100.f, 100.f))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Ready_Debug_RT(TEXT("Target_SSAOBlur"), _float2(ViewportDesc.Width - 150.f, 150.f), _float2(100.f, 100.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Ready_Debug_RT(L"Target_Outline", _float2(ViewportDesc.Width - 250.f, 50.f), _float2(100.f, 100.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Ready_Debug_RT(L"Target_Reflection_Final", _float2(ViewportDesc.Width - 250.f, 150.f), _float2(100.f, 100.f))))
		return E_FAIL;

	if (FAILED(m_pShadowMap->Ready_Debug(_float2(ViewportDesc.Width - 350.f, 50.f), _float2(100.f, 100.f))))
		return E_FAIL;


#endif // _DEBUG

	if (FAILED(m_pDevice->CreateDeferredContext(0, &m_pDeferrd)))
		return E_FAIL;

#pragma region 평균 휘도값 구하기

	m_pLumShader = CCompute_Shader::Create(m_pDevice, m_pDeferrd, L"../Bin/ShaderFiles/Shader_AvgLum.hlsl", "DownScaleFirst", 0);

	m_pDownShader = CCompute_Shader::Create(m_pDevice, m_pDeferrd, L"../Bin/ShaderFiles/Shader_AvgLum.hlsl", "DownScaleSecond", 0);

	m_pDown2Shader = CCompute_Shader::Create(m_pDevice, m_pDeferrd, L"../Bin/ShaderFiles/Shader_AvgLum.hlsl", "DownScaleThird", 0);

	m_pDown3Shader = CCompute_Shader::Create(m_pDevice, m_pDeferrd, L"../Bin/ShaderFiles/Shader_AvgLum.hlsl", "DownScaleFourth", 0);

	m_pGetAvgLumShader = CCompute_Shader::Create(m_pDevice, m_pDeferrd, L"../Bin/ShaderFiles/Shader_AvgLum.hlsl", "LastDownScaled", 0);

	_uint2 iSize = m_WinSize;

	iSize = _uint2(iSize.x / 4, iSize.y / 3);

	m_pLumRT = CCompute_RenderTarget::Create(m_pDevice, m_pDeferrd, iSize, DXGI_FORMAT_R16_UNORM);

	iSize = _uint2(iSize.x / 4, iSize.y / 4);

	m_pDownRT = CCompute_RenderTarget::Create(m_pDevice, m_pDeferrd, iSize, DXGI_FORMAT_R16_UNORM);

	iSize = _uint2(iSize.x / 4, iSize.y / 3);

	m_pDownRT1 = CCompute_RenderTarget::Create(m_pDevice, m_pDeferrd, iSize, DXGI_FORMAT_R16_UNORM);

	iSize = _uint2(iSize.x / 5, iSize.y / 5);

	m_pDownRT2 = CCompute_RenderTarget::Create(m_pDevice, m_pDeferrd, iSize, DXGI_FORMAT_R16_UNORM);

	iSize = _uint2(1, 1);

	m_pLumValue = CCompute_RenderTarget::Create(m_pDevice, m_pDeferrd, iSize, DXGI_FORMAT_R16_UNORM);
#pragma endregion

#pragma region 블러만들기

	m_BLParam.radius = GAUSSIAN_RADIUS;
	m_BLParam.direction = 0;

	m_pBlurShader = CCompute_Shader::Create(m_pDevice, m_pDeferrd, L"../Bin/ShaderFiles/Shader_Blur.hlsl", "Blur", sizeof(BLURPARAM));

	m_pDownScaleShader = CCompute_Shader::Create(m_pDevice, m_pDeferrd, L"../Bin/ShaderFiles/Shader_DownScale.hlsl", "Downsample", sizeof(DSPARAM));

	iSize = _uint2(m_WinSize.x / 2, m_WinSize.y / 2);
	m_DSParam[0].ScaleX = iSize.x;
	m_DSParam[0].ScaleY = iSize.y;

	m_pHalfRT = CCompute_RenderTarget::Create(m_pDevice, m_pDeferrd, iSize, DXGI_FORMAT_R16G16B16A16_UNORM);
	m_pHalfBlurRT = CCompute_RenderTarget::Create(m_pDevice, m_pDeferrd, iSize, DXGI_FORMAT_R16G16B16A16_UNORM);

	iSize = _uint2(iSize.x / 2, iSize.y / 2);
	m_DSParam[1].ScaleX = iSize.x;
	m_DSParam[1].ScaleY = iSize.y;

	m_pQuarterRT = CCompute_RenderTarget::Create(m_pDevice, m_pDeferrd, iSize, DXGI_FORMAT_R16G16B16A16_UNORM);
	m_pQuarterBlurRT = CCompute_RenderTarget::Create(m_pDevice, m_pDeferrd, iSize, DXGI_FORMAT_R16G16B16A16_UNORM);

	iSize = _uint2(iSize.x / 2, iSize.y / 2);
	m_DSParam[2].ScaleX = iSize.x;
	m_DSParam[2].ScaleY = iSize.y;

	m_pEightRT = CCompute_RenderTarget::Create(m_pDevice, m_pDeferrd, iSize, DXGI_FORMAT_R16G16B16A16_UNORM);
	m_pEightBlurRT = CCompute_RenderTarget::Create(m_pDevice, m_pDeferrd, iSize, DXGI_FORMAT_R16G16B16A16_UNORM);

	m_pGetBlurShader = CShader::Create(m_pDevice, m_pContext, L"../Bin/ShaderFiles/Shader_BlurTex.hlsl", VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);

	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BlurTex"), static_cast<_uint>(ViewportDesc.Width), static_cast<_uint>(ViewportDesc.Height), DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_MRT(L"MRT_BLURTEX", L"Target_BlurTex")))
		return E_FAIL;

#pragma endregion

#pragma region 레디얼 블러

	m_pRadialShader = CCompute_Shader::Create(m_pDevice, m_pDeferrd, L"../Bin/ShaderFiles/Shader_Blur.hlsl", "Radial_Blur", sizeof(RadialParams));
	m_pRadialRT = CCompute_RenderTarget::Create(m_pDevice, m_pDeferrd, m_WinSize, DXGI_FORMAT_R16G16B16A16_UNORM);

#pragma endregion

#pragma region 모션 블러

	m_pMotionShader = CCompute_Shader::Create(m_pDevice, m_pDeferrd, L"../Bin/ShaderFiles/Shader_Blur.hlsl", "Motion_Blur");
	m_pMotionRT = CCompute_RenderTarget::Create(m_pDevice, m_pDeferrd, m_WinSize, DXGI_FORMAT_R16G16B16A16_UNORM);

#pragma endregion


	if (FAILED(FinishCommand()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Init(void* pArg)
{
	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RenderGroup eRenderGroup, CGameObject* pRenderObject)
{
	if (!pRenderObject)
	{
		MSG_BOX("pRenderObject is NULL");
		return E_FAIL;
	}

	m_RenderObjects[ToIndex(eRenderGroup)].push_back(pRenderObject);

	Safe_AddRef(pRenderObject);

	return S_OK;
}

HRESULT CRenderer::Draw_RenderGroup()
{
	if (FAILED(Clear_Instance()))
		return E_FAIL;

	//if (FAILED(Render_Priority()))
	//{
	//	MSG_BOX("Failed to Render : Priority");
	//	return E_FAIL;
	//}


	if (FAILED(Render_Shadow()))
	{
		MSG_BOX("Failed to Render : Shadow");
		return E_FAIL;
	}

	if (FAILED(Clear_Instance()))
		return E_FAIL;

	if (FAILED(Render_Reflection()))
	{
		MSG_BOX("Failed to Render : Reflection");
		return E_FAIL;
	}

	if (FAILED(Clear_Instance()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
	{
		return E_FAIL;
	}

	if (FAILED(Render_Water()))
	{
		MSG_BOX("Failed to Render : Water");
		return E_FAIL;
	}

	if (FAILED(Render_NonBlend()))
	{
		MSG_BOX("Failed to Render : NonBlend");
		return E_FAIL;
	}
	if (FAILED(Render_NonBlend_Instance()))
	{
		MSG_BOX("Failed to Render : NonBlend_Instance");
		return E_FAIL;
	}

	if (FAILED(Clear_Instance()))
		return E_FAIL;

	if (FAILED(Render_AnimNonBlend_Instance()))
	{
		MSG_BOX("Failed to Render : AnimNonBlend_Instance");
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	if (FAILED(Render_LightAcc()))
	{
		MSG_BOX("Failed to Render : LightAcc");
		return E_FAIL;
	}

	if (FAILED(Render_Deferred()))
	{
		MSG_BOX("Failed to Render : Deferred");
		return E_FAIL;
	}

	if (FAILED(Render_HDR()))
	{
		MSG_BOX("Failed to Render : HDR");
		return E_FAIL;
	}

	if (FAILED(Render_Outline()))
	{
		MSG_BOX("Failed to Render : Outline");
		return E_FAIL;
	}

	if (FAILED(Render_Blend()))
	{
		MSG_BOX("Failed to Render : Blend");
		return E_FAIL;
	}

	if (FAILED(Render_Distortion()))
	{
		MSG_BOX("Failed to Render : Distortion");
		return E_FAIL;
	}

	if (FAILED(Render_NoneBlendFinal()))
	{
		MSG_BOX("Failed to Render : NoneBlendFinal");
		return E_FAIL;
	}

	if (FAILED(Render_BlendFinal()))
	{
		MSG_BOX("Failed to Render : BlendFinal");
		return E_FAIL;
	}

	if (FAILED(Render_Final()))
	{
		MSG_BOX("Failed to Render : Final");
		return E_FAIL;
	}

	if (FAILED(Render_UI()))
	{
		MSG_BOX("Failed to Render : UI");
		return E_FAIL;
	}

#ifdef _DEBUG
	if (not m_pGameInstance->IsSkipDebugRendering())
	{
		if (FAILED(Render_Debug()))
		{
			MSG_BOX("Failed to Render Debug");
			return E_FAIL;
		}
	}
#endif // _DEBUG


	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Add_DebugComponent(CComponent* pDebugComponent)
{
	if (m_pGameInstance->IsSkipDebugRendering())
	{
		return S_OK;
	}

	m_DebugComponents.push_back(pDebugComponent);
	Safe_AddRef(pDebugComponent);

	return S_OK;
}
#endif // _DEBUG


HRESULT CRenderer::Ready_ShadowDSV()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	{
		ID3D11Texture2D* pDepthStencilTexture = nullptr;

		D3D11_TEXTURE2D_DESC	TextureDesc;
		ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

		TextureDesc.Width = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION;
		TextureDesc.Height = D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION / 2;
		TextureDesc.MipLevels = 1;
		TextureDesc.ArraySize = 1;
		TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		TextureDesc.SampleDesc.Quality = 0;
		TextureDesc.SampleDesc.Count = 1;

		TextureDesc.Usage = D3D11_USAGE_DEFAULT;
		TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL
			/*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
		TextureDesc.CPUAccessFlags = 0;
		TextureDesc.MiscFlags = 0;

		if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr,
			&pDepthStencilTexture)))
			return E_FAIL;

		/* RenderTarget */
		/* ShaderResource */
		/* DepthStencil */

		if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr,
			&m_pShadowDSV)))
			return E_FAIL;

		Safe_Release(pDepthStencilTexture);
	}

	{
		//ID3D11Texture2D* pDepthStencilTexture = nullptr;

		//D3D11_TEXTURE2D_DESC	TextureDesc;
		//ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

		//TextureDesc.Width = 1280;
		//TextureDesc.Height = 720;
		//TextureDesc.MipLevels = 1;
		//TextureDesc.ArraySize = 1;
		//TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		//TextureDesc.SampleDesc.Quality = 0;
		//TextureDesc.SampleDesc.Count = 1;

		//TextureDesc.Usage = D3D11_USAGE_DEFAULT;
		//TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL
		//	/*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
		//TextureDesc.CPUAccessFlags = 0;
		//TextureDesc.MiscFlags = 0;

		//if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr,
		//									  &pDepthStencilTexture)))
		//	return E_FAIL;

		///* RenderTarget */
		///* ShaderResource */
		///* DepthStencil */

		//if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr,
		//											 &m_pBlurDSV)))
		//	return E_FAIL;

		//Safe_Release(pDepthStencilTexture);
	}

	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderObjects[RG_Priority])
	{
		if (pGameObject)
		{
			if (FAILED(pGameObject->Render()))
			{
				MSG_BOX("Failed to Render");
				return E_FAIL;
			}
		}
		Safe_Release(pGameObject);
	}

	m_RenderObjects[RG_Priority].clear();
	return S_OK;
}

HRESULT CRenderer::Render_Shadow()
{
	//if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Shadow"), m_pShadowDSV)))
	//{
	//	return E_FAIL;
	//}

	//_uint iNumViewPorts{ 1 };

	//D3D11_VIEWPORT OldViewportDesc{};

	//m_pContext->RSGetViewports(&iNumViewPorts, &OldViewportDesc);

	//D3D11_VIEWPORT TempViewPortDesc{};
	//TempViewPortDesc.TopLeftX = 0;
	//TempViewPortDesc.TopLeftY = 0;
	//TempViewPortDesc.Width = static_cast<_float>(D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION);
	//TempViewPortDesc.Height = static_cast<_float>(D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION * 0.5f);
	//TempViewPortDesc.MinDepth = 0.f;
	//TempViewPortDesc.MaxDepth = 1.f;

	//m_pContext->RSSetViewports(iNumViewPorts, &TempViewPortDesc);



	if (FAILED(m_pShadowMap->Begin_ShadowMap()))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RG_Shadow])
	{
		if (true == m_pGameInstance->Get_TurnOnShadow()) {

			if (pGameObject)
			{
				if (FAILED(pGameObject->Render_Shadow()))
				{
					MSG_BOX("Failed to Render");
					return E_FAIL;
				}
			}
		}
		Safe_Release(pGameObject);
	}

	m_RenderObjects[RG_Shadow].clear();

	if (true == m_pGameInstance->Get_TurnOnShadow()) {


		map<InstanceID, vector<CGameObject*>> InstanceData;

		for (auto& pGameObject : m_RenderObjects[RG_NonBlend_Instance])
		{
			if (pGameObject->Find_Component(L"Com_Model") == nullptr)
				continue;

			const _int iPassIndex = static_cast<CShader*>(pGameObject->Find_Component(L"Com_Shader"))->Get_PassIndex();
			const _int iInstanceID = static_cast<CModel*>(pGameObject->Find_Component(L"Com_Model"))->Get_InstanceID();
			InstanceID ID(iPassIndex, iInstanceID);
			InstanceData[ID].push_back(pGameObject);
		}

		for (auto& Pair : InstanceData)
		{
			vector<CGameObject*>& vInstances = Pair.second;
			const InstanceID instanceId = Pair.first;
			CGameObject*& pHead = vInstances[0];

			for (_uint i = 0; i < vInstances.size(); i++)
			{
				CGameObject*& pGameObject = vInstances[i];
				Instance_Data MeshInstancing;
				CTransform* pTransform = static_cast<CTransform*>(pGameObject->Find_Component(L"Com_Transform"));
				//MeshInstancing.vRight = pTransform->Get_State(State::Right);
				//MeshInstancing.vUp = pTransform->Get_State(State::Up);
				//MeshInstancing.vLook = pTransform->Get_State(State::Look);
				//MeshInstancing.vPos = pTransform->Get_State(State::Pos);
				MeshInstancing.mMatrix = pTransform->Get_World_Matrix();
				MeshInstancing.m_iID = pGameObject->Get_ID();
				Add_Instance(instanceId, MeshInstancing);
			}

			for (auto& iter : vInstances)
			{
				iter->InitRendered();
			}
			CVIBuffer_Mesh_Instance*& pBuffer = m_InstanceBuffers[instanceId];
			pHead->Render_Instance();
			CModel* pModel = static_cast<CModel*>(pHead->Find_Component(L"Com_Model"));
			CShader* pShader = static_cast<CShader*>(pHead->Find_Component(L"Com_Shader"));
			pModel->Render_Shadow_Instancing(pBuffer, pShader);
		}

		InstanceData.clear();

		for (auto& pGameObject : m_RenderObjects[RG_AnimNonBlend_Instance])
		{
			if (pGameObject->Find_Component(L"Com_Model") == nullptr)
				continue;

			const _int iPassIndex = static_cast<CShader*>(pGameObject->Find_Component(L"Com_Shader"))->Get_PassIndex();
			const _int iInstanceID = static_cast<CVTFModel*>(pGameObject->Find_Component(L"Com_Model"))->Get_InstanceID();
			InstanceID ID(iPassIndex, iInstanceID);
			InstanceData[ID].push_back(pGameObject);
		}

		for (auto& Pair : InstanceData)
		{
			vector<CGameObject*>& vInstances = Pair.second;
			const InstanceID instanceId = Pair.first;;
			CGameObject*& pHead = vInstances[0];
			//Late_Tick 2번 들어와서 터지는거 방지
			if (vInstances.size() > MAX_INSTANCE)
				break;

			INSTANCED_PLAYANIM_DESC* PlayAnimDescs = new INSTANCED_PLAYANIM_DESC;

			for (_uint i = 0; i < vInstances.size(); i++)
			{
				CGameObject*& pGameObject = vInstances[i];
				Instance_Data MeshInstancing;
				CTransform* pTransform = static_cast<CTransform*>(pGameObject->Find_Component(L"Com_Transform"));
				MeshInstancing.mMatrix = pTransform->Get_World_Matrix();
				MeshInstancing.m_iID = i;
				Add_Instance(instanceId, MeshInstancing);

				CVTFModel* pModel = static_cast<CVTFModel*>(pGameObject->Find_Component(L"Com_Model"));
				PlayAnimDescs->PlayAnim[i] = pModel->Get_PlayAnimDesc();
			}

			CVTFModel* pHeadModel = static_cast<CVTFModel*>(pHead->Find_Component(L"Com_Model"));
			CShader* pHeadShader = static_cast<CShader*>(pHead->Find_Component(L"Com_Shader"));
			if (FAILED(pHeadShader->Bind_RawValue("g_PlayAnimInstances", PlayAnimDescs, MAX_INSTANCE * sizeof(PLAYANIM_DESC))))
				return E_FAIL;

			Safe_Delete(PlayAnimDescs);

			CVIBuffer_Mesh_Instance*& pBuffer = m_InstanceBuffers[instanceId];
			pHead->Render_Instance(); //셰이더 바인딩 리소스
			pHeadModel->Render_Shadow_Instancing(pBuffer, pHeadShader);
		}

	}
	if (FAILED(m_pShadowMap->End_ShadowMap()))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->End_MRT()))
	//{
	//	return E_FAIL;
	//}

	//m_pContext->RSSetViewports(iNumViewPorts, &OldViewportDesc);

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
	for (auto& pGameObject : m_RenderObjects[RG_NonBlend])
	{
		if (pGameObject)
		{
			if (FAILED(pGameObject->Render()))
			{
				MSG_BOX("Failed to Render");
			}
		}

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RG_NonBlend].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonBlend_Instance()
{
	map<InstanceID, vector<CGameObject*>> InstanceData;

	for (auto& pGameObject : m_RenderObjects[RG_NonBlend_Instance])
	{
		if (pGameObject->Find_Component(L"Com_Model") == nullptr)
			continue;

		const _int iPassIndex = static_cast<CShader*>(pGameObject->Find_Component(L"Com_Shader"))->Get_PassIndex();
		const _int iInstanceID = static_cast<CModel*>(pGameObject->Find_Component(L"Com_Model"))->Get_InstanceID();
		InstanceID ID(iPassIndex, iInstanceID);
		InstanceData[ID].push_back(pGameObject);
	}

	for (auto& Pair : InstanceData)
	{
		vector<CGameObject*>& vInstances = Pair.second;
		const InstanceID instanceId = Pair.first;
		CGameObject*& pHead = vInstances[0];

		for (_uint i = 0; i < vInstances.size(); i++)
		{
			CGameObject*& pGameObject = vInstances[i];
			Instance_Data MeshInstancing;
			CTransform* pTransform = static_cast<CTransform*>(pGameObject->Find_Component(L"Com_Transform"));
			//MeshInstancing.vRight = pTransform->Get_State(State::Right);
			//MeshInstancing.vUp = pTransform->Get_State(State::Up);
			//MeshInstancing.vLook = pTransform->Get_State(State::Look);
			//MeshInstancing.vPos = pTransform->Get_State(State::Pos);
			MeshInstancing.mMatrix = pTransform->Get_World_Matrix();
			MeshInstancing.m_iID = pGameObject->Get_ID();
			Add_Instance(instanceId, MeshInstancing);
		}

		for (auto& iter : vInstances)
		{
			iter->InitRendered();
		}
		CVIBuffer_Mesh_Instance*& pBuffer = m_InstanceBuffers[instanceId];
		pHead->Render_Instance();
		CModel* pModel = static_cast<CModel*>(pHead->Find_Component(L"Com_Model"));
		CShader* pShader = static_cast<CShader*>(pHead->Find_Component(L"Com_Shader"));
		pModel->Render_Instancing(pBuffer, pShader);
	}

	for (auto& pGameObject : m_RenderObjects[RG_NonBlend_Instance])
		Safe_Release(pGameObject);
	m_RenderObjects[RG_NonBlend_Instance].clear();

	return S_OK;
}

HRESULT CRenderer::Render_AnimNonBlend_Instance()
{
	map<InstanceID, vector<CGameObject*>> InstanceData;

	for (auto& pGameObject : m_RenderObjects[RG_AnimNonBlend_Instance])
	{
		if (pGameObject->Find_Component(L"Com_Model") == nullptr)
			continue;

		const _int iPassIndex = static_cast<CShader*>(pGameObject->Find_Component(L"Com_Shader"))->Get_PassIndex();
		const _int iInstanceID = static_cast<CVTFModel*>(pGameObject->Find_Component(L"Com_Model"))->Get_InstanceID();
		InstanceID ID(iPassIndex, iInstanceID);
		InstanceData[ID].push_back(pGameObject);
	}

	for (auto& Pair : InstanceData)
	{
		vector<CGameObject*>& vInstances = Pair.second;
		const InstanceID instanceId = Pair.first;;
		CGameObject*& pHead = vInstances[0];
		//Late_Tick 2번 들어와서 터지는거 방지
		if (vInstances.size() > MAX_INSTANCE)
			break;

		INSTANCED_PLAYANIM_DESC* PlayAnimDescs = new INSTANCED_PLAYANIM_DESC;

		for (_uint i = 0; i < vInstances.size(); i++)
		{
			CGameObject*& pGameObject = vInstances[i];
			Instance_Data MeshInstancing;
			CTransform* pTransform = static_cast<CTransform*>(pGameObject->Find_Component(L"Com_Transform"));
			MeshInstancing.mMatrix = pTransform->Get_World_Matrix();
			MeshInstancing.m_iID = i;
			Add_Instance(instanceId, MeshInstancing);

			CVTFModel* pModel = static_cast<CVTFModel*>(pGameObject->Find_Component(L"Com_Model"));
			PlayAnimDescs->PlayAnim[i] = pModel->Get_PlayAnimDesc();
		}

		CVTFModel* pHeadModel = static_cast<CVTFModel*>(pHead->Find_Component(L"Com_Model"));
		CShader* pHeadShader = static_cast<CShader*>(pHead->Find_Component(L"Com_Shader"));
		if (FAILED(pHeadShader->Bind_RawValue("g_PlayAnimInstances", PlayAnimDescs, MAX_INSTANCE * sizeof(PLAYANIM_DESC))))
			return E_FAIL;

		Safe_Delete(PlayAnimDescs);

		CVIBuffer_Mesh_Instance*& pBuffer = m_InstanceBuffers[instanceId];
		pHead->Render_Instance(); //셰이더 바인딩 리소스
		pHeadModel->Render_Instancing(pBuffer, pHeadShader);
	}

	for (auto& pGameObject : m_RenderObjects[RG_AnimNonBlend_Instance])
		Safe_Release(pGameObject);
	m_RenderObjects[RG_AnimNonBlend_Instance].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Refraction()
{
	/*if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Refraction"))))
		return E_FAIL;

	for (auto& pWater : m_RenderObjects[RG_Water]) {
		if (pWater) {
			CTransform* pWaterTransform = dynamic_cast<CTransform*>(pWater->Find_Component(L"Com_Transform"));
			_float fWaterHeight = pWaterTransform->Get_CenterPos().y;
			_float4 vClipPlane = _float4(0.f, 1.f, 0.f, fWaterHeight + 0.1f);
			for (auto& pGameObject : m_RenderObjects[RG_Water_Refraction]) {
				if (pGameObject) {
					if (FAILED(pGameObject->Render_Refract(vClipPlane)))
						return E_FAIL;
				}
			}
		}
	}

	for (auto& pGameObject : m_RenderObjects[RG_Water_Refraction])
		Safe_Release(pGameObject);
	m_RenderObjects[RG_Water_Refraction].clear();


	if (FAILED(m_pGameInstance->End_MRT()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Clear_DepthStencil_View()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_Refraction_Light")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Inversed(TransformType::View))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Inversed(TransformType::Proj))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CameraPos(), sizeof _float4)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_RawValue("g_vCamNF", &m_pGameInstance->Get_CameraNF(), sizeof _float2)))
	{
		return E_FAIL;
	}

	for (auto& pWater : m_RenderObjects[RG_Water]) {
		if (pWater) {

			if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_NormalTexture", L"Target_Object_Refraction_Normal")))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_DepthTexture", L"Target_Object_Refraction_Depth")))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_ObjectSpecTexture", L"Target_Object_Refraction_Specular")))
			{
				return E_FAIL;
			}

			if (FAILED(m_pGameInstance->Render_Lights(m_pGameInstance->Get_CurrentLevelIndex(), m_pShader, m_pVIBuffer)))
			{
				return E_FAIL;
			}
		}
	}

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_Refraction_Final")))
		return E_FAIL;

	for (auto& pWater : m_RenderObjects[RG_Water]) {
		if (pWater) {
			if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_DiffuseTexture", L"Target_Object_Refraction_Diffuse")))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_ShadeTexture", L"Target_Refraction_Shade")))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_SpecularTexture", L"Target_Refraction_Specular")))
				return E_FAIL;

			if (FAILED(m_pShader->Begin(DefPass_Water)))
				return E_FAIL;

			if (FAILED(m_pVIBuffer->Render()))
				return E_FAIL;
		}
	}

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CRenderer::Render_Reflection()
{
	if (m_RenderObjects[RG_Water].empty())
		return S_OK;

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Reflection"))))
		return E_FAIL;

	_mat OriginView = m_pGameInstance->Get_Transform(TransformType::View);

	for (auto& pWater : m_RenderObjects[RG_Water]) {
		if (pWater) {
			CTransform* pWaterTransform = dynamic_cast<CTransform*>(pWater->Find_Component(L"Com_Transform"));
			_float fWaterHeight = pWaterTransform->Get_CenterPos().y;
			_mat ReflectionViewMat = m_pGameInstance->ChagneViewForReflection(fWaterHeight);

			m_pGameInstance->Set_Transform(TransformType::View, ReflectionViewMat);
			m_pGameInstance->Update_PipeLine();

			_float4 vClipPlane = _float4(0.f, 1.f, 0.f, -(fWaterHeight - 0.5f));

			for (auto& pGameObject : m_RenderObjects[RG_Priority])
			{
				if (pGameObject)
				{
					if (FAILED(pGameObject->Render_Reflection(vClipPlane)))
					{
						MSG_BOX("Failed to Render");
						return E_FAIL;
					}
				}
			}

			for (auto& pGameObject : m_RenderObjects[RG_NonBlend])
			{
				if (pGameObject)
				{
					if (FAILED(pGameObject->Render_Reflection(vClipPlane)))
					{
						MSG_BOX("Failed to Render");
					}
				}
			}


			map<InstanceID, vector<CGameObject*>> InstanceData;

			for (auto& pGameObject : m_RenderObjects[RG_NonBlend_Instance])
			{
				if (pGameObject->Find_Component(L"Com_Model") == nullptr)
					continue;

				const _int iPassIndex = static_cast<CShader*>(pGameObject->Find_Component(L"Com_Shader"))->Get_PassIndex();
				const _int iInstanceID = static_cast<CModel*>(pGameObject->Find_Component(L"Com_Model"))->Get_InstanceID();
				InstanceID ID(iPassIndex, iInstanceID);
				InstanceData[ID].push_back(pGameObject);
			}

			for (auto& Pair : InstanceData)
			{
				vector<CGameObject*>& vInstances = Pair.second;
				const InstanceID instanceId = Pair.first;
				CGameObject*& pHead = vInstances[0];

				for (_uint i = 0; i < vInstances.size(); i++)
				{
					CGameObject*& pGameObject = vInstances[i];
					Instance_Data MeshInstancing;
					CTransform* pTransform = static_cast<CTransform*>(pGameObject->Find_Component(L"Com_Transform"));
					//MeshInstancing.vRight = pTransform->Get_State(State::Right);
					//MeshInstancing.vUp = pTransform->Get_State(State::Up);
					//MeshInstancing.vLook = pTransform->Get_State(State::Look);
					//MeshInstancing.vPos = pTransform->Get_State(State::Pos);
					MeshInstancing.mMatrix = pTransform->Get_World_Matrix();
					MeshInstancing.m_iID = pGameObject->Get_ID();
					Add_Instance(instanceId, MeshInstancing);
				}

				for (auto& iter : vInstances)
				{
					iter->InitRendered();
				}
				CVIBuffer_Mesh_Instance*& pBuffer = m_InstanceBuffers[instanceId];
				pHead->Render_Instance();
				CModel* pModel = static_cast<CModel*>(pHead->Find_Component(L"Com_Model"));
				CShader* pShader = static_cast<CShader*>(pHead->Find_Component(L"Com_Shader"));
				pModel->Render_Reflection_Instancing(pBuffer, pShader, vClipPlane);
			}


			m_pGameInstance->Set_Transform(TransformType::View, OriginView);
			m_pGameInstance->Update_PipeLine();
		}
	}

	if (FAILED(m_pGameInstance->End_MRT()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Clear_DepthStencil_View()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_Reflection_Light")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_RawValue("g_vCamNF", &m_pGameInstance->Get_CameraNF(), sizeof _float2)))
	{
		return E_FAIL;
	}

	for (auto& pGameObejct : m_RenderObjects[RG_Water]) {
		if (pGameObejct) {
			CTransform* pWaterTransform = dynamic_cast<CTransform*>(pGameObejct->Find_Component(L"Com_Transform"));
			_float fWaterHeight = pWaterTransform->Get_CenterPos().y;
			_mat ReflectionViewMat = m_pGameInstance->ChagneViewForReflection(fWaterHeight);
			_mat ReflectionViewInv = ReflectionViewMat.Invert();

			if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", ReflectionViewInv)))
			{
				return E_FAIL;
			}
			if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Inversed(TransformType::Proj))))
			{
				return E_FAIL;
			}

			_vec4 vCamPos = m_pGameInstance->Get_CameraPos();

			_float fReflectionCamY = -vCamPos.y + 2 * fWaterHeight;
			_vec4 vReflectionCamPos(vCamPos.x, fReflectionCamY, vCamPos.z, 1.f);

			if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &vReflectionCamPos, sizeof _float4)))
			{
				return E_FAIL;
			}

			if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_Normal_Spec_Texture", L"Target_Object_Reflection_Normal_Spec")))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_Depth_Velocity_Texture", L"Target_Object_Reflection_Depth_Velocity")))
				return E_FAIL;


			if (FAILED(m_pGameInstance->Render_Lights(m_pGameInstance->Get_CurrentLevelIndex(), m_pShader, m_pVIBuffer)))
			{
				return E_FAIL;
			}
		}
	}
	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_Reflection_Final")))
		return E_FAIL;

	for (auto& pWater : m_RenderObjects[RG_Water]) {
		if (pWater) {
			if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_DiffuseTexture", L"Target_Object_Reflection_Diffuse")))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_ShadeTexture", L"Target_Reflection_Shade")))
				return E_FAIL;

			if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_SpecularTexture", L"Target_Reflection_Specular")))
				return E_FAIL;

			if (FAILED(m_pShader->Begin(DefPass_Water)))
				return E_FAIL;

			if (FAILED(m_pVIBuffer->Render()))
				return E_FAIL;
		}
	}

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::Render_Water()
{
	for (auto& pGameObject : m_RenderObjects[RG_Water])
	{
		if (pGameObject)
		{
			if (FAILED(pGameObject->Render())) {
				MSG_BOX("Failed to Water Render");
				return E_FAIL;
			}
		}

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RG_Water].clear();

	return S_OK;
}

HRESULT CRenderer::Render_LightAcc()
{
#pragma region MRT_Lights

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Lights"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_Normal_Spec_Texture", TEXT("Target_Normal_Spec"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_Depth_Velocity_Texture", TEXT("Target_Depth_Velocity"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShader->Bind_RawValue("g_vCamPosition", &m_pGameInstance->Get_CameraPos(), sizeof _float4)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_RawValue("g_vCamNF", &m_pGameInstance->Get_CameraNF(), sizeof _float2)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Inversed(TransformType::View))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Inversed(TransformType::Proj))))
	{
		return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Render_Lights(m_pGameInstance->Get_CurrentLevelIndex(), m_pShader, m_pVIBuffer)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->End_MRT()))
	{
		return E_FAIL;
	}

#pragma endregion

#pragma region SSAO(Still Test...)

	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_SSAOTEST"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_Normal_Spec_Texture", TEXT("Target_Normal_Spec"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_Depth_Velocity_Texture", TEXT("Target_Depth_Velocity"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShader->Bind_Matrix("g_CamViewMatrix", m_pGameInstance->Get_Transform(TransformType::View))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_CamProjMatrix", m_pGameInstance->Get_Transform(TransformType::Proj))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_SSAO", &m_SSAO, sizeof(SSAO_DESC))))
		return E_FAIL;

	if (FAILED(m_pNoiseNormal->Bind_ShaderResource(m_pShader, "g_SSAONoiseNormal")))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(DefPass_SSAO)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pVIBuffer->Render()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->End_MRT()))
	{
		return E_FAIL;
	}

	if (FAILED(Get_BlurTex(m_pGameInstance->Get_SRV(L"Target_SSAOTEST"), L"MRT_SSAOBlur", m_fSSAOBlurPower)))
		return E_FAIL;

#pragma endregion

	if (FAILED(FinishCommand()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Deferred()
{
	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_Deferred")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_DiffuseTexture", TEXT("Target_Diffuse"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_ShadeTexture", TEXT("Target_Shade"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_SpecularTexture", TEXT("Target_Specular"))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_Depth_Velocity_Texture", TEXT("Target_Depth_Velocity"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_SSAOTexture", TEXT("Target_SSAOBlur"))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_RimMaskTexture", TEXT("Target_Rim"))))
		return E_FAIL;

	_bool TurnOnShadow = m_pGameInstance->Get_TurnOnShadow();
	if (FAILED(m_pShader->Bind_RawValue("g_TurnOnShadow", &TurnOnShadow, sizeof(_bool))))
		return E_FAIL;

	if (true == TurnOnShadow) {
		if (FAILED(m_pShadowMap->Bind_SRV(m_pShader, "g_LightDepthTexture")))
			return E_FAIL;

		CASCADE_DESC Cascade = m_pGameInstance->Get_CascadeDesc();

		if (FAILED(m_pShader->Bind_Matrices("g_LightViewMatrix", Cascade.LightView, 3)))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_Matrices("g_LightProjMatrix", Cascade.LightProj, 3)))
			return E_FAIL;

		_vec4 ClipZ;
		ClipZ.x = Cascade.ClipZ[0];
		ClipZ.y = Cascade.ClipZ[1];
		ClipZ.z = Cascade.ClipZ[2];
		ClipZ.w = Cascade.ClipZ[3];

		if (FAILED(m_pShader->Bind_RawValue("g_ClipZ", &ClipZ, sizeof(_vec4))))
			return E_FAIL;
	}

	if (FAILED(m_pShader->Bind_RawValue("g_fHellStart", &m_pGameInstance->Get_HellHeight(), sizeof _float)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShader->Bind_RawValue("g_vCamNF", &m_pGameInstance->Get_CameraNF(), sizeof _float2)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_RawValue("g_vFogNF", &m_pGameInstance->Get_FogNF(), sizeof _float2)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_RawValue("g_vFogColor", &m_pGameInstance->Get_FogColor(), sizeof _color)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInv", m_pGameInstance->Get_Transform_Inversed(TransformType::View))))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInv", m_pGameInstance->Get_Transform_Inversed(TransformType::Proj))))
	{
		return E_FAIL;
	}


	if (m_pGameInstance->Key_Down(DIK_F2))
		m_TurnOnSSAO = !m_TurnOnSSAO;

	// 원명
	if (FAILED(m_pShader->Bind_RawValue("TurnOnSSAO", &m_TurnOnSSAO, sizeof(_bool))))
		return E_FAIL;

	//if (FAILED(m_pShader->Bind_RawValue("TurnOnThunder", &m_Thunder, sizeof(_bool))))
	//	return E_FAIL;

	if (FAILED(m_pShader->Begin(DefPass_Deferred)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pVIBuffer->Render()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_HDR()
{
	// 원명
	if (FAILED(Get_AvgLuminance()))
		return E_FAIL;

	if (FAILED(FinishCommand()))
		return E_FAIL;

	if (FAILED(Get_BlurTex(m_pGameInstance->Get_SRV(L"Target_Deferrd"), L"MRT_BLURTEX", m_fHDRBloomPower, true)))
		return E_FAIL;

	if (FAILED(FinishCommand()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_HDR")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_DeferredTexture", TEXT("Target_Deferrd"))))
	{
		return E_FAIL;
	}

	//if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_HDRTexture", TEXT("Target_BlurTex"))))
	//{
	//	return E_FAIL;
	//}

	if (FAILED(m_pShader->Bind_ShaderResourceView("g_Luminance", m_pLumValue->Get_SRV())))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_BloomTexture", L"Target_BlurTex")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("g_HDR", &m_HDR, sizeof(HDR_DESC))))
		return E_FAIL;

	if (m_pGameInstance->Key_Down(DIK_F3))
		m_TurnOnToneMap = !m_TurnOnToneMap;

	if (m_pGameInstance->Key_Down(DIK_F4))
		m_TurnOnBloom = !m_TurnOnBloom;

	if (FAILED(m_pShader->Bind_RawValue("TurnOnToneMap", &m_TurnOnToneMap, sizeof(_bool))))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_RawValue("TurnOnBloom", &m_TurnOnBloom, sizeof(_bool))))
		return E_FAIL;


	if (FAILED(m_pShader->Begin(DefPass_HDR)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pVIBuffer->Render()))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Outline()
{
	if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Outline"))))
	{
		return E_FAIL;
	}

	_vec4 CheckColor;
	for (size_t i = OutlineColor_White; i < OutlineColor_End; i++)
	{
		switch (i)
		{
		case OutlineColor_White:
			CheckColor = _vec4(0.f, 0.f, 1.f, 1.f);
			break;
		case OutlineColor_Yellow:
			CheckColor = _vec4(0.f, 1.f, 0.f, 1.f);
			break;
		case OutlineColor_Red:
			CheckColor = _vec4(1.f, 0.f, 0.f, 1.f);
			break;
		}
		if (FAILED(m_pShader->Bind_RawValue("g_OutlineColorIndex", &i, sizeof(_uint))))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_RawValue("g_OutlineColor", &CheckColor, sizeof(_vec4))))
			return E_FAIL;

		if (FAILED(m_pShader->Begin(DefPass_Stencil)))
			return E_FAIL;

		if (FAILED(m_pVIBuffer->Render()))
			return E_FAIL;
	}


	if (FAILED(m_pGameInstance->End_MRT()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_Effect")))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RG_Blend])
	{
		if (pGameObject)
		{
			if (FAILED(pGameObject->Render()))
			{
				MSG_BOX("Failed to Render");
			}
		}

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RG_Blend].clear();

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	if (FAILED(Get_BlurTex(m_pGameInstance->Get_SRV(L"Target_Effect_Blur"), L"MRT_Blur", m_fEffectBlurPower)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Distortion()
{
	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_Distortion")))
		return E_FAIL;

	for (auto& pGameObject : m_RenderObjects[RG_Distortion])
	{
		if (pGameObject)
		{
			if (FAILED(pGameObject->Render()))
			{
				MSG_BOX("Failed to Render");
			}
		}

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RG_Distortion].clear();


	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_NoneBlendFinal()
{
	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_HDR_Sky")))
		return E_FAIL;

	if (FAILED(Render_Priority()))
		return E_FAIL;

	// HDR Texture
	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_Texture", L"Target_HDR")))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(DefPass_JustDraw)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	// Outline
	_vec4 vColor;
	_vec4 CheckColor;
	for (size_t i = OutlineColor_White; i < OutlineColor_End; i++)
	{
		switch (i)
		{
		case OutlineColor_White:
			CheckColor = _vec4(0.f, 0.f, 1.f, 1.f);
			vColor = _vec4(1.f, 1.f, 1.f, 1.f);
			break;
		case OutlineColor_Yellow:
			CheckColor = _vec4(0.f, 1.f, 0.f, 1.f);
			vColor = _vec4(1.f, 1.f, 0.f, 1.f);
			break;
		case OutlineColor_Red:
			CheckColor = _vec4(1.f, 0.f, 0.f, 1.f);
			vColor = _vec4(1.f, 0.f, 0.f, 1.f);
			break;
		}

		if (FAILED(m_pShader->Bind_RawValue("g_OutlineColorIndex", &i, sizeof(_uint))))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_RawValue("g_CheckColor", &CheckColor, sizeof(_vec4))))
			return E_FAIL;

		if (FAILED(m_pShader->Bind_RawValue("g_OutlineColor", &vColor, sizeof(_vec4))))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_StencilTexture", L"Target_Outline")))
			return E_FAIL;

		if (FAILED(m_pShader->Begin(DefPass_Outline)))
			return E_FAIL;

		if (FAILED(m_pVIBuffer->Render()))
			return E_FAIL;

	}

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_BlendFinal()
{
	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_HDR_Sky", nullptr, false)))
		return E_FAIL;

	// Effect
	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_EffectColorTexture", L"Target_Effect")))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_AlphaTexture", L"Target_Effect_Alpha")))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(DefPass_Blend)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_HDR_Sky", nullptr, false)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_BlendTexture", L"Target_Bloom")))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(DefPass_Blur)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;


	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_Final()
{
	if (m_pGameInstance->Key_Down(DIK_F6))
		m_bMotionBlur = !m_bMotionBlur;

	if (true == m_bMotionBlur) {

		if (FAILED(m_pMotionShader->Set_Shader()))
			return E_FAIL;

		ID3D11ShaderResourceView* SRVs[2] = { m_pGameInstance->Get_SRV(L"Target_HDR_Sky"), m_pGameInstance->Get_SRV(L"Target_Depth_Velocity") };

		if (FAILED(m_pMotionShader->Bind_SRVs(SRVs, 0, 2)))
			return E_FAIL;

		if (FAILED(m_pMotionShader->Bind_UAV(m_pMotionRT->Get_UAV(), 0)))
			return E_FAIL;

		_uint2 ThreadGroupSize = _uint2((m_WinSize.x + 7) / 8, (m_WinSize.y + 7) / 8);
		if (FAILED(m_pMotionShader->Begin_MultiSRV(_uint3(ThreadGroupSize.x, ThreadGroupSize.y, 1))))
			return E_FAIL;

		//if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_MotionBlur")))
		//	return E_FAIL;

		//if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_Depth_Velocity_Texture", L"Target_Depth_Velocity")))
		//	return E_FAIL;

		//if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_Texture", L"Target_HDR_Sky")))
		//	return E_FAIL;

		//if (FAILED(m_pShader->Begin(14)))
		//	return E_FAIL;

		//if (FAILED(m_pVIBuffer->Render()))
		//	return E_FAIL;

		//if (FAILED(m_pGameInstance->End_MRT()))
		//	return E_FAIL;

	}

#pragma region Old_RadialBlur

	//if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_RadialBlur")))
	//	return E_FAIL;

	//if (FAILED(m_pShader->Bind_RawValue("g_fRadialBlur_Power", &m_fRadial_BlurPower, sizeof(_float))))
	//	return E_FAIL;

	//if (FAILED(m_pShader->Bind_RawValue("g_WorldOrTex", &m_bWorldOrTex, sizeof(_bool))))
	//	return E_FAIL;

	//if (false == m_bWorldOrTex) {
	//	if (FAILED(m_pShader->Bind_RawValue("g_vCenterPos", &m_vRadialWorldCenter, sizeof(_vec4))))
	//		return E_FAIL;
	//}
	//else {
	//	if (FAILED(m_pShader->Bind_RawValue("g_vCenterTexPos", &m_vRadialTexCenter, sizeof(_vec2))))
	//		return E_FAIL;
	//}

	//if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_Texture", MotionBlur)))
	//	return E_FAIL;

	//if (FAILED(m_pShader->Begin(15)))
	//	return E_FAIL;

	//if (FAILED(m_pVIBuffer->Render()))
	//	return E_FAIL;

	//if (FAILED(m_pGameInstance->End_MRT()))
	//	return E_FAIL;

#pragma endregion

#pragma region Compute_RadialBlur

	if (FAILED(m_pRadialShader->Set_Shader()))
		return E_FAIL;

	_uint2 iSlot = _uint2(0, 0);
	if (true == m_bMotionBlur) {
		if (FAILED(m_pRadialShader->Bind_ShaderResourceView(m_pMotionRT->Get_SRV(), m_pRadialRT->Get_UAV(), iSlot)))
			return E_FAIL;
	}
	else {
		if (FAILED(m_pRadialShader->Bind_ShaderResourceView(m_pGameInstance->Get_SRV(L"Target_HDR_Sky"), m_pRadialRT->Get_UAV(), iSlot)))
			return E_FAIL;
	}

	if(FAILED(m_pRadialShader->Change_Value(&m_RBParams, sizeof(RadialParams), 1)))
		return E_FAIL;

	_uint2 ThreadGroupSize = _uint2((m_WinSize.x + 7) / 8, (m_WinSize.y + 7) / 8);
	if (FAILED(m_pRadialShader->Begin(_uint3(ThreadGroupSize.x, ThreadGroupSize.y, 1))))
		return E_FAIL;

#pragma endregion

	if (FAILED(FinishCommand()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pShader, "g_DistortionTexture", L"Target_Distortion")))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_ShaderResourceView("g_Texture", m_pRadialRT->Get_SRV())))
		return E_FAIL;

	if (FAILED(m_pShader->Begin(DefPass_Distortion)))
	{
		return E_FAIL;
	}
	if (FAILED(m_pVIBuffer->Render()))
	{
		return E_FAIL;
	}

	return S_OK;
}


HRESULT CRenderer::Render_UI()
{
	m_RenderObjects[RG_UI].sort([](CGameObject* pSrc, CGameObject* pDst)
		{
			return dynamic_cast<COrthographicObject*>(pSrc)->Get_Depth() > dynamic_cast<COrthographicObject*>(pDst)->Get_Depth();
		});

	for (auto& pGameObject : m_RenderObjects[RG_UI])
	{
		if (pGameObject)
		{
			if (FAILED(pGameObject->Render()))
			{
				MSG_BOX("Failed to Render");
			}
		}

		Safe_Release(pGameObject);
	}

	m_RenderObjects[RG_UI].clear();

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderer::Render_Debug()
{
	for (auto& pComponent : m_DebugComponents)
	{
		pComponent->Render();
		Safe_Release(pComponent);
	}
	m_DebugComponents.clear();

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
	{
		return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Render_Debug_RT(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Render_Debug_RT(TEXT("MRT_Lights"), m_pShader, m_pVIBuffer)))
	{
		return E_FAIL;
	}

	//if (FAILED(m_pGameInstance->Render_Debug_RT(TEXT("MRT_Shadow"), m_pShader, m_pVIBuffer)))
	//{
	//	return E_FAIL;
	//}

	if (FAILED(m_pGameInstance->Render_Debug_RT(TEXT("MRT_Effect"), m_pShader, m_pVIBuffer)))
	{
		return E_FAIL;
	}

	//if (FAILED(m_pGameInstance->Render_Debug_RT(TEXT("MRT_BlurTest"), m_pShader, m_pVIBuffer)))
	//{
	//	return E_FAIL;
	//}

	if (FAILED(m_pGameInstance->Render_Debug_RT(TEXT("MRT_SSAOBlur"), m_pShader, m_pVIBuffer)))
	{
		return E_FAIL;
	}


	if (FAILED(m_pGameInstance->Render_Debug_RT(TEXT("MRT_Reflection_Final"), m_pShader, m_pVIBuffer)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShadowMap->Render(m_pShader, m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Debug_RT(L"MRT_Outline", m_pShader, m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Render_Debug_RT(L"MRT_Deferred", m_pShader, m_pVIBuffer)))
		return E_FAIL;

	return S_OK;
}
#endif // _DEBUG

HRESULT CRenderer::Get_AvgLuminance()
{
	_uint2 iSize{};

	_uint2 iSlot{};

	{
		iSlot = _uint2(0, 0);
		ID3D11ShaderResourceView* pSRV = m_pGameInstance->Get_SRV(L"Target_Deferrd");;

		if (FAILED(m_pLumShader->Set_Shader()))
			return E_FAIL;

		if (FAILED(m_pLumShader->Bind_ShaderResourceView(pSRV, m_pLumRT->Get_UAV(), iSlot)))
			return E_FAIL;

		iSize = _uint2(m_WinSize.x / 32, m_WinSize.y / 18);

		if (FAILED(m_pLumShader->Begin(_uint3(iSize.x, iSize.y, 1))))
			return E_FAIL;
	}

	{
		iSlot = _uint2(1, 1);

		if (FAILED(m_pDownShader->Set_Shader()))
			return E_FAIL;

		if (FAILED(m_pDownShader->Bind_ShaderResourceView(m_pLumRT->Get_SRV(), m_pDownRT->Get_UAV(), iSlot)))
			return E_FAIL;

		iSize = _uint2(320 / 16, 240 / 16);

		if (FAILED(m_pDownShader->Begin(_uint3(iSize.x, iSize.y, 1))))
			return E_FAIL;
	}

	{
		iSlot = _uint2(2, 2);
		if (FAILED(m_pDown2Shader->Set_Shader()))
			return E_FAIL;

		if (FAILED(m_pDown2Shader->Bind_ShaderResourceView(m_pDownRT->Get_SRV(), m_pDownRT1->Get_UAV(), iSlot)))
			return E_FAIL;

		iSize = _uint2(80 / 16, 60 / 12);

		if (FAILED(m_pDown2Shader->Begin(_uint3(iSize.x, iSize.y, 1))))
			return E_FAIL;
	}

	{
		iSlot = _uint2(3, 3);
		if (FAILED(m_pDown3Shader->Set_Shader()))
			return E_FAIL;

		if (FAILED(m_pDown3Shader->Bind_ShaderResourceView(m_pDownRT1->Get_SRV(), m_pDownRT2->Get_UAV(), iSlot)))
			return E_FAIL;

		if (FAILED(m_pDown3Shader->Begin(_uint3(2, 2, 1))))
			return E_FAIL;
	}

	{
		iSlot = _uint2(4, 4);
		if (FAILED(m_pGetAvgLumShader->Set_Shader()))
			return E_FAIL;

		if (FAILED(m_pGetAvgLumShader->Bind_ShaderResourceView(m_pDownRT2->Get_SRV(), m_pLumValue->Get_UAV(), iSlot)))
			return E_FAIL;

		if (FAILED(m_pGetAvgLumShader->Begin(_uint3(1, 1, 1))))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CRenderer::Get_BlurTex(ID3D11ShaderResourceView* pSRV, const wstring& MRT_Tag, _float fBlurPower, _bool isBloom)
{

	ID3D11ShaderResourceView* InputSRV = pSRV;

	if (true == isBloom) {
		if (FAILED(m_pGameInstance->Begin_MRT(L"MRT_BLURTEX")))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Bind_ShaderResourceView(m_pGetBlurShader, "g_DeferredTexture", L"Target_Deferrd")))
			return E_FAIL;

		if (FAILED(m_pGetBlurShader->Bind_ShaderResourceView("g_Luminance", m_pLumValue->Get_SRV())))
			return E_FAIL;

		if (FAILED(m_pGetBlurShader->Begin(1)))
			return E_FAIL;

		if (FAILED(m_pVIBuffer->Render()))
			return E_FAIL;

		if (FAILED(m_pGameInstance->End_MRT()))
			return E_FAIL;

		InputSRV = m_pGameInstance->Get_SRV(L"Target_BlurTex");
	}

	_uint2 iSlot = _uint2(0, 0);

	ID3D11ShaderResourceView* pSRVs[3] = { InputSRV, m_pHalfRT->Get_SRV(), m_pQuarterRT->Get_SRV() };
	ID3D11UnorderedAccessView* pUAVs[3] = { m_pHalfRT->Get_UAV(), m_pQuarterRT->Get_UAV(), m_pEightRT->Get_UAV() };

	if (FAILED(m_pDownScaleShader->Set_Shader()))
		return E_FAIL;

	_uint2 iSize = m_WinSize;

	for (size_t i = 0; i < 3; i++)
	{
		if (FAILED(m_pDownScaleShader->Change_Value(&m_DSParam[i], sizeof(DSPARAM))))
			return E_FAIL;

		if (FAILED(m_pDownScaleShader->Bind_ShaderResourceView(pSRVs[i], pUAVs[i], iSlot)))
			return E_FAIL;

		_uint2 ThreadGroupSize = _uint2((iSize.x + 15) / 16, (iSize.y + 15) / 16);

		if (FAILED(m_pDownScaleShader->Begin(_uint3(ThreadGroupSize.x, ThreadGroupSize.y, 1))))
			return E_FAIL;

		iSize = _uint2(iSize.x / 2, iSize.y / 2);
	}

	if (FAILED(FinishCommand()))
		return E_FAIL;

	ID3D11ShaderResourceView* pDownSRVs[3] = { m_pHalfRT->Get_SRV(), m_pQuarterRT->Get_SRV(), m_pEightRT->Get_SRV() };
	ID3D11ShaderResourceView* pBlurSRVs[3] = { m_pHalfBlurRT->Get_SRV(), m_pQuarterBlurRT->Get_SRV(), m_pEightBlurRT->Get_SRV() };
	ID3D11UnorderedAccessView* pBlurUAVs[3] = { m_pHalfBlurRT->Get_UAV(), m_pQuarterBlurRT->Get_UAV(), m_pEightBlurRT->Get_UAV() };

	iSize = m_WinSize;

	if (FAILED(m_pBlurShader->Set_Shader()))
		return E_FAIL;


	m_BLParam.fBlurPower = fBlurPower;
	for (size_t i = 0; i < 3; i++)
	{
		iSize = _uint2(iSize.x / 2, iSize.y / 2);

		m_BLParam.ScaleX = iSize.x;
		m_BLParam.ScaleY = iSize.y;

		_uint2 ThreadGroupSize = _uint2((iSize.x + 7) / 8, (iSize.y + 7) / 8);

		for (size_t j = 0; j < 2; j++)
		{
			if (0 == j % 2) {
				if (FAILED(m_pBlurShader->Bind_ShaderResourceView(pDownSRVs[i], pBlurUAVs[i], iSlot)))
					return E_FAIL;
			}
			else {
				if (FAILED(m_pBlurShader->Bind_ShaderResourceView(pBlurSRVs[i], pUAVs[i], iSlot)))
					return E_FAIL;
			}
			//
			m_BLParam.direction = j; // 가로 세로로 블러먹이기
			if (FAILED(m_pBlurShader->Change_Value(&m_BLParam, sizeof(BLURPARAM))))
				return E_FAIL;

			if (FAILED(m_pBlurShader->Begin(_uint3(ThreadGroupSize.x, ThreadGroupSize.y, 1))))
				return E_FAIL;
		}

	}

	if (FAILED(FinishCommand()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Begin_MRT(MRT_Tag)))
		return E_FAIL;

	if (FAILED(m_pGetBlurShader->Bind_Matrix("g_WorldMatrix", m_WorldMatrix)))
		return E_FAIL;
	if (FAILED(m_pGetBlurShader->Bind_Matrix("g_ViewMatrix", m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pGetBlurShader->Bind_Matrix("g_ProjMatrix", m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pGetBlurShader->Bind_ShaderResourceView("g_HalfTex", m_pHalfRT->Get_SRV())))
		return E_FAIL;

	if (FAILED(m_pGetBlurShader->Bind_ShaderResourceView("g_QuarterTex", m_pQuarterRT->Get_SRV())))
		return E_FAIL;

	if (FAILED(m_pGetBlurShader->Bind_ShaderResourceView("g_EightTex", m_pEightRT->Get_SRV())))
		return E_FAIL;


	if (FAILED(m_pGetBlurShader->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBuffer->Render()))
		return E_FAIL;

	if (FAILED(m_pGameInstance->End_MRT()))
		return E_FAIL;


	return S_OK;
}

HRESULT CRenderer::Add_Instance(InstanceID InstanceID, Instance_Data& pMeshInstancing)
{
	if (m_InstanceBuffers.find(InstanceID) == m_InstanceBuffers.end())
	{
		m_InstanceBuffers[InstanceID] = CVIBuffer_Mesh_Instance::Create(m_pDevice, m_pContext);
	}
	m_InstanceBuffers[InstanceID]->Add_Instance(pMeshInstancing);
	return S_OK;
}

HRESULT CRenderer::Clear_Instance()
{
	for (auto& pair : m_InstanceBuffers)
	{
		CVIBuffer_Mesh_Instance* pBuffer = pair.second;
		pBuffer->Clear_Instance();
	}
	return S_OK;
}

HRESULT CRenderer::FinishCommand()
{
	if (FAILED(m_pDeferrd->FinishCommandList(FALSE, &m_pCommandList)))
		return E_FAIL;

	m_pContext->ExecuteCommandList(m_pCommandList, TRUE);

	Safe_Release(m_pCommandList);

	return S_OK;
}

const void CRenderer::Set_RadialBlur_World(_vec4 vRadialCenter) {
	_vec4 vPos = vRadialCenter;
	vPos = XMVector3TransformCoord(vPos, m_pGameInstance->Get_Transform(TransformType::View));
	vPos = XMVector3TransformCoord(vPos, m_pGameInstance->Get_Transform(TransformType::Proj));
	vPos = vPos / vPos.w;
	m_RBParams.vCenterPos = vPos;
	m_RBParams.isTex = false;
}

CRenderer* CRenderer::Create(_dev pDevice, _context pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Create : CRenderer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CRenderer::Clone(void* pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	__super::Free();

#pragma region 평균휘도 Release

	Clear_Instance();

	Safe_Release(m_pLumShader);
	Safe_Release(m_pLumRT);

	Safe_Release(m_pDownShader);
	Safe_Release(m_pDownRT);

	Safe_Release(m_pDown2Shader);
	Safe_Release(m_pDownRT1);

	Safe_Release(m_pDown3Shader);
	Safe_Release(m_pDownRT2);

	Safe_Release(m_pGetAvgLumShader);
	Safe_Release(m_pLumValue);

#pragma endregion

#pragma region 블러 Release

	Safe_Release(m_pBlurShader);

	Safe_Release(m_pDownScaleShader);

	Safe_Release(m_pHalfRT);
	Safe_Release(m_pQuarterRT);
	Safe_Release(m_pEightRT);

	Safe_Release(m_pHalfBlurRT);
	Safe_Release(m_pQuarterBlurRT);
	Safe_Release(m_pEightBlurRT);

	Safe_Release(m_pGetBlurShader);

#pragma endregion

#pragma region Radial Blur Release
	Safe_Release(m_pRadialShader);
	Safe_Release(m_pRadialRT);
#pragma endregion

#pragma region Motion Blur Release
	Safe_Release(m_pMotionShader);
	Safe_Release(m_pMotionRT);
#pragma endregion



	Safe_Release(m_pShadowMap);

	//SSAO
	Safe_Release(m_pNoiseNormal);

	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShadowDSV);

	for (auto& ObjectList : m_RenderObjects)
	{
		for (auto& pGameObject : ObjectList)
		{
			Safe_Release(pGameObject);
		}
		ObjectList.clear();
	}

#ifdef _DEBUG
	for (auto& pComponent : m_DebugComponents)
	{
		Safe_Release(pComponent);
	}
	m_DebugComponents.clear();
#endif // _DEBUG

	for (auto& pair : m_InstanceBuffers)
	{
		Safe_Release(pair.second);  // 동적으로 할당된 메모리를 해제
	}
	m_InstanceBuffers.clear();


	Safe_Release(m_pDeferrd);
}