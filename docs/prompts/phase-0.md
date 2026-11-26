※ 2026-11-26 결정: 각 Phase의 테스트 파일(tests/)은 Claude가 쓴다
※ 2026-11-26 결정: C++ 앱은 BE/ 아래(BE/src, BE/tests, CMake 파일 등), 배경 콘텐츠·웹은 FE/ 아래 — 아래 경로는 그 기준으로 읽는다 (CLAUDE.md "저장소 구조")

# Phase 0 — 준비 (W1)

> 원본: `docs/UNDERLAY.pdf` 21장. 아래 블록을 그대로 복사해서 Claude Code에 붙여넣는다.

```text
Phase 0 — 준비를 시작한다.
읽을 것: CLAUDE.md, docs/PROGRESS.md, docs/spec/02-architecture.md, docs/spec/07-roadmap.md

[목표] 빌드·테스트·CI가 돌아가는 빈 C++ 프로젝트와, 화면에 뜨는 첫 Win32 창. 타이핑 모드가 실제로 동작하는지 확인한다.

[네가 만들 파일]
- CMakeLists.txt(타깃 underlay.exe + underlay_core 정적 라이브러리 + 테스트 타깃, /W4 /permissive- /utf-8, C++20),
  CMakePresets.json(windows-debug / windows-release), vcpkg.json(spdlog, gtest — 이번 Phase에 필요한 것만)
- resources/app.manifest(Per-Monitor V2 DPI, UTF-8 코드 페이지), resources/app.rc, 임시 아이콘
- .gitignore, .clang-format, README(빌드 방법), GitHub Actions(windows 러너에서 빌드 + ctest)
- docs/adr/001-native-render-plus-webview.md, 002-dear-imgui.md, 003-cmake-vcpkg.md (7장 표 근거)

[내가 칠 파일 — 예상]
- src/app/Main.cpp (wWinMain — 창 하나 만들고 메시지 루프)
- src/app/MainWindow.h, src/app/MainWindow.cpp (창 클래스 등록·생성·WndProc)
- src/core/Log.h, src/core/Log.cpp (spdlog 초기화, 로그 폴더)
- tests/SmokeTest.cpp (GoogleTest가 도는지 확인하는 테스트 1개)

[범위 밖] 바탕화면 붙이기, Direct3D, 트레이

[완료 조건]
- Visual Studio에서 CMake 폴더를 열고 F5 → 빈 창이 뜨고 닫힌다
- ctest 통과, CI 초록불, %LOCALAPPDATA%\UNDERLAY\logs 에 로그 파일
- 훅 확인: src에 새 .cpp 쓰기 시도가 막힌다
- TYPING_LOG.md에 이번 Phase 파일 기록

[학습 포인트]
- 컴파일과 링크, 헤더와 소스를 나누는 이유, #pragma once
- CMake 타깃(실행 파일 vs 라이브러리)과 vcpkg 매니페스트가 하는 일
- wWinMain, 창 클래스, 메시지 루프, WndProc — Java GUI의 이벤트 디스패치 스레드와 비교
- 와이드 문자열(L"…")이 왜 필요한가

먼저 실행 계획을 내고 멈춰라. 내가 칠 파일은 Log → MainWindow → Main → SmokeTest 순으로 제안해라.
```
