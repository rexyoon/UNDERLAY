# UNDERLAY

바탕화면 아이콘 아래에서 움직이는 배경 엔진. 셰이더·영상·이미지·웹 배경을 그리고, 지금 흐르는 음악에 반응하며, 게임을 켜면 스스로 멈추는 Windows 11용 라이브 월페이퍼. C++20 · Win32 · Direct3D 11.

> 개발 중 (Phase 0). 기획·설계는 [docs/spec/](docs/spec/), 진행 상황은 [docs/PROGRESS.md](docs/PROGRESS.md).

## 저장소 구조

```
underlay/
├── BE/                 C++ 앱 (CMake 프로젝트 루트)
│   ├── CMakeLists.txt  CMakePresets.json  vcpkg.json
│   ├── src/            app · core · desktop · monitor · render · wallpaper · audio · policy · library · ui
│   ├── tests/          GoogleTest
│   └── shaders/  sql/  resources/  installer/
├── FE/                 배경 콘텐츠와 웹
│   ├── wallpapers/     배경 패키지 (폴더 하나 = 배경 하나)
│   └── bridge/         웹 배경용 JS 다리 스크립트
└── docs/               기획서 원본(UNDERLAY.pdf) · spec · prompts · adr
```

## 빌드

### 준비물
- Windows 11 (24H2 이후 권장)
- Visual Studio 2026 — "C++를 사용한 데스크톱 개발" 워크로드 (MSVC, Windows SDK, CMake 도구, vcpkg 포함)
- Git, Node.js LTS (Claude Code 타이핑 모드 훅 실행용)

### Visual Studio에서
1. Visual Studio로 `BE` 폴더를 연다 (파일 → 열기 → 폴더).
2. 위쪽 구성 목록에서 `windows-debug`를 고른다. 처음에는 vcpkg가 spdlog·gtest를 받아 빌드하느라 몇 분 걸린다.
3. 시작 항목을 `underlay.exe`로 두고 F5.

### 명령줄에서 (Developer PowerShell for VS, `BE` 폴더에서)
```powershell
cmake --preset windows-debug
cmake --build --preset windows-debug
ctest --preset windows-debug --output-on-failure
```

실행 파일: `BE/build/windows-debug/underlay.exe` · 로그: `%LOCALAPPDATA%\UNDERLAY\logs\underlay.log`

`VCPKG_ROOT` 환경 변수가 vcpkg 폴더를 가리켜야 한다. Visual Studio의 개발자 명령 프롬프트에는 기본으로 잡혀 있다.

## 라이선스

미정 (Phase 9에서 추가)
