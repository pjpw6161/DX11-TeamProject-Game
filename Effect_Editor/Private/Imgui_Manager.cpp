#include "Imgui_Manager.h"
#include "Camera.h"
#include "Effect_Dummy.h"
#include <commdlg.h>
#include "Engine_Defines.h"

IMPLEMENT_SINGLETON(CImgui_Manager)

using namespace ImGui;

CImgui_Manager::CImgui_Manager()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

const _bool& CImgui_Manager::Has_Light()
{
	return m_hasLight;
}

const _color& CImgui_Manager::Get_BGColor()
{
	return m_vBGColor;
}

HRESULT CImgui_Manager::Init(_dev pDevice, _context pContext, vector<string>* pTextureList, vector<string>* pModelList)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pDevice);

	IMGUI_CHECKVERSION();
	CreateContext();

	StyleColorsClassic();
	//StyleColorsDark();

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);

	m_iNumTextures = static_cast<_int>(pTextureList->size());
	m_pItemList_Texture = new const _char * [m_iNumTextures];
	m_pTextures = new CTexture * [m_iNumTextures];

	for (_int i = 0; i < m_iNumTextures; i++)
	{
		m_pItemList_Texture[i] = (*pTextureList)[i].c_str();
	}

	m_iNumModels = static_cast<_int>(pModelList->size());
	m_pItemList_Model = new const _char * [m_iNumModels];
	m_pModels = new CModel * [m_iNumModels];

	for (_int i = 0; i < m_iNumModels; i++)
	{
		m_pItemList_Model[i] = (*pModelList)[i].c_str();
	}

	if (FAILED(Ready_Layers()))
	{
		return E_FAIL;
	}

	m_ParticleInfo.vMinPos = _float3(0.f, 0.f, 0.f);
	m_ParticleInfo.vMaxPos = _float3(0.f, 0.f, 0.f);
	m_ParticleInfo.vScaleRange = _float2(1.f, 1.f);
	m_ParticleInfo.vSpeedRange = _float2(1.f, 1.f);
	m_ParticleInfo.vLifeTime = _float2(1.f, 1.f);
	m_ParticleInfo.vMinDir = _float3(0.f, 1.f, 0.f);
	m_ParticleInfo.vMaxDir = _float3(0.f, 1.f, 0.f);
	m_ParticleInfo.isLoop = true;

	CGameInstance::Func_TickFX func_Tick = [this](auto... args) { return Effect_Tick(args...); };
	CGameInstance::Func_TickFX func_LateTick = [this](auto... args) { return Effect_LateTick(args...); };
	m_pGameInstance->Register_Tick_LateTick_Callback(func_Tick, func_LateTick);

	//Load_OldData();

	return S_OK;
}

void CImgui_Manager::Tick(_float fTimeDelta)
{
	static _bool bDemo{ true };
	ShowDemoWindow(&bDemo);
	EffectInfo Info{};
	static _bool shouldScrollToSelectedItem{};
#pragma region Main Window
	Begin("Effect_Tool");

	const _char* szType[ET_END]{ "Particle", "Rect", "Mesh", "Distortion", "Particle_Distortion"};

	if (Combo("Type", &m_iCurrent_Type, szType, IM_ARRAYSIZE(szType)))
	{

	}

	static _bool hasLifeTime{};
	Checkbox("Limited Life Time", &hasLifeTime);

	if (hasLifeTime)
	{
		InputFloat("Effect Life", &m_fEffectLifeTime);
		Info.fLifeTime = m_fEffectLifeTime;
	}
	else
	{
		m_fEffectLifeTime = -1.f;
		Info.fLifeTime = m_fEffectLifeTime;
	}

	NewLine();
	SeparatorText("Position Offset");
	static _vec3 vPosOffset{};
	InputFloat3("Offset", reinterpret_cast<_float*>(&vPosOffset), "%.2f");
	Info.vPosOffset = vPosOffset;
	NewLine();

#pragma region Pass
	static _int iPassIndex{};

	const _char* szInstancingPasses[InstPass_End]
	{
		"TextureMask",
		"Sprite_Color",
		"MaskColor",
		"TextureMask_Dissolve",
		"Sprite_Color_Dissolve",
		"MaskColor_Dissolve",
		"Sprite_Texture",
		"Sprite_Texture_Dissolve",
		"Sprite_Texture_RandomIndex",
		"Sprite_Texture_RandomIndex_Dissolve",
		"Sprite_Color_RandomIndex",
		"Sprite_Color_RandomIndex_Dissolve",
		"TextureMask_Trail",
		"Sprite_Color_Trail",
		"MaskColor_Trail",
		"TextureMask_Dissolve_Trail",
		"Sprite_Color_Dissolve_Trail",
		"MaskColor_Dissolve_Trail",
		"Sprite_Texture_Trail",
		"Sprite_Texture_Dissolve_Trail",
		"Sprite_Texture_RandomIndex_Trail",
		"Sprite_Texture_RandomIndex_Dissolve_Trail",
		"Sprite_Color_RandomIndex_Trail",
		"Sprite_Color_RandomIndex_Dissolve_Trail",
		"Sprite_DiffMask",
		"Sprite_DiffMask_Dissolve",
		"Sprite_DiffMask_RandomIndex",
		"Sprite_DiffMask_RandomIndex_Dissolve",
		"Sprite_DiffMask_Trail",
		"Sprite_DiffMask_Dissolve_Trail",
		"Sprite_DiffMask_RandomIndex_Trail",
		"Sprite_DiffMask_RandomIndex_Dissolve_Trail",
		"MaskColor_Alpha",
		"Distortion",
	};

	const _char* szVTPasses[VTPass_End]
	{
		"UI",
		"UI_Alpha",
		"UI_Color_Alpha",
		"Button",
		"Background",
		"Background_Mask",
		"Mask_Texture",
		"Inv_Mask_Texture",
		"Mask_Color",
		"Mask_ColorAlpha",
		"HP",
		"Hit",
		"Sprite",
		"SpriteMaskTexture",
		"SpriteMaskColor",
		"Hell",
		"Mask_Texture_Dissolve",
		"Inv_Mask_Texture_Dissolve",
		"Mask_Color_Dissolve",
		"Sprite_Dissolve",
		"SpriteMaskTexture_Dissolve",
		"SpriteMaskColor_Dissolve",
		"Dust",
		"Dissolve",
		"MP",
		"BLUR",
		"Bright",
		"ScrollAlpha",
		"HPNoMask",
		"NineSlice",
		"FadeVertical",
		"FadeHorizontal",
	};

	const _char* szStatPasses[StaticPass_End]
	{
		"Default",
		"NonLight",
		"OutLine",
		"AlphaTestMeshes",
		"Diff_Dissolve",
		"COLMesh",
		"SingleColorFx",
		"SingleColorDissolve",
		"Fireball",
		"MaskEffect",
		"MaskDissolve",
		"MaskEffectClamp",
		"SingleColoredEffectFrontCull",
		"Shadow",
		"Water",
		"Worldmap_Water",
		"Worldmap_Cloud",
		"Rim",
		"DiffEffect",
		"SingleColorAlpha",
		"MaskAlpha",
		"DiffAlpha",
		"Distortion",
	};

	SeparatorText("Shader Pass");

	if (m_iCurrent_Type == ET_PARTICLE or m_iCurrent_Type == ET_PARTI_DISTORTION)
	{
		//if (BeginListBox("", ImVec2(-FLT_MIN, 7 * ImGui::GetTextLineHeightWithSpacing())))
		//{
		//	for (size_t i = 0; i < InstPass_End; i++)
		//	{
		//		//ListBox("Pass##1", &iPassIndex, szInstancingPasses, InstPass_End);
		//		const _bool isSelected = iPassIndex == i;
		//		if (Selectable(szInstancingPasses[i], isSelected))
		//		{
		//			iPassIndex = i;
		//		}
		//		if (isSelected)
		//		{
		//			SetItemDefaultFocus();
		//		}
		//	}
		//	if (iPassIndex >= InstPass_End)
		//	{
		//		iPassIndex = 0;
		//	}
		//	EndListBox();
		//}

		Text(szInstancingPasses[iPassIndex]);
		static ImGuiTextFilter Filter_Pass;
		Filter_Pass.Draw("Pass##1"); SameLine();
		if (Button("Scroll##6"))
		{
			shouldScrollToSelectedItem = true;
		}
		ImGui::BeginChild("Instancing Pass", ImVec2(0, 150), true);

		for (int i = 0; i < InstPass_End; ++i)
		{
			if (Filter_Pass.PassFilter(szInstancingPasses[i]))
			{
				_bool isSelected = (i == iPassIndex);

				if (isSelected)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
				}

				if (ImGui::Selectable(szInstancingPasses[i], isSelected))
				{
					iPassIndex = i;
				}

				if (isSelected)
				{
					if (shouldScrollToSelectedItem)
					{
						SetScrollHereY();
					}
					PopStyleColor();
				}
			}
		}
		ImGui::EndChild();
	}
	else if (m_iCurrent_Type == ET_RECT)
	{
		//ListBox("Pass##2", &iPassIndex, szVTPasses, VTPass_End);
		//if (iPassIndex >= VTPass_End)
		//{
		//	iPassIndex = 0;
		//}
		Text(szVTPasses[iPassIndex]);
		static ImGuiTextFilter Filter_Pass;
		Filter_Pass.Draw("Pass##2"); SameLine();
		if (Button("Scroll##7"))
		{
			shouldScrollToSelectedItem = true;
		}
		ImGui::BeginChild("VT Pass", ImVec2(0, 150), true);

		for (int i = 0; i < VTPass_End; ++i)
		{
			if (Filter_Pass.PassFilter(szVTPasses[i]))
			{
				_bool isSelected = (i == iPassIndex);

				if (isSelected)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
				}

				if (ImGui::Selectable(szVTPasses[i], isSelected))
				{
					iPassIndex = i;
				}

				if (isSelected)
				{
					if (shouldScrollToSelectedItem)
					{
						SetScrollHereY();
					}
					PopStyleColor();
				}
			}
		}
		ImGui::EndChild();

	}
	else if (m_iCurrent_Type == ET_MESH or m_iCurrent_Type == ET_DISTORTION)
	{
		//ListBox("Pass##3", &iPassIndex, szStatPasses, StaticPass_End);
		//if (iPassIndex >= StaticPass_End)
		//{
		//	iPassIndex = 0;
		//}
		Text(szStatPasses[iPassIndex]);
		static ImGuiTextFilter Filter_Pass;
		Filter_Pass.Draw("Pass##3"); SameLine();
		if (Button("Scroll##8"))
		{
			shouldScrollToSelectedItem = true;
		}
		ImGui::BeginChild("StatMesh Pass", ImVec2(0, 150), true);

		for (int i = 0; i < StaticPass_End; ++i)
		{
			if (Filter_Pass.PassFilter(szStatPasses[i]))
			{
				_bool isSelected = (i == iPassIndex);

				if (isSelected)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
				}

				if (ImGui::Selectable(szStatPasses[i], isSelected))
				{
					iPassIndex = i;
				}

				if (isSelected)
				{
					if (shouldScrollToSelectedItem)
					{
						SetScrollHereY();
					}
					PopStyleColor();
				}
			}
		}
		ImGui::EndChild();
	}
	Info.iType = m_iCurrent_Type;
	Info.iPassIndex = iPassIndex;
#pragma endregion

	Separator();
	NewLine();

	static _bool bSkipBloom{};
	Checkbox("No Bloom", &bSkipBloom);
	Info.bSkipBloom = bSkipBloom;

	NewLine();

	static _int iIsColor{};
	static _bool hasUnDissolve{};
	static _bool hasDissolve{};

#pragma region Sprite
	static _bool isSprite{};
	static _bool isFixedIndex{};
	static _int2 vNumSprites{ 1, 1 };
	static _float fSpriteDuration{ 1.f };
	static _int iFixedSpriteIndex{};
	Checkbox("Sprite", &isSprite);
	if (isSprite)
	{
		SameLine();
		Checkbox("Fix Index", &isFixedIndex);

		InputInt2("Number of Sprites", reinterpret_cast<_int*>(&vNumSprites));

		if (isFixedIndex)
		{
			InputInt("Index", &iFixedSpriteIndex);
		}
		else
		{
			InputFloat("Duration", &fSpriteDuration, 0.f, 0.f, "%.1f");
		}

		Info.isSprite = true;
		Info.vNumSprites = vNumSprites;
		Info.isFixedIndex = isFixedIndex;
		Info.iFixedSpriteIndex = iFixedSpriteIndex;
		Info.fSpriteDuration = fSpriteDuration;
	}
#pragma endregion

	Separator();
	NewLine();

#pragma region Diffuse
	if (m_iCurrent_Type != ET_DISTORTION and m_iCurrent_Type != ET_PARTI_DISTORTION)
	{
		SeparatorText("Diffuse");
		RadioButton("Texture", &iIsColor, 0); SameLine();
		RadioButton("Color", &iIsColor, 1);
	}
	else
	{
		SeparatorText("Distortion");
		iIsColor = false;
	}

	if (iIsColor)
	{
		NewLine();

		ColorPicker4("Color", reinterpret_cast<_float*>(&m_vColor), ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB);
		m_hasDiffTexture = false;
	}
	else
	{
		if (m_iSelected_Texture < 0)
		{
			m_iSelected_Texture = 0;
		}
		NewLine();

		Text(m_pItemList_Texture[m_iSelected_Texture]);
		static ImGuiTextFilter Filter_Diff;
		Filter_Diff.Draw("Search##1"); SameLine();
		if (Button("Scroll##1"))
		{
			shouldScrollToSelectedItem = true;
		}
		ImGui::BeginChild("Diff Texture", ImVec2(0, 150), true);

		// ListBox의 각 아이템을 생성합니다.
		for (int i = 0; i < m_iNumTextures; ++i)
		{
			if (Filter_Diff.PassFilter(m_pItemList_Texture[i]))
			{
				_bool isSelected = (i == m_iSelected_Texture);

				// 선택된 항목의 경우 색상을 변경합니다.
				if (isSelected)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
				}

				if (ImGui::Selectable(m_pItemList_Texture[i], isSelected))
				{
					m_iSelected_Texture = i;
				}

				if (isSelected)
				{
					if (shouldScrollToSelectedItem)
					{
						SetScrollHereY();
					}
					PopStyleColor();
				}
			}
		}
		ImGui::EndChild();

		//if (ListBox("Texture", &m_iSelected_Texture, m_pItemList_Texture, m_iNumTextures))
		//{
		//	SetScrollHereY();
		//}
		Image(reinterpret_cast<void*>(m_pTextures[m_iSelected_Texture]->Get_SRV()), ImVec2(128.f, 128.f));
		m_hasDiffTexture = true;
	}

	if (m_hasDiffTexture)
	{
		//Info.iDiffTextureID = m_iSelected_Texture;
		_tchar strUnicode[MAX_PATH]{};
		MultiByteToWideChar(CP_ACP, 0, m_pItemList_Texture[m_iSelected_Texture], static_cast<int>(strlen(m_pItemList_Texture[m_iSelected_Texture])), strUnicode, static_cast<int>(strlen(m_pItemList_Texture[m_iSelected_Texture])));
		Info.strDiffuseTexture = strUnicode;
	}
	else
	{
		m_iSelected_Texture = -1;
		Info.strDiffuseTexture = {};
		Info.vColor = m_vColor;
	}
#pragma endregion

	Separator();
	NewLine();

#pragma region Mask Texture
	Checkbox("Mask", &m_hasMask);

	static _vec2 vUVInit{};
	static _vec2 vUVDelta{};
	static _bool isUVLoop{};
	static _float fAlphaDelta{};
	static _float fAlphaInit{};
	if (m_hasMask)
	{
		if (m_iSelected_MaskTexture < 0)
		{
			m_iSelected_MaskTexture = 0;
		}
		SameLine();
		SeparatorText("Mask Texture");

		InputFloat("Alpha Init", &fAlphaInit);
		Info.fAlphaInit = fAlphaInit;

		InputFloat("Alpha Delta", &fAlphaDelta);
		Info.fAlphaDelta = fAlphaDelta;

		if (m_iCurrent_Type == ET_MESH or m_iCurrent_Type == ET_DISTORTION)
		{
			InputFloat2("UV Initializer", reinterpret_cast<_float*>(&vUVInit));
			Info.vUVInit = vUVInit;
			InputFloat2("UV Delta", reinterpret_cast<_float*>(&vUVDelta));
			Info.vUVDelta = vUVDelta;
			Checkbox("UV Loop", &isUVLoop);
			Info.isUVLoop = isUVLoop;
		}
		Text(m_pItemList_Texture[m_iSelected_MaskTexture]);

		static ImGuiTextFilter Filter_Mask;
		Filter_Mask.Draw("Search##2"); SameLine();
		if (Button("Scroll##2"))
		{
			shouldScrollToSelectedItem = true;
		}
		ImGui::BeginChild("Mask Texture", ImVec2(0, 150), true);

		for (int i = 0; i < m_iNumTextures; ++i)
		{
			if (Filter_Mask.PassFilter(m_pItemList_Texture[i]))
			{
				_bool isSelected = (i == m_iSelected_MaskTexture);

				if (isSelected)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
				}

				if (ImGui::Selectable(m_pItemList_Texture[i], isSelected))
				{
					m_iSelected_MaskTexture = i;
				}

				if (isSelected)
				{
					if (shouldScrollToSelectedItem)
					{
						SetScrollHereY();
					}
					PopStyleColor();
				}
			}
		}

		ImGui::EndChild();

		//ListBox("Mask Texture", &m_iSelected_MaskTexture, m_pItemList_Texture, m_iNumTextures);
		Image(reinterpret_cast<void*>(m_pTextures[m_iSelected_MaskTexture]->Get_SRV()), ImVec2(128.f, 128.f));

		//Info.iMaskTextureID = m_iSelected_MaskTexture;
		_tchar strUnicode[MAX_PATH]{};
		MultiByteToWideChar(CP_ACP, 0, m_pItemList_Texture[m_iSelected_MaskTexture], static_cast<int>(strlen(m_pItemList_Texture[m_iSelected_MaskTexture])), strUnicode, static_cast<int>(strlen(m_pItemList_Texture[m_iSelected_MaskTexture])));
		Info.strMaskTexture = strUnicode;
	}
	else
	{
		m_iSelected_MaskTexture = -1;
		Info.strMaskTexture = {};
	}
#pragma endregion

	Separator();
	NewLine();

#pragma region UnDissolve Texture
	Checkbox("UnDissolve", &hasUnDissolve);

	static _int iSelectd_UnDissolve{};
	static _float fUnDissolveDuration{};
	if (hasUnDissolve)
	{
		SameLine();
		SeparatorText("Texture");
		InputFloat("UnDissove Duration", &fUnDissolveDuration);
		Info.fUnDissolveDuration = fUnDissolveDuration;
		if (iSelectd_UnDissolve < 0)
		{
			iSelectd_UnDissolve = 0;
		}
		Text(m_pItemList_Texture[iSelectd_UnDissolve]);

		static ImGuiTextFilter Filter_UnDiss;
		Filter_UnDiss.Draw("Search##3"); SameLine();
		if (Button("Scroll##3"))
		{
			shouldScrollToSelectedItem = true;
		}
		ImGui::BeginChild("UnDissolve Texture", ImVec2(0, 150), true);

		for (int i = 0; i < m_iNumTextures; ++i)
		{
			if (Filter_UnDiss.PassFilter(m_pItemList_Texture[i]))
			{
				_bool isSelected = (i == iSelectd_UnDissolve);

				if (isSelected)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
				}

				if (ImGui::Selectable(m_pItemList_Texture[i], isSelected))
				{
					iSelectd_UnDissolve = i;
				}

				if (isSelected)
				{
					if (shouldScrollToSelectedItem)
					{
						SetScrollHereY();
					}
					PopStyleColor();
				}
			}
		}

		ImGui::EndChild();

		//ListBox("UnDissolve Texture", &iSelectd_UnDissolve, m_pItemList_Texture, m_iNumTextures);
		Image(reinterpret_cast<void*>(m_pTextures[iSelectd_UnDissolve]->Get_SRV()), ImVec2(128.f, 128.f));

		//Info.iUnDissolveTextureID = iSelectd_UnDissolve;
		_tchar strUnicode[MAX_PATH]{};
		MultiByteToWideChar(CP_ACP, 0, m_pItemList_Texture[iSelectd_UnDissolve], static_cast<int>(strlen(m_pItemList_Texture[iSelectd_UnDissolve])), strUnicode, static_cast<int>(strlen(m_pItemList_Texture[iSelectd_UnDissolve])));
		Info.strUnDissolveTexture = strUnicode;
	}
	else
	{
		iSelectd_UnDissolve = -1;
		Info.strUnDissolveTexture = {};
	}
#pragma endregion

	Separator();
	NewLine();

#pragma region Dissolve Texture
	Checkbox("Dissolve", &hasDissolve);

	static _int iSelectd_Dissolve{};
	static _float fDissolveDuration{};
	if (hasDissolve)
	{
		SameLine();
		SeparatorText("Texture");
		InputFloat("Dissove Duration", &fDissolveDuration);
		Info.fDissolveDuration = fDissolveDuration;
		if (iSelectd_Dissolve < 0)
		{
			iSelectd_Dissolve = 0;
		}
		Text(m_pItemList_Texture[iSelectd_Dissolve]);

		static ImGuiTextFilter Filter_Diss;
		Filter_Diss.Draw("Search##4"); SameLine();
		if (Button("Scroll##4"))
		{
			shouldScrollToSelectedItem = true;
		}
		ImGui::BeginChild("Dissolve Texture", ImVec2(0, 150), true);

		for (int i = 0; i < m_iNumTextures; ++i)
		{
			if (Filter_Diss.PassFilter(m_pItemList_Texture[i]))
			{
				_bool isSelected = (i == iSelectd_Dissolve);

				if (isSelected)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
				}

				if (ImGui::Selectable(m_pItemList_Texture[i], isSelected))
				{
					iSelectd_Dissolve = i;
				}

				if (isSelected)
				{
					if (shouldScrollToSelectedItem)
					{
						SetScrollHereY();
					}
					PopStyleColor();
				}
			}
		}

		ImGui::EndChild();

		//ListBox("Dissolve Texture", &iSelectd_Dissolve, m_pItemList_Texture, m_iNumTextures);
		Image(reinterpret_cast<void*>(m_pTextures[iSelectd_Dissolve]->Get_SRV()), ImVec2(128.f, 128.f));

		//Info.iDissolveTextureID = iSelectd_Dissolve;
		_tchar strUnicode[MAX_PATH]{};
		MultiByteToWideChar(CP_ACP, 0, m_pItemList_Texture[iSelectd_Dissolve], static_cast<int>(strlen(m_pItemList_Texture[iSelectd_Dissolve])), strUnicode, static_cast<int>(strlen(m_pItemList_Texture[iSelectd_Dissolve])));
		Info.strDissolveTexture = strUnicode;
	}
	else
	{
		iSelectd_Dissolve = -1;
		Info.strDissolveTexture = {};
	}
#pragma endregion

	Separator();
	NewLine();

	static LIGHT_DESC Light_Desc{};
	static _int iAttenuation{};
#pragma region Light
	if (m_iCurrent_Type == ET_DISTORTION or m_iCurrent_Type == ET_PARTI_DISTORTION)
	{
		m_hasLight = true;
		Info.hasLight = m_hasLight;
		Info.Light_Desc.eType = LIGHT_DESC::Point;
		Info.Light_Desc.vDiffuse = _vec4(1.f);
		Info.Light_Desc.vAmbient = _vec4(1.f);
		Info.Light_Desc.vSpecular = _vec4(1.f);
		Info.Light_Desc.vAttenuation = LIGHT_RANGE_3250;
	}
	else
	{
		Checkbox("Has Light", &m_hasLight);
		Separator();
		const _char* szAttenuations[]
		{
			"LIGHT_RANGE_7",
			"LIGHT_RANGE_13",
			"LIGHT_RANGE_20",
			"LIGHT_RANGE_32",
			"LIGHT_RANGE_50",
			"LIGHT_RANGE_65",
			"LIGHT_RANGE_100",
			"LIGHT_RANGE_160",
			"LIGHT_RANGE_200",
			"LIGHT_RANGE_325",
			"LIGHT_RANGE_600",
			"LIGHT_RANGE_3250",
		};
		Light_Desc.eType = LIGHT_DESC::Point;

		Info.hasLight = m_hasLight;
		if (m_hasLight)
		{
			SameLine();
			SeparatorText("Light Description");
			ColorPicker4("Diffuse", reinterpret_cast<_float*>(&Light_Desc.vDiffuse), ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB);
			InputFloat4("Ambient", reinterpret_cast<_float*>(&Light_Desc.vAmbient), "%.2f");
			InputFloat4("Specular", reinterpret_cast<_float*>(&Light_Desc.vSpecular), "%.2f");
			Combo("Attenuation", &iAttenuation, szAttenuations, IM_ARRAYSIZE(szAttenuations));

			switch (iAttenuation)
			{
			case 0:
				Light_Desc.vAttenuation = LIGHT_RANGE_7;
				break;
			case 1:
				Light_Desc.vAttenuation = LIGHT_RANGE_13;
				break;
			case 2:
				Light_Desc.vAttenuation = LIGHT_RANGE_20;
				break;
			case 3:
				Light_Desc.vAttenuation = LIGHT_RANGE_32;
				break;
			case 4:
				Light_Desc.vAttenuation = LIGHT_RANGE_50;
				break;
			case 5:
				Light_Desc.vAttenuation = LIGHT_RANGE_65;
				break;
			case 6:
				Light_Desc.vAttenuation = LIGHT_RANGE_100;
				break;
			case 7:
				Light_Desc.vAttenuation = LIGHT_RANGE_160;
				break;
			case 8:
				Light_Desc.vAttenuation = LIGHT_RANGE_200;
				break;
			case 9:
				Light_Desc.vAttenuation = LIGHT_RANGE_325;
				break;
			case 10:
				Light_Desc.vAttenuation = LIGHT_RANGE_600;
				break;
			case 11:
				Light_Desc.vAttenuation = LIGHT_RANGE_3250;
				break;
			}

			Info.Light_Desc = Light_Desc;
		}

	}
	NewLine();
	NewLine();
#pragma endregion

	End();
#pragma endregion

#pragma region Option
	Begin("Option");

	static _vec3 vSize{ 1.f };
	static _float fSizeforSprite{ 1.f };
	static _vec3 vSizeDelta{};
	static _bool bApplyGravity{};
	static _vec3 vGravityDir{};
	static _float fRectRotationAngle{};
	static _bool isBillboard{};
	static _vec3 vBillboardRotation{};
	static _float fPartiAppearRatio{};
	static _float fPartiDissolveRatio{ 0.8f };

	if (m_iCurrent_Type == ET_PARTICLE or m_iCurrent_Type == ET_PARTI_DISTORTION)
	{
		SeparatorText("Particle Information");
		Checkbox("Loop", &m_ParticleInfo.isLoop);

		InputInt("Instance Number", &m_iNumInstance);
		m_iNumInstance = clamp(m_iNumInstance, 0, 1024);

		InputFloat3("Min Pos", reinterpret_cast<_float*>(&m_ParticleInfo.vMinPos));
		if (m_ParticleInfo.vMinPos.x > m_ParticleInfo.vMaxPos.x)
		{
			m_ParticleInfo.vMaxPos.x = m_ParticleInfo.vMinPos.x;
		}
		if (m_ParticleInfo.vMinPos.y > m_ParticleInfo.vMaxPos.y)
		{
			m_ParticleInfo.vMaxPos.y = m_ParticleInfo.vMinPos.y;
		}
		if (m_ParticleInfo.vMinPos.z > m_ParticleInfo.vMaxPos.z)
		{
			m_ParticleInfo.vMaxPos.z = m_ParticleInfo.vMinPos.z;
		}
		InputFloat3("Max Pos", reinterpret_cast<_float*>(&m_ParticleInfo.vMaxPos)); SameLine(); if (Button("Same with Min")) { m_ParticleInfo.vMaxPos = m_ParticleInfo.vMinPos; }
		if (m_ParticleInfo.vMinPos.x > m_ParticleInfo.vMaxPos.x)
		{
			m_ParticleInfo.vMinPos.x = m_ParticleInfo.vMaxPos.x;
		}
		if (m_ParticleInfo.vMinPos.y > m_ParticleInfo.vMaxPos.y)
		{
			m_ParticleInfo.vMinPos.y = m_ParticleInfo.vMaxPos.y;
		}
		if (m_ParticleInfo.vMinPos.z > m_ParticleInfo.vMaxPos.z)
		{
			m_ParticleInfo.vMinPos.z = m_ParticleInfo.vMaxPos.z;
		}

		InputFloat3("Min Dir", reinterpret_cast<_float*>(&m_ParticleInfo.vMinDir));
		if (m_ParticleInfo.vMinDir.x > m_ParticleInfo.vMaxDir.x)
		{
			m_ParticleInfo.vMaxDir.x = m_ParticleInfo.vMinDir.x;
		}
		if (m_ParticleInfo.vMinDir.y > m_ParticleInfo.vMaxDir.y)
		{
			m_ParticleInfo.vMaxDir.y = m_ParticleInfo.vMinDir.y;
		}
		if (m_ParticleInfo.vMinDir.z > m_ParticleInfo.vMaxDir.z)
		{
			m_ParticleInfo.vMaxDir.z = m_ParticleInfo.vMinDir.z;
		}
		InputFloat3("Max Dir", reinterpret_cast<_float*>(&m_ParticleInfo.vMaxDir)); SameLine(); if (Button("Same with Min##1")) { m_ParticleInfo.vMaxDir = m_ParticleInfo.vMinDir; }
		if (m_ParticleInfo.vMinDir.x > m_ParticleInfo.vMaxDir.x)
		{
			m_ParticleInfo.vMinDir.x = m_ParticleInfo.vMaxDir.x;
		}
		if (m_ParticleInfo.vMinDir.y > m_ParticleInfo.vMaxDir.y)
		{
			m_ParticleInfo.vMinDir.y = m_ParticleInfo.vMaxDir.y;
		}
		if (m_ParticleInfo.vMinDir.z > m_ParticleInfo.vMaxDir.z)
		{
			m_ParticleInfo.vMinDir.z = m_ParticleInfo.vMaxDir.z;
		}

		InputFloat2("LifeTime", reinterpret_cast<_float*>(&m_ParticleInfo.vLifeTime));
		if (m_ParticleInfo.vLifeTime.x > m_ParticleInfo.vLifeTime.y)
		{
			m_ParticleInfo.vLifeTime.y = m_ParticleInfo.vLifeTime.x;
		}

		InputFloat2("Scale", reinterpret_cast<_float*>(&m_ParticleInfo.vScaleRange));
		if (m_ParticleInfo.vScaleRange.x > m_ParticleInfo.vScaleRange.y)
		{
			m_ParticleInfo.vScaleRange.y = m_ParticleInfo.vScaleRange.x;
		}

		InputFloat2("Speed", reinterpret_cast<_float*>(&m_ParticleInfo.vSpeedRange));
		if (m_ParticleInfo.vSpeedRange.x > m_ParticleInfo.vSpeedRange.y)
		{
			m_ParticleInfo.vSpeedRange.y = m_ParticleInfo.vSpeedRange.x;
		}

		Checkbox("Gravity", &bApplyGravity);

		if (bApplyGravity)
		{
			InputFloat3("Gravity Dir", reinterpret_cast<_float*>(&vGravityDir), "%.2f");
		}

		NewLine();
		if (SliderFloat("Appearance", &fPartiAppearRatio, 0.f, 1.f, "Ratio: %.2f"))
		{
			if (fPartiDissolveRatio < fPartiAppearRatio)
			{
				fPartiDissolveRatio = fPartiAppearRatio;
			}
		}
		if (SliderFloat("Dissolve", &fPartiDissolveRatio, 0.f, 1.f, "Ratio: %.2f"))
		{
			if (fPartiDissolveRatio < fPartiAppearRatio)
			{
				fPartiAppearRatio = fPartiDissolveRatio;
			}
		}

		Info.fPartiAppearRatio = fPartiAppearRatio;
		Info.fPartiDissolveRatio = fPartiDissolveRatio;
		Info.iNumInstances = m_iNumInstance;
		Info.PartiDesc = m_ParticleInfo;
		Info.vSize = _vec3(1.f);
		Info.bApplyGravity = bApplyGravity;
		Info.vGravityDir = vGravityDir;

		Info.strModel = {};
	}
	else if (m_iCurrent_Type == ET_RECT)
	{
		SeparatorText("Rect Information");

		Checkbox("Billboard##1", &isBillboard);
		Info.isBillboard = isBillboard;

		//if (Info.isSprite)
		//{
		//	InputFloat("Size##1", &fSizeforSprite);

		//	Info.vSize.y = fSizeforSprite;
		//	_float fSizeRatio = 1.f / (Info.vNumSprites.x / Info.vNumSprites.y);
		//	Info.vSize.x = fSizeforSprite * fSizeRatio;
		//}
		//else
		{
			InputFloat2("Size##2", reinterpret_cast<_float*>(&vSize), "%.2f");
			vSize.z = 1.f;
			Info.vSize = vSize;
		}

		InputFloat2("Size Delta", reinterpret_cast<_float*>(&vSizeDelta), "%.2f");
		Info.vSizeDelta = vSizeDelta;

		NewLine();
		Text("Rotation");
		SliderAngle(" ", &fRectRotationAngle); SameLine();
		if (Button("Reset"))
		{
			fRectRotationAngle = 0.f;
		}
		InputFloat(" ##2", &fRectRotationAngle, 45.f, 0.f, "%.2f");
		Info.fRectRotationAngle = fRectRotationAngle;

		Info.strModel = {};
	}
	else if (m_iCurrent_Type == ET_MESH or m_iCurrent_Type == ET_DISTORTION)
	{
		SeparatorText("Mesh Information");

		Checkbox("Billboard", &isBillboard);
		Info.isBillboard = isBillboard;
		if (isBillboard)
		{
			Text("Rotation");
			SliderAngle("X", &vBillboardRotation.x); SameLine();
			if (Button("Reset##1"))
			{
				vBillboardRotation.x = 0.f;
			}
			InputFloat("##3", &vBillboardRotation.x, 45.f, 0.f, "%.2f");
			Info.vBillboardRotation.x = vBillboardRotation.x;
			NewLine();
			SliderAngle("Y", &vBillboardRotation.y); SameLine();
			if (Button("Reset##2"))
			{
				vBillboardRotation.y = 0.f;
			}
			InputFloat("##4", &vBillboardRotation.y, 45.f, 0.f, "%.2f");
			Info.vBillboardRotation.y = vBillboardRotation.y;
			NewLine();
			SliderAngle("Z", &vBillboardRotation.z); SameLine();
			if (Button("Reset##3"))
			{
				vBillboardRotation.z = 0.f;
			}
			InputFloat("##5", &vBillboardRotation.z, 45.f, 0.f, "%.2f");
			Info.vBillboardRotation.z = vBillboardRotation.z;
		}

		Separator();
		NewLine();
		InputFloat3("Size##2", reinterpret_cast<_float*>(&vSize), "%.2f");
		Info.vSize = vSize;

		InputFloat3("Size Delta", reinterpret_cast<_float*>(&vSizeDelta), "%.2f");
		Info.vSizeDelta = vSizeDelta;

		static ImGuiTextFilter Filter_Diss;
		Filter_Diss.Draw("Search##5"); SameLine();
		if (Button("Scroll##5"))
		{
			shouldScrollToSelectedItem = true;
		}
		ImGui::BeginChild("Model", ImVec2(0, 250), true);

		for (int i = 0; i < m_iNumModels; ++i)
		{
			if (Filter_Diss.PassFilter(m_pItemList_Model[i]))
			{
				_bool isSelected = (i == m_iSelected_Model);

				if (isSelected)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
				}

				if (ImGui::Selectable(m_pItemList_Model[i], isSelected))
				{
					m_iSelected_Model = i;
				}

				if (isSelected)
				{
					if (shouldScrollToSelectedItem)
					{
						SetScrollHereY();
					}
					PopStyleColor();
				}
			}
		}
		ImGui::EndChild();
		//ListBox("Model", &m_iSelected_Model, m_pItemList_Model, m_iNumModels);

		//Info.iModelIndex = m_iSelected_Model;
		Info.strModel = m_pItemList_Model[m_iSelected_Model];
	}

	End();
#pragma endregion

#pragma region Dummy Move
	Begin("Dummy Movement");

	static _bool isDummyMove{};
	static _float fRadius{ 1.f };
	static _float fRotationPerSec{ 90.f };

	Checkbox("Move Dummy", &isDummyMove);

	if (isDummyMove)
	{
		InputFloat("Radius", &fRadius);
		InputFloat("Speed", &fRotationPerSec);

		m_DummyMatrix *= _mat::CreateRotationY(XMConvertToRadians(fRotationPerSec * fTimeDelta));
	}
	else
	{
		m_DummyMatrix = _mat::Identity;
	}

	End();

#pragma endregion

	shouldScrollToSelectedItem = false;

#pragma region Files
	Begin("Files");

	Text(m_CurrFilePath.stem().string().c_str());

	if (Button("Export"))
	{
		Export_Data(Info);
	} SameLine();

	if (Button("Load"))
	{
		EffectInfo Info = Load_Data();

		Safe_Release(m_pEffect);
		if (Info.vSize.z == 0.f)
		{
			Info.vSize.z = 1.f;
		}
		Info.pMatrix = &m_DummyMatrix;
		m_pEffect = dynamic_cast<CEffect_Dummy*>(m_pGameInstance->Clone_Object(L"Prototype_GameObject_Dummy", &Info));

		m_iCurrent_Type = Info.iType;

		if (Info.fLifeTime < 0)
		{
			hasLifeTime = false;
			m_fEffectLifeTime = -1.f;
		}
		else
		{
			hasLifeTime = true;
			m_fEffectLifeTime = Info.fLifeTime;
		}

		vPosOffset = Info.vPosOffset;

		iPassIndex = Info.iPassIndex;

		bSkipBloom = Info.bSkipBloom;

		if (Info.strDiffuseTexture.size())
		{
			iIsColor = 0;
			m_hasDiffTexture = true;
			m_iSelected_Texture = Compute_TextureIndex(Info.strDiffuseTexture);
		}
		else
		{
			iIsColor = 1;
			m_hasDiffTexture = false;
			m_vColor = Info.vColor;
		}

		isSprite = Info.isSprite;
		if (Info.isSprite)
		{
			vNumSprites = Info.vNumSprites;
			isFixedIndex = Info.isFixedIndex;
			if (isFixedIndex)
			{
				iFixedSpriteIndex = Info.iFixedSpriteIndex;
			}
			else
			{
				fSpriteDuration = Info.fSpriteDuration;
			}
		}

		if (Info.strMaskTexture.size())
		{
			m_hasMask = true;
			m_iSelected_MaskTexture = Compute_TextureIndex(Info.strMaskTexture);
			vUVDelta = Info.vUVDelta;
			vUVInit = Info.vUVInit;
			isUVLoop = Info.isUVLoop;
			fAlphaInit = Info.fAlphaInit;
			fAlphaDelta = Info.fAlphaDelta;
		}
		else
		{
			m_hasMask = false;
		}

		if (Info.strUnDissolveTexture.size())
		{
			hasUnDissolve = true;
			iSelectd_UnDissolve = Compute_TextureIndex(Info.strUnDissolveTexture);
			fUnDissolveDuration = Info.fUnDissolveDuration;
		}
		else
		{
			hasUnDissolve = false;
		}

		if (Info.strDissolveTexture.size())
		{
			hasDissolve = true;
			iSelectd_Dissolve = Compute_TextureIndex(Info.strDissolveTexture);
			fDissolveDuration = Info.fDissolveDuration;
		}
		else
		{
			hasDissolve = false;
		}

		switch (Info.iType)
		{
		case Engine::ET_PARTICLE:
		case Engine::ET_PARTI_DISTORTION:
			m_ParticleInfo = Info.PartiDesc;
			m_iNumInstance = Info.iNumInstances;
			bApplyGravity = Info.bApplyGravity;
			vGravityDir = Info.vGravityDir;
			fPartiDissolveRatio = Info.fPartiDissolveRatio;
			fPartiAppearRatio = Info.fPartiAppearRatio;
			break;
		case Engine::ET_RECT:
			isBillboard = Info.isBillboard;
			if (Info.isSprite)
			{
				fSizeforSprite = Info.vSize.y;
			}
			vSize = Info.vSize;
			if (vSize.z == 0.f)
			{
				vSize.z = 1.f;
			}
			vSizeDelta = Info.vSizeDelta;
			fRectRotationAngle = Info.fRectRotationAngle;
			break;
		case Engine::ET_MESH:
		case Engine::ET_DISTORTION:
			isBillboard = Info.isBillboard;
			if (isBillboard)
			{
				vBillboardRotation = Info.vBillboardRotation;
			}
			m_iSelected_Model = Compute_ModelIndex(Info.strModel);
			vSize = Info.vSize;
			vSizeDelta = Info.vSizeDelta;
			break;
		}

		m_hasLight = Info.hasLight;

		//"LIGHT_RANGE_7",
		//"LIGHT_RANGE_13",
		//"LIGHT_RANGE_20",
		//"LIGHT_RANGE_32",
		//"LIGHT_RANGE_50",
		//"LIGHT_RANGE_65",
		//"LIGHT_RANGE_100",
		//"LIGHT_RANGE_160",
		//"LIGHT_RANGE_200",
		//"LIGHT_RANGE_325",
		//"LIGHT_RANGE_600",
		//"LIGHT_RANGE_3250",
		if (m_hasLight)
		{
			Light_Desc = Info.Light_Desc;
			if (Light_Desc.vAttenuation.x > 600.f)
			{
				iAttenuation = 11;
			}
			else if (Light_Desc.vAttenuation.x > 325.f)
			{
				iAttenuation = 10;
			}
			else if (Light_Desc.vAttenuation.x > 200.f)
			{
				iAttenuation = 9;
			}
			else if (Light_Desc.vAttenuation.x > 160.f)
			{
				iAttenuation = 8;
			}
			else if (Light_Desc.vAttenuation.x > 100.f)
			{
				iAttenuation = 7;
			}
			else if (Light_Desc.vAttenuation.x > 65.f)
			{
				iAttenuation = 6;
			}
			else if (Light_Desc.vAttenuation.x > 50.f)
			{
				iAttenuation = 5;
			}
			else if (Light_Desc.vAttenuation.x > 32.f)
			{
				iAttenuation = 4;
			}
			else if (Light_Desc.vAttenuation.x > 20.f)
			{
				iAttenuation = 3;
			}
			else if (Light_Desc.vAttenuation.x > 13.f)
			{
				iAttenuation = 2;
			}
			else if (Light_Desc.vAttenuation.x > 7.f)
			{
				iAttenuation = 1;
			}
			else
			{
				iAttenuation = 0;
			}
		}

		shouldScrollToSelectedItem = true;
	} SameLine();

	if (Button("Quick Load"))
	{
		EffectInfo Info = Load_Data(false, true);

		Safe_Release(m_pEffect);
		if (Info.vSize.z == 0.f)
		{
			Info.vSize.z = 1.f;
		}
		Info.pMatrix = &m_DummyMatrix;
		m_pEffect = dynamic_cast<CEffect_Dummy*>(m_pGameInstance->Clone_Object(L"Prototype_GameObject_Dummy", &Info));

		m_iCurrent_Type = Info.iType;

		if (Info.fLifeTime < 0)
		{
			hasLifeTime = false;
			m_fEffectLifeTime = -1.f;
		}
		else
		{
			hasLifeTime = true;
			m_fEffectLifeTime = Info.fLifeTime;
		}

		vPosOffset = Info.vPosOffset;

		iPassIndex = Info.iPassIndex;

		bSkipBloom = Info.bSkipBloom;

		if (Info.strDiffuseTexture.size())
		{
			iIsColor = 0;
			m_hasDiffTexture = true;
			m_iSelected_Texture = Compute_TextureIndex(Info.strDiffuseTexture);
		}
		else
		{
			iIsColor = 1;
			m_hasDiffTexture = false;
			m_vColor = Info.vColor;
		}

		isSprite = Info.isSprite;
		if (Info.isSprite)
		{
			vNumSprites = Info.vNumSprites;
			isFixedIndex = Info.isFixedIndex;
			if (isFixedIndex)
			{
				iFixedSpriteIndex = Info.iFixedSpriteIndex;
			}
			else
			{
				fSpriteDuration = Info.fSpriteDuration;
			}
		}

		if (Info.strMaskTexture.size())
		{
			m_hasMask = true;
			m_iSelected_MaskTexture = Compute_TextureIndex(Info.strMaskTexture);
			vUVDelta = Info.vUVDelta;
			vUVInit = Info.vUVInit;
			isUVLoop = Info.isUVLoop;
			fAlphaInit = Info.fAlphaInit;
			fAlphaDelta = Info.fAlphaDelta;
		}
		else
		{
			m_hasMask = false;
		}

		if (Info.strUnDissolveTexture.size())
		{
			hasUnDissolve = true;
			iSelectd_UnDissolve = Compute_TextureIndex(Info.strUnDissolveTexture);
			fUnDissolveDuration = Info.fUnDissolveDuration;
		}
		else
		{
			hasUnDissolve = false;
		}

		if (Info.strDissolveTexture.size())
		{
			hasDissolve = true;
			iSelectd_Dissolve = Compute_TextureIndex(Info.strDissolveTexture);
			fDissolveDuration = Info.fDissolveDuration;
		}
		else
		{
			hasDissolve = false;
		}

		switch (Info.iType)
		{
		case Engine::ET_PARTICLE:
		case Engine::ET_PARTI_DISTORTION:
			m_ParticleInfo = Info.PartiDesc;
			m_iNumInstance = Info.iNumInstances;
			bApplyGravity = Info.bApplyGravity;
			vGravityDir = Info.vGravityDir;
			fPartiDissolveRatio = Info.fPartiDissolveRatio;
			fPartiAppearRatio = Info.fPartiAppearRatio;
			break;
		case Engine::ET_RECT:
			isBillboard = Info.isBillboard;
			if (Info.isSprite)
			{
				fSizeforSprite = Info.vSize.y;
			}
			vSize = Info.vSize;
			if (vSize.z == 0.f)
			{
				vSize.z = 1.f;
			}
			vSizeDelta = Info.vSizeDelta;
			fRectRotationAngle = Info.fRectRotationAngle;
			break;
		case Engine::ET_MESH:
		case Engine::ET_DISTORTION:
			isBillboard = Info.isBillboard;
			if (isBillboard)
			{
				vBillboardRotation = Info.vBillboardRotation;
			}
			m_iSelected_Model = Compute_ModelIndex(Info.strModel);
			vSize = Info.vSize;
			vSizeDelta = Info.vSizeDelta;
			break;
		}

		m_hasLight = Info.hasLight;

		//"LIGHT_RANGE_7",
		//"LIGHT_RANGE_13",
		//"LIGHT_RANGE_20",
		//"LIGHT_RANGE_32",
		//"LIGHT_RANGE_50",
		//"LIGHT_RANGE_65",
		//"LIGHT_RANGE_100",
		//"LIGHT_RANGE_160",
		//"LIGHT_RANGE_200",
		//"LIGHT_RANGE_325",
		//"LIGHT_RANGE_600",
		//"LIGHT_RANGE_3250",
		if (m_hasLight)
		{
			Light_Desc = Info.Light_Desc;
			if (Light_Desc.vAttenuation.x > 600.f)
			{
				iAttenuation = 11;
			}
			else if (Light_Desc.vAttenuation.x > 325.f)
			{
				iAttenuation = 10;
			}
			else if (Light_Desc.vAttenuation.x > 200.f)
			{
				iAttenuation = 9;
			}
			else if (Light_Desc.vAttenuation.x > 160.f)
			{
				iAttenuation = 8;
			}
			else if (Light_Desc.vAttenuation.x > 100.f)
			{
				iAttenuation = 7;
			}
			else if (Light_Desc.vAttenuation.x > 65.f)
			{
				iAttenuation = 6;
			}
			else if (Light_Desc.vAttenuation.x > 50.f)
			{
				iAttenuation = 5;
			}
			else if (Light_Desc.vAttenuation.x > 32.f)
			{
				iAttenuation = 4;
			}
			else if (Light_Desc.vAttenuation.x > 20.f)
			{
				iAttenuation = 3;
			}
			else if (Light_Desc.vAttenuation.x > 13.f)
			{
				iAttenuation = 2;
			}
			else if (Light_Desc.vAttenuation.x > 7.f)
			{
				iAttenuation = 1;
			}
			else
			{
				iAttenuation = 0;
			}
		}

		shouldScrollToSelectedItem = true;
	} SameLine();

	if (Button("Override"))
	{
		Override_Data(Info);
	}

	if (Button("Add"))
	{
		CEffect_Dummy* pEffect{};
		pEffect = dynamic_cast<CEffect_Dummy*>(m_pGameInstance->Clone_Object(L"Prototype_GameObject_Dummy", &Load_Data(true)));
		m_AddEffect.push_back(pEffect);
	} SameLine();

	if (Button("Clear Add"))
	{
		for (auto& pEffect : m_AddEffect)
		{
			Safe_Release(pEffect);
		}
		m_AddEffect.clear();
	}


	if (not m_pEffect or m_pEffect->isDead() or m_pGameInstance->Key_Down(DIK_RETURN) or m_pGameInstance->Key_Down(DIK_E))
	{
		Safe_Release(m_pEffect);
		if (isDummyMove)
		{
			m_DummyMatrix = _mat::CreateTranslation(_vec3(fRadius, 0.f, 0.f));
		}
		else
		{
			m_DummyMatrix = _mat::Identity;
		}

		Info.pMatrix = &m_DummyMatrix;
		m_pEffect = dynamic_cast<CEffect_Dummy*>(m_pGameInstance->Clone_Object(L"Prototype_GameObject_Dummy", &Info));
		//m_pEffect->Tick(fTimeDelta);
	}

	End();
#pragma endregion

	Begin("Background");
	ColorPicker4("Background Color", reinterpret_cast<_float*>(&m_vBGColor), ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB);
	End();

}

void CImgui_Manager::Effect_Tick(_float fTimeDelta)
{
	if (m_pEffect)
	{
		m_pEffect->Tick(fTimeDelta);
	}

	for (auto& pEffect : m_AddEffect)
	{
		pEffect->Tick(fTimeDelta);
	}
}

void CImgui_Manager::Effect_LateTick(_float fTimeDelta)
{
	if (m_pEffect)
	{
		m_pEffect->Late_Tick(fTimeDelta);
	}

	for (auto& pEffect : m_AddEffect)
	{
		pEffect->Late_Tick(fTimeDelta);
	}
}

HRESULT CImgui_Manager::Render()
{
	//if (m_pEffect)
	//{
	//	if (FAILED(m_pEffect->Render()))
	//	{
	//		return E_FAIL;
	//	}
	//}

	//for (auto& pEffect : m_AddEffect)
	//{
	//	if (FAILED(pEffect->Render()))
	//	{
	//		return E_FAIL;
	//	}
	//}


	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(GetDrawData());

	return S_OK;
}

HRESULT CImgui_Manager::Ready_Layers()
{
	LIGHT_DESC LightDesc{};

	LightDesc.eType = LIGHT_DESC::Directional;
	LightDesc.vDirection = _float4(-1.f, -2.f, -1.f, 0.f);
	LightDesc.vDiffuse = _float4(0.f, 0.f, 0.f, 1.f);
	LightDesc.vAmbient = _float4(0.3f, 0.3f, 0.3f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LEVEL_STATIC, TEXT("Light_Main"), LightDesc)))
	{
		return E_FAIL;
	}

	CCamera::Camera_Desc CamDesc;
	CamDesc.vCameraPos = _float4(0.f, 5.f, -5.f, 1.f);
	CamDesc.vFocusPos = _float4(0.f, 0.f, 0.f, 1.f);
	CamDesc.fFovY = XMConvertToRadians(60.f);
	CamDesc.fAspect = static_cast<_float>(g_iWinSizeX) / g_iWinSizeY;
	CamDesc.fNear = 0.1f;
	CamDesc.fFar = 100.f;

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, L"Layer_Camera", TEXT("Prototype_GameObject_Camera"), &CamDesc)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_BG"), TEXT("Prototype_GameObject_BG"))))
	{
		return E_FAIL;
	}

	TERRAIN_INFO TerrainInfo{};
	TerrainInfo.vTerrainSize = { 3, 3 };
	TerrainInfo.isMesh = false;

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Terrain"), TEXT("Prototype_GameObject_Terrain"), &TerrainInfo)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Layer(LEVEL_STATIC, TEXT("Layer_Map"), TEXT("Prototype_GameObject_Map"))))
	{
		return E_FAIL;
	}

	//CVIBuffer_Instancing::ParticleDesc Desc{};
	//m_pVtxTexShader = dynamic_cast<CShader*>(m_pGameInstance->Clone_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxTex"));
	//if (not m_pVtxTexShader)
	//{
	//	return E_FAIL;
	//}

	//m_pInstanceShader = dynamic_cast<CShader*>(m_pGameInstance->Clone_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxTex_Instancing"));
	//if (not m_pInstanceShader)
	//{
	//	return E_FAIL;
	//}

	//m_pGameInstance->Clone_Component(LEVEL_STATIC, L"Prototype_Component_VIBuffer_Rect", &m_pVIBuffer_Rect);
	//m_pGameInstance->Clone_Component(LEVEL_STATIC, L"Prototype_Component_VIBuffer_Instancing_Point", &m_pVIBuffer_Instancing);

	for (_int i = 0; i < m_iNumTextures; i++)
	{
		wstring strPrototypeTag = L"Prototype_Component_Texture_Effect_";
		_tchar strUnicode[MAX_PATH]{};
		MultiByteToWideChar(CP_ACP, 0, m_pItemList_Texture[i], static_cast<int>(strlen(m_pItemList_Texture[i])), strUnicode, static_cast<int>(strlen(m_pItemList_Texture[i])));

		strPrototypeTag += strUnicode;
		m_pTextures[i] = dynamic_cast<CTexture*>(m_pGameInstance->Clone_Component(LEVEL_STATIC, strPrototypeTag));
	}

	for (_int i = 0; i < m_iNumModels; i++)
	{
		wstring strPrototypeTag = L"Prototype_Model_";
		_tchar strUnicode[MAX_PATH]{};
		MultiByteToWideChar(CP_ACP, 0, m_pItemList_Model[i], static_cast<int>(strlen(m_pItemList_Model[i])), strUnicode, static_cast<int>(strlen(m_pItemList_Model[i])));

		strPrototypeTag += strUnicode;
		m_pModels[i] = dynamic_cast<CModel*>(m_pGameInstance->Clone_Component(LEVEL_STATIC, strPrototypeTag));
	}

	return S_OK;
}

const _int& CImgui_Manager::Compute_TextureIndex(const wstring& strTexture)
{
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, strTexture.c_str(), (int)strTexture.size(), NULL, 0, NULL, NULL);
	std::string str(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, strTexture.c_str(), (int)strTexture.size(), &str[0], size_needed, NULL, NULL);

	for (size_t i = 0; i < m_iNumTextures; i++)
	{
		if (str == m_pItemList_Texture[i])
		{
			return i;
		}
	}

	return -1;
}

const _int& CImgui_Manager::Compute_ModelIndex(const string& strModel)
{
	for (size_t i = 0; i < m_iNumModels; i++)
	{
		if (strModel == m_pItemList_Model[i])
		{
			return i;
		}
	}

	return -1;
}

EffectInfo CImgui_Manager::Load_Data(_bool isAdd, _bool isQuick)
{
	EffectInfo Info{};

	OPENFILENAME ofn;
	TCHAR filePathName[MAX_PATH] = L"";
	TCHAR lpstrFile[MAX_PATH] = L"Effect.effect";
	static TCHAR filter[] = L"이펙트 파일(*.effect)\0*.effect\0";

	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = g_hWnd;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = lpstrFile;
	ofn.nMaxFile = 256;
	ofn.lpstrInitialDir = L"..\\..\\Client\\Bin\\EffectData";

	if (isQuick or GetOpenFileName(&ofn))
	{
		filesystem::path strFilePath = ofn.lpstrFile;
		if (isQuick)
		{
			strFilePath = m_CurrFilePath;
		}
		ifstream InFile(strFilePath.c_str(), ios::binary);

		if (not isAdd)
		{
			m_CurrFilePath = strFilePath;
		}

		if (InFile.is_open())
		{
			InFile.read(reinterpret_cast<_char*>(&Info.iType), sizeof Info.iType);
			InFile.read(reinterpret_cast<_char*>(&Info.isSprite), sizeof Info.isSprite);
			InFile.read(reinterpret_cast<_char*>(&Info.vNumSprites), sizeof Info.vNumSprites);
			InFile.read(reinterpret_cast<_char*>(&Info.fSpriteDuration), sizeof Info.fSpriteDuration);
			InFile.read(reinterpret_cast<_char*>(&Info.PartiDesc), sizeof Info.PartiDesc);
			InFile.read(reinterpret_cast<_char*>(&Info.iNumInstances), sizeof Info.iNumInstances);
			InFile.read(reinterpret_cast<_char*>(&Info.fLifeTime), sizeof Info.fLifeTime);
			InFile.read(reinterpret_cast<_char*>(&Info.vColor), sizeof Info.vColor);
			InFile.read(reinterpret_cast<_char*>(&Info.iPassIndex), sizeof Info.iPassIndex);
			InFile.read(reinterpret_cast<_char*>(&Info.vSize), sizeof Info.vSize);
			InFile.read(reinterpret_cast<_char*>(&Info.vPosOffset), sizeof Info.vPosOffset);
			InFile.read(reinterpret_cast<_char*>(&Info.vSizeDelta), sizeof Info.vSizeDelta);
			InFile.read(reinterpret_cast<_char*>(&Info.bApplyGravity), sizeof Info.bApplyGravity);
			InFile.read(reinterpret_cast<_char*>(&Info.vGravityDir), sizeof Info.vGravityDir);
			InFile.read(reinterpret_cast<_char*>(&Info.fDissolveDuration), sizeof Info.fDissolveDuration);
			InFile.read(reinterpret_cast<_char*>(&Info.bSkipBloom), sizeof Info.bSkipBloom);
			InFile.read(reinterpret_cast<_char*>(&Info.fUnDissolveDuration), sizeof Info.fUnDissolveDuration);
			InFile.read(reinterpret_cast<_char*>(&Info.vUVInit), sizeof Info.vUVInit);
			InFile.read(reinterpret_cast<_char*>(&Info.vUVDelta), sizeof Info.vUVDelta);
			InFile.read(reinterpret_cast<_char*>(&Info.isRandomSprite), sizeof Info.isRandomSprite);
			InFile.read(reinterpret_cast<_char*>(&Info.hasLight), sizeof Info.hasLight);
			InFile.read(reinterpret_cast<_char*>(&Info.Light_Desc), sizeof Info.Light_Desc);
			InFile.read(reinterpret_cast<_char*>(&Info.isFixedIndex), sizeof Info.isFixedIndex);
			InFile.read(reinterpret_cast<_char*>(&Info.iFixedSpriteIndex), sizeof Info.iFixedSpriteIndex);
			InFile.read(reinterpret_cast<_char*>(&Info.isUVLoop), sizeof Info.isUVLoop);
			InFile.read(reinterpret_cast<_char*>(&Info.fAlphaInit), sizeof Info.fAlphaInit);
			InFile.read(reinterpret_cast<_char*>(&Info.fAlphaDelta), sizeof Info.fAlphaDelta);
			InFile.read(reinterpret_cast<_char*>(&Info.fRectRotationAngle), sizeof Info.fRectRotationAngle);
			InFile.read(reinterpret_cast<_char*>(&Info.isBillboard), sizeof Info.isBillboard);
			InFile.read(reinterpret_cast<_char*>(&Info.vBillboardRotation), sizeof Info.vBillboardRotation);
			InFile.read(reinterpret_cast<_char*>(&Info.fPartiDissolveRatio), sizeof Info.fPartiDissolveRatio);
			InFile.read(reinterpret_cast<_char*>(&Info.fPartiAppearRatio), sizeof Info.fPartiAppearRatio);

			size_t iNameSize{};

			{
				_tchar* pBuffer{};

				InFile.read(reinterpret_cast<_char*>(&iNameSize), sizeof size_t);
				pBuffer = new _tchar[iNameSize / sizeof(_tchar)];
				InFile.read(reinterpret_cast<_char*>(pBuffer), iNameSize);
				Info.strDiffuseTexture = pBuffer;
				Safe_Delete_Array(pBuffer);
				iNameSize = {};

				InFile.read(reinterpret_cast<_char*>(&iNameSize), sizeof size_t);
				pBuffer = new _tchar[iNameSize / sizeof(_tchar)];
				InFile.read(reinterpret_cast<_char*>(pBuffer), iNameSize);
				Info.strMaskTexture = pBuffer;
				Safe_Delete_Array(pBuffer);
				iNameSize = {};

				InFile.read(reinterpret_cast<_char*>(&iNameSize), sizeof size_t);
				pBuffer = new _tchar[iNameSize / sizeof(_tchar)];
				InFile.read(reinterpret_cast<_char*>(pBuffer), iNameSize);
				Info.strDissolveTexture = pBuffer;
				Safe_Delete_Array(pBuffer);
				iNameSize = {};

				InFile.read(reinterpret_cast<_char*>(&iNameSize), sizeof size_t);
				pBuffer = new _tchar[iNameSize / sizeof(_tchar)];
				InFile.read(reinterpret_cast<_char*>(pBuffer), iNameSize);
				Info.strUnDissolveTexture = pBuffer;
				Safe_Delete_Array(pBuffer);
				iNameSize = {};
			}

			{
				_char* pBuffer{};
				InFile.read(reinterpret_cast<_char*>(&iNameSize), sizeof size_t);
				pBuffer = new _char[iNameSize];
				InFile.read(pBuffer, iNameSize);
				Info.strModel = pBuffer;
				Safe_Delete_Array(pBuffer);
				iNameSize = {};
			}

			InFile.close();
		}
	}

	return Info;
}

void CImgui_Manager::Load_OldData()
{
	OldEffectInfo OldInfo{};
	//OldEffectInfo OldInfo{};

	string strInputFilePath = "../../Client/Bin/EffectData/";
	for (const auto& entry : std::filesystem::recursive_directory_iterator(strInputFilePath))
	{
		if (entry.is_regular_file())
		{
			wstring extension = entry.path().extension().wstring();
			if (entry.path().extension().wstring() != L".effect")
			{
				continue;
			}

			OldInfo = {};
			ifstream InFile(entry.path().c_str(), ios::binary);

			if (InFile.is_open())
			{
				InFile.read(reinterpret_cast<_char*>(&OldInfo.iType), sizeof OldInfo.iType);
				InFile.read(reinterpret_cast<_char*>(&OldInfo.isSprite), sizeof OldInfo.isSprite);
				InFile.read(reinterpret_cast<_char*>(&OldInfo.vNumSprites), sizeof OldInfo.vNumSprites);
				InFile.read(reinterpret_cast<_char*>(&OldInfo.fSpriteDuration), sizeof OldInfo.fSpriteDuration);
				InFile.read(reinterpret_cast<_char*>(&OldInfo.PartiDesc), sizeof OldInfo.PartiDesc);
				InFile.read(reinterpret_cast<_char*>(&OldInfo.iNumInstances), sizeof OldInfo.iNumInstances);
				InFile.read(reinterpret_cast<_char*>(&OldInfo.fLifeTime), sizeof OldInfo.fLifeTime);
				InFile.read(reinterpret_cast<_char*>(&OldInfo.vColor), sizeof OldInfo.vColor);
				InFile.read(reinterpret_cast<_char*>(&OldInfo.iPassIndex), sizeof OldInfo.iPassIndex);
				InFile.read(reinterpret_cast<_char*>(&OldInfo.vSize), sizeof OldInfo.vSize);
				InFile.read(reinterpret_cast<_char*>(&OldInfo.vPosOffset), sizeof OldInfo.vPosOffset);
				InFile.read(reinterpret_cast<_char*>(&OldInfo.vSizeDelta), sizeof OldInfo.vSizeDelta);
				InFile.read(reinterpret_cast<_char*>(&OldInfo.bApplyGravity), sizeof OldInfo.bApplyGravity);
				InFile.read(reinterpret_cast<_char*>(&OldInfo.vGravityDir), sizeof OldInfo.vGravityDir);
				InFile.read(reinterpret_cast<_char*>(&OldInfo.fDissolveDuration), sizeof OldInfo.fDissolveDuration);
				InFile.read(reinterpret_cast<_char*>(&OldInfo.bSkipBloom), sizeof OldInfo.bSkipBloom);
				InFile.read(reinterpret_cast<_char*>(&OldInfo.fUnDissolveDuration), sizeof OldInfo.fUnDissolveDuration);
				InFile.read(reinterpret_cast<_char*>(&OldInfo.vUVInit), sizeof OldInfo.vUVInit);
				InFile.read(reinterpret_cast<_char*>(&OldInfo.vUVDelta), sizeof OldInfo.vUVDelta);
				InFile.read(reinterpret_cast<_char*>(&OldInfo.isRandomSprite), sizeof OldInfo.isRandomSprite);
				InFile.read(reinterpret_cast<_char*>(&OldInfo.hasLight), sizeof OldInfo.hasLight);
				InFile.read(reinterpret_cast<_char*>(&OldInfo.Light_Desc), sizeof OldInfo.Light_Desc);
				InFile.read(reinterpret_cast<_char*>(&OldInfo.isFixedIndex), sizeof OldInfo.isFixedIndex);
				InFile.read(reinterpret_cast<_char*>(&OldInfo.iFixedSpriteIndex), sizeof OldInfo.iFixedSpriteIndex);
				InFile.read(reinterpret_cast<_char*>(&OldInfo.isUVLoop), sizeof OldInfo.isUVLoop);
				InFile.read(reinterpret_cast<_char*>(&OldInfo.fAlphaInit), sizeof OldInfo.fAlphaInit);
				InFile.read(reinterpret_cast<_char*>(&OldInfo.fAlphaDelta), sizeof OldInfo.fAlphaDelta);
				InFile.read(reinterpret_cast<_char*>(&OldInfo.fRectRotationAngle), sizeof OldInfo.fRectRotationAngle);
				InFile.read(reinterpret_cast<_char*>(&OldInfo.isBillboard), sizeof OldInfo.isBillboard);
				InFile.read(reinterpret_cast<_char*>(&OldInfo.vBillboardRotation), sizeof OldInfo.vBillboardRotation);
				InFile.read(reinterpret_cast<_char*>(&OldInfo.fPartiDissolveRatio), sizeof OldInfo.fPartiDissolveRatio);

				size_t iNameSize{};

				{
					_tchar* pBuffer{};

					InFile.read(reinterpret_cast<_char*>(&iNameSize), sizeof size_t);
					pBuffer = new _tchar[iNameSize / sizeof(_tchar)];
					InFile.read(reinterpret_cast<_char*>(pBuffer), iNameSize);
					OldInfo.strDiffuseTexture = pBuffer;
					Safe_Delete_Array(pBuffer);
					iNameSize = {};

					InFile.read(reinterpret_cast<_char*>(&iNameSize), sizeof size_t);
					pBuffer = new _tchar[iNameSize / sizeof(_tchar)];
					InFile.read(reinterpret_cast<_char*>(pBuffer), iNameSize);
					OldInfo.strMaskTexture = pBuffer;
					Safe_Delete_Array(pBuffer);
					iNameSize = {};

					InFile.read(reinterpret_cast<_char*>(&iNameSize), sizeof size_t);
					pBuffer = new _tchar[iNameSize / sizeof(_tchar)];
					InFile.read(reinterpret_cast<_char*>(pBuffer), iNameSize);
					OldInfo.strDissolveTexture = pBuffer;
					Safe_Delete_Array(pBuffer);
					iNameSize = {};

					InFile.read(reinterpret_cast<_char*>(&iNameSize), sizeof size_t);
					pBuffer = new _tchar[iNameSize / sizeof(_tchar)];
					InFile.read(reinterpret_cast<_char*>(pBuffer), iNameSize);
					OldInfo.strUnDissolveTexture = pBuffer;
					Safe_Delete_Array(pBuffer);
					iNameSize = {};
				}

				{
					_char* pBuffer{};
					InFile.read(reinterpret_cast<_char*>(&iNameSize), sizeof size_t);
					pBuffer = new _char[iNameSize];
					InFile.read(pBuffer, iNameSize);
					OldInfo.strModel = pBuffer;
					Safe_Delete_Array(pBuffer);
					iNameSize = {};
				}

				InFile.close();
			}

			EffectInfo NewInfo{ };

			NewInfo.iType = OldInfo.iType;
			NewInfo.isSprite = OldInfo.isSprite;
			NewInfo.vNumSprites = OldInfo.vNumSprites;
			NewInfo.fSpriteDuration = OldInfo.fSpriteDuration;
			NewInfo.PartiDesc = OldInfo.PartiDesc;
			NewInfo.iNumInstances = OldInfo.iNumInstances;
			NewInfo.fLifeTime = OldInfo.fLifeTime;
			NewInfo.vColor = OldInfo.vColor;
			NewInfo.iPassIndex = OldInfo.iPassIndex;
			NewInfo.vSize = OldInfo.vSize;
			NewInfo.vPosOffset = OldInfo.vPosOffset;
			NewInfo.vSizeDelta = OldInfo.vSizeDelta;
			NewInfo.bApplyGravity = OldInfo.bApplyGravity;
			NewInfo.vGravityDir = OldInfo.vGravityDir;
			NewInfo.fDissolveDuration = OldInfo.fDissolveDuration;
			NewInfo.bSkipBloom = OldInfo.bSkipBloom;
			NewInfo.fUnDissolveDuration = OldInfo.fUnDissolveDuration;
			NewInfo.vUVInit = OldInfo.vUVInit;
			NewInfo.vUVDelta = OldInfo.vUVDelta;
			NewInfo.isRandomSprite = OldInfo.isRandomSprite;
			NewInfo.hasLight = OldInfo.hasLight;
			NewInfo.Light_Desc = OldInfo.Light_Desc;
			NewInfo.isFixedIndex = OldInfo.isFixedIndex;
			NewInfo.iFixedSpriteIndex = OldInfo.iFixedSpriteIndex;
			NewInfo.isUVLoop = OldInfo.isUVLoop;
			NewInfo.fAlphaDelta = OldInfo.fAlphaDelta;
			NewInfo.fAlphaInit = OldInfo.fAlphaInit;
			NewInfo.fRectRotationAngle = OldInfo.fRectRotationAngle;
			NewInfo.isBillboard = OldInfo.isBillboard;
			NewInfo.vBillboardRotation = OldInfo.vBillboardRotation;
			NewInfo.fPartiDissolveRatio = OldInfo.fPartiDissolveRatio;
			NewInfo.fPartiAppearRatio = 0.f;

			NewInfo.strDiffuseTexture = OldInfo.strDiffuseTexture;
			NewInfo.strMaskTexture = OldInfo.strMaskTexture;
			NewInfo.strDissolveTexture = OldInfo.strDissolveTexture;
			NewInfo.strUnDissolveTexture = OldInfo.strUnDissolveTexture;
			NewInfo.strModel = OldInfo.strModel;

			filesystem::path strFilePath = entry.path();
			ofstream OutFile(strFilePath.c_str(), ios::binary);

			if (OutFile.is_open())
			{
				OutFile.write(reinterpret_cast<const _char*>(&NewInfo.iType), sizeof NewInfo.iType);
				OutFile.write(reinterpret_cast<const _char*>(&NewInfo.isSprite), sizeof NewInfo.isSprite);
				OutFile.write(reinterpret_cast<const _char*>(&NewInfo.vNumSprites), sizeof NewInfo.vNumSprites);
				OutFile.write(reinterpret_cast<const _char*>(&NewInfo.fSpriteDuration), sizeof NewInfo.fSpriteDuration);
				OutFile.write(reinterpret_cast<const _char*>(&NewInfo.PartiDesc), sizeof NewInfo.PartiDesc);
				OutFile.write(reinterpret_cast<const _char*>(&NewInfo.iNumInstances), sizeof NewInfo.iNumInstances);
				OutFile.write(reinterpret_cast<const _char*>(&NewInfo.fLifeTime), sizeof NewInfo.fLifeTime);
				OutFile.write(reinterpret_cast<const _char*>(&NewInfo.vColor), sizeof NewInfo.vColor);
				OutFile.write(reinterpret_cast<const _char*>(&NewInfo.iPassIndex), sizeof NewInfo.iPassIndex);
				OutFile.write(reinterpret_cast<const _char*>(&NewInfo.vSize), sizeof NewInfo.vSize);
				OutFile.write(reinterpret_cast<const _char*>(&NewInfo.vPosOffset), sizeof NewInfo.vPosOffset);
				OutFile.write(reinterpret_cast<const _char*>(&NewInfo.vSizeDelta), sizeof NewInfo.vSizeDelta);
				OutFile.write(reinterpret_cast<const _char*>(&NewInfo.bApplyGravity), sizeof NewInfo.bApplyGravity);
				OutFile.write(reinterpret_cast<const _char*>(&NewInfo.vGravityDir), sizeof NewInfo.vGravityDir);
				OutFile.write(reinterpret_cast<const _char*>(&NewInfo.fDissolveDuration), sizeof NewInfo.fDissolveDuration);
				OutFile.write(reinterpret_cast<const _char*>(&NewInfo.bSkipBloom), sizeof NewInfo.bSkipBloom);
				OutFile.write(reinterpret_cast<const _char*>(&NewInfo.fUnDissolveDuration), sizeof NewInfo.fUnDissolveDuration);
				OutFile.write(reinterpret_cast<const _char*>(&NewInfo.vUVInit), sizeof NewInfo.vUVInit);
				OutFile.write(reinterpret_cast<const _char*>(&NewInfo.vUVDelta), sizeof NewInfo.vUVDelta);
				OutFile.write(reinterpret_cast<const _char*>(&NewInfo.isRandomSprite), sizeof NewInfo.isRandomSprite);
				OutFile.write(reinterpret_cast<const _char*>(&NewInfo.hasLight), sizeof NewInfo.hasLight);
				OutFile.write(reinterpret_cast<const _char*>(&NewInfo.Light_Desc), sizeof NewInfo.Light_Desc);
				OutFile.write(reinterpret_cast<const _char*>(&NewInfo.isFixedIndex), sizeof NewInfo.isFixedIndex);
				OutFile.write(reinterpret_cast<const _char*>(&NewInfo.iFixedSpriteIndex), sizeof NewInfo.iFixedSpriteIndex);
				OutFile.write(reinterpret_cast<const _char*>(&NewInfo.isUVLoop), sizeof NewInfo.isUVLoop);
				OutFile.write(reinterpret_cast<const _char*>(&NewInfo.fAlphaInit), sizeof NewInfo.fAlphaInit);
				OutFile.write(reinterpret_cast<const _char*>(&NewInfo.fAlphaDelta), sizeof NewInfo.fAlphaDelta);
				OutFile.write(reinterpret_cast<const _char*>(&NewInfo.fRectRotationAngle), sizeof NewInfo.fRectRotationAngle);
				OutFile.write(reinterpret_cast<const _char*>(&NewInfo.isBillboard), sizeof NewInfo.isBillboard);
				OutFile.write(reinterpret_cast<const _char*>(&NewInfo.vBillboardRotation), sizeof NewInfo.vBillboardRotation);
				OutFile.write(reinterpret_cast<const _char*>(&NewInfo.fPartiDissolveRatio), sizeof NewInfo.fPartiDissolveRatio);
				OutFile.write(reinterpret_cast<const _char*>(&NewInfo.fPartiAppearRatio), sizeof NewInfo.fPartiAppearRatio);

				size_t iNameSize{};
				iNameSize = (NewInfo.strDiffuseTexture.size() + 1) * sizeof(_tchar);
				OutFile.write(reinterpret_cast<const _char*>(&iNameSize), sizeof size_t);
				OutFile.write(reinterpret_cast<const _char*>(NewInfo.strDiffuseTexture.data()), iNameSize);

				iNameSize = (NewInfo.strMaskTexture.size() + 1) * sizeof(_tchar);
				OutFile.write(reinterpret_cast<const _char*>(&iNameSize), sizeof size_t);
				OutFile.write(reinterpret_cast<const _char*>(NewInfo.strMaskTexture.data()), iNameSize);

				iNameSize = (NewInfo.strDissolveTexture.size() + 1) * sizeof(_tchar);
				OutFile.write(reinterpret_cast<const _char*>(&iNameSize), sizeof size_t);
				OutFile.write(reinterpret_cast<const _char*>(NewInfo.strDissolveTexture.data()), iNameSize);

				iNameSize = (NewInfo.strUnDissolveTexture.size() + 1) * sizeof(_tchar);
				OutFile.write(reinterpret_cast<const _char*>(&iNameSize), sizeof size_t);
				OutFile.write(reinterpret_cast<const _char*>(NewInfo.strUnDissolveTexture.data()), iNameSize);

				iNameSize = NewInfo.strModel.size() + sizeof(_char);
				OutFile.write(reinterpret_cast<const _char*>(&iNameSize), sizeof size_t);
				OutFile.write(NewInfo.strModel.data(), iNameSize);

				OutFile.close();
			}
		}
	}
}

HRESULT CImgui_Manager::Export_Data(EffectInfo& Info)
{
	OPENFILENAME ofn;
	TCHAR filePathName[MAX_PATH] = L"";
	wstring strTempFilePath = m_CurrFilePath.wstring();
	static TCHAR filter[] = L"이펙트 파일(*.effect)\0*.effect\0";

	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = g_hWnd;
	ofn.lpstrFilter = filter;
	ofn.lpstrFile = strTempFilePath.data();
	ofn.nMaxFile = 256;
	ofn.lpstrInitialDir = L"..\\..\\Client\\Bin\\EffectData";
	ofn.Flags = OFN_OVERWRITEPROMPT;

	if (GetSaveFileName(&ofn))
	{
		filesystem::path strFilePath = ofn.lpstrFile;
		ofstream OutFile(strFilePath.c_str(), ios::binary);

		m_CurrFilePath = strFilePath;

		if (OutFile.is_open())
		{
			OutFile.write(reinterpret_cast<const _char*>(&Info.iType), sizeof Info.iType);
			OutFile.write(reinterpret_cast<const _char*>(&Info.isSprite), sizeof Info.isSprite);
			OutFile.write(reinterpret_cast<const _char*>(&Info.vNumSprites), sizeof Info.vNumSprites);
			OutFile.write(reinterpret_cast<const _char*>(&Info.fSpriteDuration), sizeof Info.fSpriteDuration);
			OutFile.write(reinterpret_cast<const _char*>(&Info.PartiDesc), sizeof Info.PartiDesc);
			OutFile.write(reinterpret_cast<const _char*>(&Info.iNumInstances), sizeof Info.iNumInstances);
			OutFile.write(reinterpret_cast<const _char*>(&Info.fLifeTime), sizeof Info.fLifeTime);
			OutFile.write(reinterpret_cast<const _char*>(&Info.vColor), sizeof Info.vColor);
			OutFile.write(reinterpret_cast<const _char*>(&Info.iPassIndex), sizeof Info.iPassIndex);
			OutFile.write(reinterpret_cast<const _char*>(&Info.vSize), sizeof Info.vSize);
			OutFile.write(reinterpret_cast<const _char*>(&Info.vPosOffset), sizeof Info.vPosOffset);
			OutFile.write(reinterpret_cast<const _char*>(&Info.vSizeDelta), sizeof Info.vSizeDelta);
			OutFile.write(reinterpret_cast<const _char*>(&Info.bApplyGravity), sizeof Info.bApplyGravity);
			OutFile.write(reinterpret_cast<const _char*>(&Info.vGravityDir), sizeof Info.vGravityDir);
			OutFile.write(reinterpret_cast<const _char*>(&Info.fDissolveDuration), sizeof Info.fDissolveDuration);
			OutFile.write(reinterpret_cast<const _char*>(&Info.bSkipBloom), sizeof Info.bSkipBloom);
			OutFile.write(reinterpret_cast<const _char*>(&Info.fUnDissolveDuration), sizeof Info.fUnDissolveDuration);
			OutFile.write(reinterpret_cast<const _char*>(&Info.vUVInit), sizeof Info.vUVInit);
			OutFile.write(reinterpret_cast<const _char*>(&Info.vUVDelta), sizeof Info.vUVDelta);
			OutFile.write(reinterpret_cast<const _char*>(&Info.isRandomSprite), sizeof Info.isRandomSprite);
			OutFile.write(reinterpret_cast<const _char*>(&Info.hasLight), sizeof Info.hasLight);
			OutFile.write(reinterpret_cast<const _char*>(&Info.Light_Desc), sizeof Info.Light_Desc);
			OutFile.write(reinterpret_cast<const _char*>(&Info.isFixedIndex), sizeof Info.isFixedIndex);
			OutFile.write(reinterpret_cast<const _char*>(&Info.iFixedSpriteIndex), sizeof Info.iFixedSpriteIndex);
			OutFile.write(reinterpret_cast<const _char*>(&Info.isUVLoop), sizeof Info.isUVLoop);
			OutFile.write(reinterpret_cast<const _char*>(&Info.fAlphaInit), sizeof Info.fAlphaInit);
			OutFile.write(reinterpret_cast<const _char*>(&Info.fAlphaDelta), sizeof Info.fAlphaDelta);
			OutFile.write(reinterpret_cast<const _char*>(&Info.fRectRotationAngle), sizeof Info.fRectRotationAngle);
			OutFile.write(reinterpret_cast<const _char*>(&Info.isBillboard), sizeof Info.isBillboard);
			OutFile.write(reinterpret_cast<const _char*>(&Info.vBillboardRotation), sizeof Info.vBillboardRotation);
			OutFile.write(reinterpret_cast<const _char*>(&Info.fPartiDissolveRatio), sizeof Info.fPartiDissolveRatio);
			OutFile.write(reinterpret_cast<const _char*>(&Info.fPartiAppearRatio), sizeof Info.fPartiAppearRatio);

			size_t iNameSize{};
			iNameSize = (Info.strDiffuseTexture.size() + 1) * sizeof(_tchar);
			OutFile.write(reinterpret_cast<const _char*>(&iNameSize), sizeof size_t);
			OutFile.write(reinterpret_cast<const _char*>(Info.strDiffuseTexture.data()), iNameSize);

			iNameSize = (Info.strMaskTexture.size() + 1) * sizeof(_tchar);
			OutFile.write(reinterpret_cast<const _char*>(&iNameSize), sizeof size_t);
			OutFile.write(reinterpret_cast<const _char*>(Info.strMaskTexture.data()), iNameSize);

			iNameSize = (Info.strDissolveTexture.size() + 1) * sizeof(_tchar);
			OutFile.write(reinterpret_cast<const _char*>(&iNameSize), sizeof size_t);
			OutFile.write(reinterpret_cast<const _char*>(Info.strDissolveTexture.data()), iNameSize);

			iNameSize = (Info.strUnDissolveTexture.size() + 1) * sizeof(_tchar);
			OutFile.write(reinterpret_cast<const _char*>(&iNameSize), sizeof size_t);
			OutFile.write(reinterpret_cast<const _char*>(Info.strUnDissolveTexture.data()), iNameSize);

			iNameSize = Info.strModel.size() + sizeof(_char);
			OutFile.write(reinterpret_cast<const _char*>(&iNameSize), sizeof size_t);
			OutFile.write(Info.strModel.data(), iNameSize);

			OutFile.close();
		}
	}

	return S_OK;
}

HRESULT CImgui_Manager::Override_Data(EffectInfo& Info)
{
	if (MessageBox(g_hWnd, L"기존 정보를 덮어 씁니다.", L"주의", MB_OKCANCEL) == IDCANCEL)
	{
		return S_OK;
	}

	ofstream OutFile(m_CurrFilePath.c_str(), ios::binary);

	if (OutFile.is_open())
	{
		OutFile.write(reinterpret_cast<const _char*>(&Info.iType), sizeof Info.iType);
		OutFile.write(reinterpret_cast<const _char*>(&Info.isSprite), sizeof Info.isSprite);
		OutFile.write(reinterpret_cast<const _char*>(&Info.vNumSprites), sizeof Info.vNumSprites);
		OutFile.write(reinterpret_cast<const _char*>(&Info.fSpriteDuration), sizeof Info.fSpriteDuration);
		OutFile.write(reinterpret_cast<const _char*>(&Info.PartiDesc), sizeof Info.PartiDesc);
		OutFile.write(reinterpret_cast<const _char*>(&Info.iNumInstances), sizeof Info.iNumInstances);
		OutFile.write(reinterpret_cast<const _char*>(&Info.fLifeTime), sizeof Info.fLifeTime);
		OutFile.write(reinterpret_cast<const _char*>(&Info.vColor), sizeof Info.vColor);
		OutFile.write(reinterpret_cast<const _char*>(&Info.iPassIndex), sizeof Info.iPassIndex);
		OutFile.write(reinterpret_cast<const _char*>(&Info.vSize), sizeof Info.vSize);
		OutFile.write(reinterpret_cast<const _char*>(&Info.vPosOffset), sizeof Info.vPosOffset);
		OutFile.write(reinterpret_cast<const _char*>(&Info.vSizeDelta), sizeof Info.vSizeDelta);
		OutFile.write(reinterpret_cast<const _char*>(&Info.bApplyGravity), sizeof Info.bApplyGravity);
		OutFile.write(reinterpret_cast<const _char*>(&Info.vGravityDir), sizeof Info.vGravityDir);
		OutFile.write(reinterpret_cast<const _char*>(&Info.fDissolveDuration), sizeof Info.fDissolveDuration);
		OutFile.write(reinterpret_cast<const _char*>(&Info.bSkipBloom), sizeof Info.bSkipBloom);
		OutFile.write(reinterpret_cast<const _char*>(&Info.fUnDissolveDuration), sizeof Info.fUnDissolveDuration);
		OutFile.write(reinterpret_cast<const _char*>(&Info.vUVInit), sizeof Info.vUVInit);
		OutFile.write(reinterpret_cast<const _char*>(&Info.vUVDelta), sizeof Info.vUVDelta);
		OutFile.write(reinterpret_cast<const _char*>(&Info.isRandomSprite), sizeof Info.isRandomSprite);
		OutFile.write(reinterpret_cast<const _char*>(&Info.hasLight), sizeof Info.hasLight);
		OutFile.write(reinterpret_cast<const _char*>(&Info.Light_Desc), sizeof Info.Light_Desc);
		OutFile.write(reinterpret_cast<const _char*>(&Info.isFixedIndex), sizeof Info.isFixedIndex);
		OutFile.write(reinterpret_cast<const _char*>(&Info.iFixedSpriteIndex), sizeof Info.iFixedSpriteIndex);
		OutFile.write(reinterpret_cast<const _char*>(&Info.isUVLoop), sizeof Info.isUVLoop);
		OutFile.write(reinterpret_cast<const _char*>(&Info.fAlphaInit), sizeof Info.fAlphaInit);
		OutFile.write(reinterpret_cast<const _char*>(&Info.fAlphaDelta), sizeof Info.fAlphaDelta);
		OutFile.write(reinterpret_cast<const _char*>(&Info.fRectRotationAngle), sizeof Info.fRectRotationAngle);
		OutFile.write(reinterpret_cast<const _char*>(&Info.isBillboard), sizeof Info.isBillboard);
		OutFile.write(reinterpret_cast<const _char*>(&Info.vBillboardRotation), sizeof Info.vBillboardRotation);
		OutFile.write(reinterpret_cast<const _char*>(&Info.fPartiDissolveRatio), sizeof Info.fPartiDissolveRatio);
		OutFile.write(reinterpret_cast<const _char*>(&Info.fPartiAppearRatio), sizeof Info.fPartiAppearRatio);

		size_t iNameSize{};
		iNameSize = (Info.strDiffuseTexture.size() + 1) * sizeof(_tchar);
		OutFile.write(reinterpret_cast<const _char*>(&iNameSize), sizeof size_t);
		OutFile.write(reinterpret_cast<const _char*>(Info.strDiffuseTexture.data()), iNameSize);

		iNameSize = (Info.strMaskTexture.size() + 1) * sizeof(_tchar);
		OutFile.write(reinterpret_cast<const _char*>(&iNameSize), sizeof size_t);
		OutFile.write(reinterpret_cast<const _char*>(Info.strMaskTexture.data()), iNameSize);

		iNameSize = (Info.strDissolveTexture.size() + 1) * sizeof(_tchar);
		OutFile.write(reinterpret_cast<const _char*>(&iNameSize), sizeof size_t);
		OutFile.write(reinterpret_cast<const _char*>(Info.strDissolveTexture.data()), iNameSize);

		iNameSize = (Info.strUnDissolveTexture.size() + 1) * sizeof(_tchar);
		OutFile.write(reinterpret_cast<const _char*>(&iNameSize), sizeof size_t);
		OutFile.write(reinterpret_cast<const _char*>(Info.strUnDissolveTexture.data()), iNameSize);

		iNameSize = Info.strModel.size() + sizeof(_char);
		OutFile.write(reinterpret_cast<const _char*>(&iNameSize), sizeof size_t);
		OutFile.write(Info.strModel.data(), iNameSize);

		OutFile.close();
	}

	return S_OK;
}

void CImgui_Manager::Free()
{
	for (auto& pEffect : m_AddEffect)
	{
		Safe_Release(pEffect);
	}
	m_AddEffect.clear();

	Safe_Release(m_pEffect);

	for (_int i = 0; i < m_iNumTextures; i++)
	{
		Safe_Release(m_pTextures[i]);
	}
	Safe_Delete_Array(m_pTextures);

	for (_int i = 0; i < m_iNumModels; i++)
	{
		Safe_Release(m_pModels[i]);
	}
	Safe_Delete_Array(m_pModels);

	//Safe_Release(m_pInstanceShader);
	//Safe_Release(m_pVtxTexShader);
	Safe_Delete_Array(m_pItemList_Texture);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	DestroyContext();
}
