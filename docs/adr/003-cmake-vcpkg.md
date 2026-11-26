# ADR 003 — 빌드: CMake + vcpkg (매니페스트 모드)

- 상태: 확정 (2026-11-26, 기획서 7장 투트랙 C)
- 2026-11-26 보충: CMake 프로젝트 루트는 저장소의 `BE/` 폴더다 (저장소 구조 결정, CLAUDE.md)

## 맥락
C++ 프로젝트의 빌드 설정과 외부 라이브러리(spdlog, GoogleTest, 이후 nlohmann/json·SQLite·Dear ImGui·WebView2) 관리 방식을 정해야 한다. 빌드 파일은 Claude Code가 쓰고, 다른 PC와 GitHub Actions에서도 같은 빌드가 나와야 한다.

## 선택지

| 기준 | CMake + vcpkg | Visual Studio 솔루션 + NuGet |
|---|---|---|
| 표준성 | 업계 표준, IDE와 무관 | Visual Studio 중심 |
| 의존성 | vcpkg.json 한 파일로 선언, 버전 고정(builtin-baseline) | NuGet 패키지마다 설정 |
| CI | 같은 프리셋(CMakePresets.json)을 그대로 실행 | MSBuild 설정 따로 |
| 진입 장벽 | 빌드 파일은 Claude Code가 쓰므로 낮다 | 낮다 |

## 결정
CMake + vcpkg 매니페스트 모드. 프리셋은 `windows-debug` / `windows-release` (Ninja + MSVC x64).

## 결과와 트레이드오프
- Visual Studio에서 `BE` 폴더를 열면 CMake 프로젝트로 바로 인식된다.
- 처음 구성할 때 vcpkg가 의존성을 소스에서 빌드하느라 몇 분 걸린다.
- 타이핑 모드를 위해 CMakeLists.txt는 아직 없는 C++ 파일이 있는 타깃을 건너뛰고, 새 파일이 생기면 다시 구성되게 했다.

## 다시 검토할 조건
- vcpkg로 구할 수 없는 의존성이 필요해질 때
