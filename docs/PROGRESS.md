# PROGRESS

## 현재 Phase
Phase 0 (준비) — 2026-11-26 승인. 완료 조건 4개 중 3개 확인, 남은 것은 C++ 코드를 포함한 커밋을 push한 뒤 CI 초록불 확인

## 완료
- 2026-11-26 킥오프: CLAUDE.md, .claude/settings.json, .claude/hooks/typing-mode.mjs, docs/spec/ (01~07 + questions.md), docs/prompts/ (phase-0~9, always, checklist), docs/PROGRESS.md, docs/TYPING_LOG.md, docs/adr/
- 2026-11-26 훅 확인: Write 도구 → 막힘, Bash 도구 → 막힘, PowerShell 도구 → **막히지 않음** (docs/spec/questions.md Q1)
- 2026-11-26 Q1 반영: 훅이 PowerShell 도구도 검사. 재확인 결과 Write·Bash·PowerShell 모두 막힘
- 2026-11-26 Q2 반영: 테스트 파일(tests/)은 Claude가 작성. 훅의 Write 검사는 src/만, 셸 차단은 src·tests 유지
- 2026-11-26 저장소 구조를 BE/(C++ 앱)·FE/(배경 콘텐츠·웹)로 나눔 (questions.md Q8)
- 2026-11-26 Phase 0 — Claude가 만든 파일:
  - BE/: CMakeLists.txt(underlay_core·underlay.exe·underlay_tests, 없는 파일이 있는 타깃은 건너뜀), CMakePresets.json, vcpkg.json(spdlog, gtest), .clang-format
  - BE/resources/: app.manifest(Per-Monitor V2, UTF-8), app.rc(아이콘·버전 정보), resource.h(IDI_APP_ICON), app.ico(임시)
  - BE/tests/SmokeTest.cpp
  - .gitignore, README.md, .github/workflows/ci.yml, docs/adr/001~003
  - 빈 폴더: BE/src/{app,core,desktop,monitor,render,wallpaper,audio,policy,library,ui}, BE/shaders, BE/sql, BE/installer, FE/wallpapers, FE/bridge
- 2026-11-26 Phase 0 — 사용자가 친 C++ 파일 5개 (TYPING_LOG.md에 기록):
  - BE/src/core/Log.h (5줄 오타 "고쳐줘"로 수정), Log.cpp (차이 없음)
  - BE/src/app/MainWindow.h (18줄 instance_ "고쳐줘"로 수정), MainWindow.cpp (차이 9곳 → 사용자가 고침)
  - BE/src/app/Main.cpp (15줄 whiel "고쳐줘"로 수정. GetMessage·"code{}" 띄어쓰기는 그대로)
- 2026-11-26 첫 빌드: Visual Studio 2026 Insiders(18, MSVC 19.52, SDK 10.0.26100), VS 내장 vcpkg. 구성(vcpkg 설치 3.3분) → 빌드 경고 0 → ctest 3/3 통과
- 2026-11-26 Phase 0 완료 조건 확인:
  - [x] Visual Studio에서 BE 폴더 열고 F5 → 빈 창이 뜨고 닫힌다 (사용자 확인)
  - [x] ctest 3/3 통과 · %LOCALAPPDATA%\UNDERLAY\logs\underlay.log 생성 (started / starting / main window created / destroyed / exiting, 종료 코드 0)
  - [ ] CI 초록불 — ee6f194(C++ 코드 없이 테스트 타깃만)는 success. C++ 코드를 포함한 커밋은 push 후 확인
  - [x] 훅 확인: src에 새 .cpp 쓰기 시도가 Write·Bash·PowerShell 모두 막힘 (킥오프 때 확인)
  - [x] TYPING_LOG.md에 이번 Phase 파일 5개 기록

## 진행 중
- Phase 0 커밋 → push → CI 확인

## 다음 할 일
- CI 초록불 확인 → Phase 0 완료 표시 → Phase 1 실행 계획 (docs/prompts/phase-1.md)
- docs/spec/questions.md 의 남은 질문(Q4~Q6)에 답하기

## 막힌 것
- (없음)

## 백로그
- Main.cpp: `GetMessage` → `GetMessageW`로 맞추기, 로그 문구 `(code{})` 띄어쓰기 (동작 영향 없음)
