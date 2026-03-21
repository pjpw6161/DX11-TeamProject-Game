#include "VIBuffer_Instancing.h"
#include "GameInstance.h"
#include "Compute_Shader.h"

CVIBuffer_Instancing::CVIBuffer_Instancing(_dev pDevice, _context pContext)
	: CVIBuffer(pDevice, pContext)
{
}

CVIBuffer_Instancing::CVIBuffer_Instancing(const CVIBuffer_Instancing& rhs)
	: CVIBuffer(rhs)
	, m_InstancingBufferDesc(rhs.m_InstancingBufferDesc)
	, m_InstancingInitialData(rhs.m_InstancingInitialData)
	, m_iNumInstances(rhs.m_iNumInstances)
	, m_iIndexCountPerInstance(rhs.m_iIndexCountPerInstance)
	, m_iInstanceStride(rhs.m_iInstanceStride)
	, m_pVSRB(rhs.m_pVSRB)
	, m_pVUAVB(rhs.m_pVUAVB)
	, m_pComputeShader(dynamic_cast<CCompute_Shader*>(rhs.m_pComputeShader->Clone()))
{
}

HRESULT CVIBuffer_Instancing::Init_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Instancing::Init(void* pArg)
{
	return S_OK;
}

void CVIBuffer_Instancing::Update(_float fTimeDelta, _mat WorldMatrix, _int iNumUse, _bool bApplyGravity, _vec3 vGravityDir, _float fAppearRatio, _float fDissolveRatio)
{
	_bool CS_Particle = true;

	if (not CS_Particle)
	{
#pragma region Map-Unmap Particle System
		if (iNumUse == -1)
		{
			iNumUse = m_iNumInstances;
		}
		D3D11_MAPPED_SUBRESOURCE SubResource{};

		m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

		for (size_t i = 0; i < m_iNumInstances; i++)
		{
			VTXINSTANCING* pVertex = &reinterpret_cast<VTXINSTANCING*>(SubResource.pData)[i];

			if (pVertex->vLifeTime.x == 0)
			{
				pVertex->vPos = _vec4::Transform(pVertex->vOriginPos, WorldMatrix);
				pVertex->vPrevPos = pVertex->vPos;

				pVertex->vDirection = _vec4::Transform(pVertex->vOriginDir, WorldMatrix.Get_Rotation());
			}

			if (m_isLoop and m_isFirstUpdate)
			{
				pVertex->vPos = _float4(0.f, -10000.f, 0.f, 1.f);
				pVertex->vPrevPos = pVertex->vPos;
			}

			//pVertex->vPos.y += pVertex->fSpeed * fTimeDelta;
			if (bApplyGravity)
			{
				_float fAlpha = (pVertex->vLifeTime.x / pVertex->vLifeTime.y) * 0.7f;
				pVertex->vDirection = _vec4::Lerp(pVertex->vDirection, _vec4(vGravityDir), fAlpha);
			}

			if (fAppearRatio > 0.f and pVertex->vLifeTime.x <= pVertex->vLifeTime.y * fAppearRatio)
			{
				pVertex->fDissolveRatio = 1.f - (pVertex->vLifeTime.x / (pVertex->vLifeTime.y * fAppearRatio));
				pVertex->fDissolveRatio = min(pVertex->fDissolveRatio, 1.f);
			}
			else if (pVertex->vLifeTime.x > pVertex->vLifeTime.y * fDissolveRatio)
			{
				pVertex->fDissolveRatio = (pVertex->vLifeTime.x - (pVertex->vLifeTime.y * fDissolveRatio)) / (pVertex->vLifeTime.y * (1.f - fDissolveRatio));
				pVertex->fDissolveRatio = min(pVertex->fDissolveRatio, 1.f);
			}
			else
			{
				pVertex->fDissolveRatio = 0.f;
			}

			pVertex->fIndex = clamp(pVertex->vLifeTime.x / pVertex->vLifeTime.y, 0.f, 1.f);

			pVertex->vPrevPos = pVertex->vPos;
			pVertex->vPos += pVertex->vDirection * pVertex->fSpeed * fTimeDelta;
			pVertex->vLifeTime.x += fTimeDelta;

			if (i >= iNumUse)
			{
				pVertex->vPos = _float4(-1000.f, -1000.f, -1000.f, 1.f);
				continue;
			}

			if (pVertex->vLifeTime.x > pVertex->vLifeTime.y)
			{
				if (m_isLoop)
				{
					pVertex->vLifeTime.x = 0.f;
					pVertex->vPos = _vec4::Transform(pVertex->vOriginPos, WorldMatrix);
					pVertex->vPrevPos = pVertex->vPos;
				}
				else
				{
					pVertex->vPos = _float4(0.f, -10000.f, 0.f, 1.f);
					pVertex->vPrevPos = pVertex->vPos;
				}
			}
		}

		m_pContext->Unmap(m_pVBInstance, 0);

#pragma endregion 
	}
	else
	{
#pragma region Compute Shader Particle System
		ParticleParams PartiBuffer{};
		PartiBuffer.iNumInstances = m_iNumInstances;
		PartiBuffer.iNumUse = iNumUse;
		PartiBuffer.fTimeDelta = fTimeDelta;
		PartiBuffer.vGravityDir = vGravityDir;
		PartiBuffer.fAppearRatio = fAppearRatio;
		PartiBuffer.fDissolveRatio = fDissolveRatio;
		PartiBuffer.WorldMatrix = WorldMatrix;
		PartiBuffer.isLoop = m_isLoop;
		PartiBuffer.bApplyGravity = bApplyGravity;
		PartiBuffer.isFirstUpdate = m_isFirstUpdate;

		m_pContext->CopyResource(m_pVSRB, m_pVBInstance);

		HRESULT hr = E_FAIL;

		hr = m_pComputeShader->Set_Shader();

		hr = m_pComputeShader->Change_Value(&PartiBuffer, sizeof ParticleParams);

		_uint2 iSlot = _uint2(0, 0);
		hr = m_pComputeShader->Bind_ShaderResourceView(m_pSRV, m_pUAV, iSlot);

		_uint3 ThreadGroup{ 2, 1, 1 };
		hr = m_pComputeShader->Begin(ThreadGroup);

		if (FAILED(hr))
			return;

		m_pContext->CopyResource(m_pVBInstance, m_pVUAVB);

#pragma region 값 확인용
		//D3D11_MAPPED_SUBRESOURCE SubResource{};

	//m_pContext->Map(m_pVUAVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	//VTXINSTANCING* pVertex = &reinterpret_cast<VTXINSTANCING*>(SubResource.pData)[0];

	//_vec4 vPos = pVertex->vPos;

	//m_pContext->Unmap(m_pVUAVB, 0);  
#pragma endregion
#pragma endregion 
	}

	m_isFirstUpdate = false;
}

HRESULT CVIBuffer_Instancing::Render()
{
	ID3D11Buffer* pVertexBuffer[] =
	{
		m_pVB,
		m_pVBInstance,
	};

	_uint iVertexStrides[] =
	{
		m_iVertexStride,
		m_iInstanceStride,
	};

	_uint iOffsets[] =
	{
		0,
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffer, iVertexStrides, iOffsets);

	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

	m_pContext->DrawIndexedInstanced(m_iIndexCountPerInstance, m_iNumInstances, 0, 0, 0);

	return S_OK;
}

void CVIBuffer_Instancing::Free()
{
	__super::Free();

	Safe_Release(m_pVBInstance);
	Safe_Release(m_pSRV);
	Safe_Release(m_pUAV);
	Safe_Release(m_pComputeShader);
	Safe_Release(m_pVSRB);
	Safe_Release(m_pVUAVB);
}
