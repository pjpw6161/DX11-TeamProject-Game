<div align="center">
  <img src="https://dummyimage.com/800x300/2b2b2b/36bcf7&text=DirectX11+Action+RPG" alt="Project Banner">
  <br/>
  <br/>

  <h1>⚔️ C++ / DirectX 11 3D Action RPG</h1>
  <p>
    <b>게임 플레이와 콘텐츠 제작 파이프라인을 함께 설계한 자체 엔진 프로젝트</b><br/>
    단순 하드코딩을 넘어, 런타임 엔진과 맵/컷신/이펙트/애니메이션 제작 툴 체인을 통합 구축한 대규모 3D 액션 RPG입니다.
  </p>

  <p>
    <img src="https://img.shields.io/badge/C++-00599C?style=flat-square&logo=cplusplus&logoColor=white"/>
    <img src="https://img.shields.io/badge/DirectX%2011-000000?style=flat-square&logo=windows&logoColor=white"/>
    <img src="https://img.shields.io/badge/HLSL-4B0082?style=flat-square&logo=visualstudio&logoColor=white"/>
    <img src="https://img.shields.io/badge/PhysX-2496ED?style=flat-square"/>
  </p>
</div>

---

## 📊 Project Scale
프로젝트의 규모와 자산 관리의 복잡성을 제어하기 위해 시스템을 모듈화했습니다.

| 구분 | 상세 내용 |
| :--- | :--- |
| **Architecture** | `Engine`, `Client`, `MapEditor`, `Effect_Editor`, `AnimTool`, `Shader_Editor`, `FBX_Converter` 총 7개 프로젝트 분리 |
| **Code Base** | 총 748개 파일, 약 150,662 라인 (서드파티 제외) |
| **Resources** | 텍스처 2,212개, 사운드 943개, 커스텀 메시 719개 등 대규모 에셋 운용 |

<br/>

## ✨ Key Features

### 1. 🛠️ 자체 엔진 아키텍처 및 전용 툴 체인 (Engine & Toolchain)
콘텐츠 작업이 코드 수정으로 이어지는 병목 현상을 막기 위해 에디터를 직접 구축했습니다.
- **데이터 기반 파이프라인:** 맵 조형, FastPicking 기반 배치, 이펙트 및 트리거 설정을 데이터 직렬화(`.dat`, `.effect`)로 처리하여 제작 생산성을 높였습니다.
- **일관된 객체 생성:** `Prototype + Layer` 패턴을 도입해 런타임과 에디터 모두 동일한 방식으로 오브젝트를 생성하고 적재합니다.

### 2. 🎨 멀티패스 디퍼드 렌더링 (Deferred Rendering & Post-Processing)
- **MRT 구조 설계:** `RenderTarget`과 `MRT`를 중앙에서 관리하여 지오메트리 패스와 라이트 패스를 완전히 분리했습니다.
- **후처리 및 디버깅:** SSAO, Bloom, Motion Blur, God Ray 등 고급 후처리를 구현했으며, 각 패스의 의존성을 파악하기 위해 디버그 렌더타겟 시각화 툴을 제공합니다.

### 3. ⚔️ 데이터 기반 연출 동기화 및 논리적 충돌 분리
- **프레임 동기화 연출:** `RealtimeVTFModel`과 `AnimTool`을 연동해, 특정 애니메이션 프레임에 이펙트/사운드 트리거가 자동 발동하도록 설계했습니다.
- **물리와 판정의 레이어 분리:** 이동과 환경 제어는 `PhysX_Manager`가, 전투 액션 판정(공격, 피격, 패링)은 `Collision_Manager`가 전담하도록 시스템 목적에 맞춰 분리했습니다.

<br/>

## 🕵️‍♂️ Troubleshooting

> 💡 **제목을 클릭하면 상세한 문제 해결 과정을 확인할 수 있습니다.**

<details>
<summary><b>1. 전투 카메라 시점 튐 및 장애물 클리핑 문제 해결</b></summary>
<br/>

- **문제:** 공격 줌, 피격 흔들림, 지형 충돌이 겹치며 전투 중 시야가 크게 튀는 현상 발생
- **해결:** `CCamera_Main` 기준으로 상태를 분리하고, 진입 시 초기값 재설정 및 Lerp 보간 적용. 플레이어-카메라 간 Raycast를 쏴 장애물 감지 시 카메라를 전진시켜 벽을 파고드는 문제를 해결
- **성과:** 전투 시점 안정화 및 타격감 가독성 향상
</details>

<details>
<summary><b>2. 원거리 전투의 락온(Lock-on) 및 조준 일관성 확보</b></summary>
<br/>

- **문제:** 락온 여부에 따라 플레이어 회전, 활 발사 방향, UI 타겟 표시가 서로 엇갈려 조작감이 떨어짐
- **해결:** 카메라 콜라이더 기반으로 `Get_Nearest_MonsterCollider`를 통해 최근접 타겟을 선정. 이후 플레이어 LookAt, UI 타겟 갱신, 화살 생성 로직이 모두 동일한 타겟(MonCollider)을 참조하도록 파이프라인 통일
- **성과:** 수동 조준과 자동 보정 간의 어색함을 줄이고 일관된 조작감 제공
</details>

<details>
<summary><b>3. 대규모 자산 로딩 시 프레임 프리징 병목 완화</b></summary>
<br/>

- **문제:** 수천 개의 텍스처, 메시, 데이터 파일이 한 프레임에 로드되며 초기 진입 및 레벨 전환 시 멈춤 현상 발생
- **해결:** 로딩 전용 레벨과 `_beginthreadex` 기반의 별도 로딩 스레드를 구축하여, 레벨별 리소스를 비동기적으로 단계적 적재
</details>

<br/>

## 📸 Play Gallery
*실제 인게임 전투 및 플레이 영상입니다.*

<div align="center">
  <img src="https://github.com/user-attachments/assets/b8adc805-910f-417f-8a45-c179eaeceeea" width="49%" alt="play footage 1" />
  <img src="https://github.com/user-attachments/assets/f1182860-baa5-4007-85b9-e1db60c18d77" width="49%" alt="play footage 2" />
  <br/><br/>
  <img src="https://github.com/user-attachments/assets/4e886c8d-e3cc-49a6-bde1-93ced8746fb9" width="49%" alt="play footage 3" />
  <img src="https://github.com/user-attachments/assets/08b4f5c9-f24c-450f-9072-a57fc47b5bae" width="49%" alt="play footage 4" />
  <br/><br/>
  <img src="https://github.com/user-attachments/assets/276f1a29-2e88-406f-bfef-333b3a5a51ab" width="80%" alt="play footage 5" />
</div>

<br/>
<br/>

## 💡 Retrospective
이 프로젝트를 통해 단순히 게임 기능을 구현하는 것을 넘어, **"복잡한 파이프라인을 구조적으로 관리하고 지속 가능한 콘텐츠 생산 환경을 설계하는 법"**을 깊이 배웠습니다. 특히 렌더링 패스의 디버깅 환경을 구축하고, 수작업을 줄이기 위해 전용 에디터 5종을 분리 개발한 경험은 '개발 생산성'과 '데이터 주도 설계'의 중요성을 깨닫게 해준 소중한 자산이 되었습니다.
