# PROGRESS

## 현재 Phase
Phase 0 (준비) — 2026-11-26 승인, 진행 중

## 완료
- 2026-11-26 킥오프: CLAUDE.md, .claude/settings.json, .claude/hooks/typing-mode.mjs, docs/spec/ (01~07 + questions.md), docs/prompts/ (phase-0~9, always, checklist), docs/PROGRESS.md, docs/TYPING_LOG.md, docs/adr/
- 2026-11-26 훅 확인: Write 도구 → 막힘, Bash 도구 → 막힘, PowerShell 도구 → **막히지 않음** (docs/spec/questions.md Q1)
- 2026-11-26 Q1 반영: 훅이 PowerShell 도구도 검사. 재확인 결과 Write·Bash·PowerShell 모두 막힘
- 2026-11-26 Q2 반영: 테스트 파일(tests/)은 Claude가 작성. 훅의 Write 검사는 src/만, 셸 차단은 src·tests 유지
- 2026-11-26 저장소 구조를 BE/(C++ 앱)·FE/(배경 콘텐츠·웹)로 나눔 (questions.md Q8)
- 2026-11-26 Phase 0 — Claude가 만들 파일 전부 작성 (아직 빌드해 보지 못함):
  - BE/: CMakeLists.txt(underlay_core·underlay.exe·underlay_tests, 없는 파일이 있는 타깃은 건너뜀), CMakePresets.json, vcpkg.json(spdlog, gtest), .clang-format
  - BE/resources/: app.manifest(Per-Monitor V2, UTF-8), app.rc(아이콘·버전 정보), resource.h(IDI_APP_ICON), app.ico(임시)
  - BE/tests/SmokeTest.cpp
  - .gitignore, README.md, .github/workflows/ci.yml, docs/adr/001~003
  - 빈 폴더: BE/src/{app,core,desktop,monitor,render,wallpaper,audio,policy,library,ui}, BE/shaders, BE/sql, BE/installer, FE/wallpapers, FE/bridge

## 진행 중
- 타이핑 차례: [1/5] BE/src/core/Log.h

## 다음 할 일
- Visual Studio 2026 설치 → BE 폴더 열기 → 구성(vcpkg 설치) 확인
- 사용자 타이핑: Log.h → Log.cpp → MainWindow.h → MainWindow.cpp → Main.cpp
- 각 파일 "작성 완료" → 대조 → 빌드 → TYPING_LOG 기록
- docs/spec/questions.md 의 남은 질문(Q4~Q6)에 답하기

## 막힌 것
- Visual Studio 2026 미설치("C++를 사용한 데스크톱 개발" 워크로드 필요) — 설치 전에는 빌드·테스트 불가
- CI: 아직 push 전. push 해도 C++ 파일을 치기 전에는 테스트 타깃만 빌드된다

## 백로그
- (없음)
