#pragma once
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Terrain_Dynamic.h"
#include "Transform.h"
#include "Model.h"
#include "VIBuffer_Instancing_Point.h"
#include "VIBuffer_Instancing_Mesh.h"
#include "VIBuffer_Curve.h"
#include "Collider.h"
#include "VIBuffer_Trail.h"
#include "VIBuffer_Trail_Surface.h"
#include "VIBuffer_Mesh_Instance.h"
#include "VTFModel.h"
#include "RealtimeVTFModel.h"
#include "VIBuffer_Rect_LookZ.h"

BEGIN(Engine)

class CComponent_Manager final : public CBase
{
private:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	HRESULT Init(_uint iNumLevels, _dev pDevice, _context pContext);
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& strPrototype, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg);
	void Clear(_uint iLevelIndex);

private:
	using PROTOTYPES = map<const wstring, class CComponent*>;

	PROTOTYPES* m_pPrototypes{ nullptr };
	_uint m_iNumLevels{};
	
private:
	class CComponent* Find_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag);

public:
	static CComponent_Manager* Create(_uint iNumLevels, _dev pDevice, _context pContext);
	virtual void Free() override;
};

END