#include "ImGui_Mgr.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CImGui_Mgr)

CImGui_Mgr::CImGui_Mgr()
{
}

HRESULT CImGui_Mgr::Init(_dev pDevice, _context pContext, CGameInstance* pGameInstance, CRenderer* pRenderer)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\gulim.ttc", 18.f, NULL, io.Fonts->GetGlyphRangesKorean());

	if (false == ImGui_ImplWin32_Init(g_hWnd))
		return E_FAIL;

	if (false == ImGui_ImplDX11_Init(pDevice, pContext))
		return E_FAIL;

	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	m_pGameInstance = pGameInstance;
	Safe_AddRef(m_pGameInstance);

	m_pRenderer = pRenderer;
	Safe_AddRef(m_pRenderer);

	m_SSAO = m_pRenderer->Get_SSAO();

	m_fSSAOBlurPower = m_pRenderer->Get_SSAOBlurPower();

	m_fEffectBlurPower = m_pRenderer->Get_EffectBlurPower();

	m_HDR = m_pRenderer->Get_HDR();

	m_fHDRBlurPower = m_pRenderer->Get_HDRBlurPower();

	return S_OK;
}

void CImGui_Mgr::Frame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin(u8"뽀큐");
	if (ImGui::BeginTabBar(u8"쉐이더")) {
		if (ImGui::BeginTabItem(u8"아쎄이 AO")) {
			ImGui::DragFloat(u8"Intensity(강도)", &m_SSAO.fIntensity, 0.005f, 0.001f, 100.f);
			ImGui::DragFloat(u8"Radius(반지름)", &m_SSAO.fRadius, 0.001f, 0.001f, 5.f);
			ImGui::DragFloat(u8"Scale(사이 거리)", &m_SSAO.fScale, 0.005f, 0.001f, 100.f);
			ImGui::DragFloat(u8"Bias(차폐물의 너비)", &m_SSAO.fBias, 0.005f, 0.001f, 100.f);
			ImGui::DragFloat(u8"SSAOBlurPower", &m_fSSAOBlurPower, 0.001f, 0.001f, 10.f);

			m_pRenderer->Set_SSAO(m_SSAO);
			m_pRenderer->Set_SSAOBlurPower(m_fSSAOBlurPower);

			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem(u8"똥맵")) {
			ImGui::DragFloat(u8"MiddleGray(중간 회색값)", &m_HDR.fMiddleGray, 0.005f, 0.01f, 100.f);
			ImGui::DragFloat(u8"LumWhiteSqr(하이라이트 클리핑)", &m_HDR.fLumWhiteSqr, 0.005f, 0.01f, 10.f);
			ImGui::DragFloat(u8"HDRBlurPower", &m_fHDRBlurPower, 0.001f, 0.001f, 10.f);

			m_pRenderer->Set_HDR(m_HDR);
			m_pRenderer->Set_HDRBlurPower(m_fHDRBlurPower);

			ImGui::EndTabItem();
		}


		ImGui::EndTabBar();
	}
	ImGui::End();
}

void CImGui_Mgr::Editing(_float fTimeDelta)
{
}

void CImGui_Mgr::Render()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

}

void CImGui_Mgr::Free()
{
	Safe_Release(m_pRenderer);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
