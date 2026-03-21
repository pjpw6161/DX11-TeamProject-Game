#include "Picking.h"
#include "GameInstance.h"

CPicking::CPicking(_dev pDevice, _context pContext, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
	:m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_hWnd(hWnd)
	, m_iWinSizeX(iWinSizeX)
	, m_iWinSizeY(iWinSizeY)
	, m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

void CPicking::Tick()
{
	// 뷰포트 상의 커서 위치
	POINT ptCursor{};
	GetCursorPos(&ptCursor);
	ScreenToClient(m_hWnd, &ptCursor);

	// 투영 스페이스 상의 위치
	_vector vCursorPos
	{
		XMVectorSet
		(
			ptCursor.x / (m_iWinSizeX * 0.5f) - 1.f,
			ptCursor.y / (m_iWinSizeY * -0.5f) + 1.f,
			0.f,
			1.f
		)
	};

	// 뷰 스페이스 상의 위치
	_matrix ProjMatrix_Inverse{ m_pGameInstance->Get_Transform_Inversed(TransformType::Proj) };
	vCursorPos = XMVector4Transform(vCursorPos, ProjMatrix_Inverse);

	_vector vRayPos{ XMVectorSet(0.f, 0.f, 0.f, 1.f) };
	_vector vRayDir{ XMVectorSet(vCursorPos.m128_f32[0], vCursorPos.m128_f32[1], 1.f, 0.f) };
	//_vector vRayDir{ vCursorPos - vRayPos }; 

	// 월드 스페이스 상의 위치
	_matrix ViewMat_Inverse{ m_pGameInstance->Get_Transform_Inversed(TransformType::View) };
	XMStoreFloat4(&m_vRayPos_World, XMVector4Transform(vRayPos, ViewMat_Inverse));
	XMStoreFloat4(&m_vRayDir_World, XMVector4Normalize(XMVector4Transform(vRayDir, ViewMat_Inverse)));
}

void CPicking::TransformRay_ToLocal(_fmatrix WorldMatrix)
{
	_matrix InversedWorld{ XMMatrixInverse(nullptr, WorldMatrix) };

	XMStoreFloat4(&m_vRayPos_Local, XMVector4Transform(XMLoadFloat4(&m_vRayPos_World), InversedWorld));
	XMStoreFloat4(&m_vRayDir_Local, XMVector4Normalize(XMVector4Transform(XMLoadFloat4(&m_vRayDir_World), InversedWorld)));
}

_bool CPicking::Picking_InWorld(_vec4 vPoint1, _vec4 vPoint2, _vec4 vPoint3, _Inout_ _vec3* pPickPos)
{
	_float fDist{};
	_vector vRayPos{ XMLoadFloat4(&m_vRayPos_World) };
	_vector vRayDir{ XMLoadFloat4(&m_vRayDir_World) };
	if (TriangleTests::Intersects(vRayPos, vRayDir, vPoint1, vPoint2, vPoint3, fDist))
	{
		XMStoreFloat3(pPickPos, vRayPos + (vRayDir * fDist));
		return true;
	}
	else
	{
		return false;
	}
}

_float4 CPicking::PickingDepth(_float x, _float y)
{
	ID3D11Texture2D* pTexture = nullptr;

	pTexture = m_pGameInstance->Get_Texture2D(L"Target_Depth_Velocity");

	if (nullptr == pTexture) {
		MSG_BOX("Get Target_Depth FAILED");
		return _float4(0.f, 0.f, 0.f, 0.f);
	}

	D3D11_BOX m_Box;
	m_Box.left = static_cast<_uint>(x);
	m_Box.top = static_cast<_uint>(y);
	m_Box.right = static_cast<_uint>(x + 1);
	m_Box.bottom = static_cast<_uint>(y + 1);
	m_Box.front = 0;
	m_Box.back = 1;

	m_pContext->CopySubresourceRegion(m_pTexture, 0, 0, 0, 0, pTexture, 0, &m_Box);

	D3D11_MAPPED_SUBRESOURCE MappedResource = {};

	if (FAILED(m_pContext->Map(m_pTexture, 0, D3D11_MAP_READ, 0, &MappedResource)))
		return _float4(0.f, 0.f, 0.f, 0.f);


	if (nullptr == MappedResource.pData) {
		m_pContext->Unmap(m_pTexture, 0);
		return _float4(0.f, 0.f, 0.f, 0.f);

	}

	_float4 PickPos = ((_float4*)MappedResource.pData)[0];

	m_pContext->Unmap(m_pTexture, 0);

	_float fX = (_float(x) / m_iWinSizeX) * 2.f - 1.f;
	_float fY = (_float(y) / m_iWinSizeY) * -2.f + 1.f;

	_vector vWorldPos = XMVectorSet(fX, fY, PickPos.x, 1.f);

	vWorldPos = vWorldPos * PickPos.y * m_pGameInstance->Get_CameraNF().y;

	//float fRadius = 5.f;

	//float FinalRadius = fRadius / (PickPos.y * m_pGameInstance->Get_CameraNF().y);

	vWorldPos = XMVector4Transform(vWorldPos, m_pGameInstance->Get_Transform_Inversed(TransformType::Proj));

	vWorldPos = XMVector4Transform(vWorldPos, m_pGameInstance->Get_Transform_Inversed(TransformType::View));

	_float4 WorldPos = {};

	XMStoreFloat4(&WorldPos, vWorldPos);

	return WorldPos;
}

_vec4 CPicking::PickingNormal(_float x, _float y)
{
	ID3D11Texture2D* pTexture = nullptr;

	pTexture = m_pGameInstance->Get_Texture2D(L"Target_Normal_Spec");

	if (nullptr == pTexture) {
		MSG_BOX("Get Target_Depth FAILED");
		return _float4(0.f, 0.f, 0.f, 0.f);
	}

	D3D11_BOX m_Box;
	m_Box.left = static_cast<_uint>(x);
	m_Box.top = static_cast<_uint>(y);
	m_Box.right = static_cast<_uint>(x + 1);
	m_Box.bottom = static_cast<_uint>(y + 1);
	m_Box.front = 0;
	m_Box.back = 1;

	m_pContext->CopySubresourceRegion(m_pTexture, 0, 0, 0, 0, pTexture, 0, &m_Box);

	D3D11_MAPPED_SUBRESOURCE MappedResource = {};

	if (FAILED(m_pContext->Map(m_pTexture, 0, D3D11_MAP_READ, 0, &MappedResource)))
		return _float4(0.f, 0.f, 0.f, 0.f);


	if (nullptr == MappedResource.pData) {
		m_pContext->Unmap(m_pTexture, 0);
		return _float4(0.f, 0.f, 0.f, 0.f);

	}

	_vec4 PickPos = ((_vec4*)MappedResource.pData)[0];

	m_pContext->Unmap(m_pTexture, 0);

	PickPos.w = 0.f;

	PickPos.Normalize();
	return PickPos;
}


_int CPicking::FastPicking(_uint x, _uint y)
{
	ID3D11Texture2D* pTexture = nullptr;

	pTexture = m_pGameInstance->Get_Texture2D(L"Target_ID");

	if (nullptr == pTexture) {
		MSG_BOX("Get Target_ID FAILED");
		return 0;
	}

	D3D11_BOX m_Box;
	m_Box.left = x;
	m_Box.top = y;
	m_Box.right = x + 1;
	m_Box.bottom = y + 1;
	m_Box.front = 0;
	m_Box.back = 1;

	m_pContext->CopySubresourceRegion(m_pFastTexture, 0, 0, 0, 0, pTexture, 0, &m_Box);

	D3D11_MAPPED_SUBRESOURCE MappedResource = {};

	if (FAILED(m_pContext->Map(m_pFastTexture, 0, D3D11_MAP_READ, 0, &MappedResource)))
		return 0;


	if (nullptr == MappedResource.pData) {
		m_pContext->Unmap(m_pFastTexture, 0);
		return 0;

	}

	_int PickPos = ((_int*)MappedResource.pData)[0];

	m_pContext->Unmap(m_pFastTexture, 0);

	return PickPos;
}

HRESULT CPicking::Ready_Texture2D()
{
	D3D11_TEXTURE2D_DESC Desc{};
	Desc.Width = 1;
	Desc.Height = 1;
	Desc.MipLevels = 1;
	Desc.ArraySize = 1;
	Desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	Desc.SampleDesc.Quality = 0;
	Desc.SampleDesc.Count = 1;

	Desc.Usage = D3D11_USAGE_STAGING;
	Desc.BindFlags = 0;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	Desc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&Desc, nullptr, &m_pTexture)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPicking::Ready_FastPicking()
{
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_ID"), m_iWinSizeX, m_iWinSizeY, DXGI_FORMAT_R32_SINT, _float4(0.f,0.f,0.f,0.f))))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_ID"))))
	{
		return E_FAIL;
	}

	D3D11_TEXTURE2D_DESC Desc{};
	Desc.Width = 1;
	Desc.Height = 1;
	Desc.MipLevels = 1;
	Desc.ArraySize = 1;
	Desc.Format = DXGI_FORMAT_R32_SINT;

	Desc.SampleDesc.Quality = 0;
	Desc.SampleDesc.Count = 1;

	Desc.Usage = D3D11_USAGE_STAGING;
	Desc.BindFlags = 0;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	Desc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&Desc, nullptr, &m_pFastTexture)))
	{
		return E_FAIL;
	}

	return S_OK;
}


_bool CPicking::Picking_InLocal(_vec4 vPoint1, _vec4 vPoint2, _vec4 vPoint3, _Inout_ _vec4* pPickPos)
{
	_float fDist{};
	_vector vRayPos{ XMLoadFloat4(&m_vRayPos_Local) };
	_vector vRayDir{ XMLoadFloat4(&m_vRayDir_Local) };
	if (TriangleTests::Intersects(vRayPos, vRayDir, vPoint1, vPoint2, vPoint3, fDist))
	{
		XMStoreFloat4(pPickPos, vRayPos + (vRayDir * fDist));
		return true;
	}
	else
	{
		return false;
	}
}

_bool CPicking::Picking_InLocal(_vec4 vPoint1, _vec4 vPoint2, _vec4 vPoint3, _vec4 vNormal, _Inout_ _vec4* pPickPos)
{
	if (XMVector4Dot(vNormal, XMLoadFloat4(&m_vRayDir_Local)).m128_f32[0] > 0)
	{
		return false;
	}

	_float fDist{};
	_vector vRayPos{ XMLoadFloat4(&m_vRayPos_Local) };
	_vector vRayDir{ XMLoadFloat4(&m_vRayDir_Local) };
	if (TriangleTests::Intersects(vRayPos, vRayDir, vPoint1, vPoint2, vPoint3, fDist))
	{
		XMStoreFloat4(pPickPos, vRayPos + (vRayDir * fDist));
		return true;
	}
	else
	{
		return false;
	}
}


_vec4 CPicking::Compute_MousePicked_Terrain(_float44 matTerrainWorld, _float3* pVerticesPos, _uint iNumVerticesX, _uint iNumVerticesZ)
{
	//Transform_To_LocalSpace(matTerrainWorld);

	_uint	iNumIndices = 0;
	_vec3 vPickPos = { 0.f, 0.f, 0.f};
	_bool	bCheck = false;

	for (size_t i = 0; i < iNumVerticesZ - 1; ++i)
	{
		for (size_t j = 0; j < iNumVerticesX - 1; ++j)
		{
			_uint	iIndex = (_uint)i * iNumVerticesX + (_uint)j;
			_uint	iIndices[4] = {
				iIndex + iNumVerticesX,			// 좌측상단
				iIndex + iNumVerticesX + 1,		// 우측상단
				iIndex + 1,						// 우측하단
				iIndex							// 좌측하단
			};

			// 로컬스페이스에서 충돌이 이뤄지고 있다.
			_vector vPointA = XMLoadFloat3(&pVerticesPos[iIndices[0]]);
			_vector vPointB = XMLoadFloat3(&pVerticesPos[iIndices[1]]);
			_vector vPointC = XMLoadFloat3(&pVerticesPos[iIndices[2]]);
			_vector vPointD = XMLoadFloat3(&pVerticesPos[iIndices[3]]);

			// 오른쪽 위 삼각형
			if (true == Picking_InWorld(vPointA, vPointB, vPointC, &vPickPos))
			{
				bCheck = true;
				break;
			}

			// 왼쪽 아래 삼각형
			if (true == Picking_InWorld(vPointA, vPointC, vPointD, &vPickPos))
			{
				bCheck = true;
				break;
			}
		}
		if (bCheck == true)
			break;
	}
	
	// 
	// 로컬상에서의 충돌된 위치를 월드로 변환한다.
	_vector vResult = XMVectorSet(-100.f, -100.f, -100.f, 0.f);

	if (bCheck)
	{
		vResult = XMVectorSet(vPickPos.x, vPickPos.y, vPickPos.z, 1.f);
		//XMVector3TransformCoord(vResult, XMLoadFloat4x4(&matTerrainWorld));
	}

	//XMStoreFloat4(&m_vMousePos, vResult);
	return vResult;
}

_vec4 CPicking::Compute_MousePicked_MeshTerrain(_float44 matTerrainWorld, vector<VTXNORTEX> vVertices, vector<_ulong> vIndices)
{
	//Transform_To_LocalSpace(matTerrainWorld);

	_uint	iNumIndices = 0;
	_vec3 vPickPos = { 0.f, 0.f, 0.f };
	_bool	bCheck = false;
	

	for(_uint i = 0; i < vIndices.size(); i++)
	{
		// 로컬스페이스에서 충돌이 이뤄지고 있다.
		_vector vPointA = XMLoadFloat3(&vVertices[vIndices[iNumIndices++]].vPosition);
		_vector vPointB = XMLoadFloat3(&vVertices[vIndices[iNumIndices++]].vPosition);
		_vector vPointC = XMLoadFloat3(&vVertices[vIndices[iNumIndices++]].vPosition);
		_vector vPointD = XMLoadFloat3(&vVertices[vIndices[iNumIndices++]].vPosition);


		// 오른쪽 위 삼각형
		if (true == Picking_InWorld(vPointA, vPointB, vPointC, &vPickPos))
		{
			bCheck = true;
			break;
		}

		// 왼쪽 아래 삼각형
		if (true == Picking_InWorld(vPointA, vPointC, vPointD, &vPickPos))
		{
			bCheck = true;
			break;
		}
	}


	// 
	// 로컬상에서의 충돌된 위치를 월드로 변환한다.
	_vector vResult = XMVectorSet(-100.f, -100.f, -100.f, 0.f);

	if (bCheck)
	{
		vResult = XMVectorSet(vPickPos.x, vPickPos.y, vPickPos.z, 1.f);
		//XMVector3TransformCoord(vResult, XMLoadFloat4x4(&matTerrainWorld));
	}

	//XMStoreFloat4(&m_vMousePos, vResult);
	return vResult;
}


CPicking* CPicking::Create(_dev pDevice, _context pContext, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
	return new CPicking(pDevice, pContext, hWnd, iWinSizeX, iWinSizeY);
}

void CPicking::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pTexture);
	Safe_Release(m_pFastTexture);
}
