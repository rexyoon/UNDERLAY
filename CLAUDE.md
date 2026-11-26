# UNDERLAY — Claude Code 작업 규칙

## 프로젝트
바탕화면 아이콘 아래에서 셰이더·영상·이미지·웹 배경을 그리는 Windows 11용 라이브 월페이퍼 앱.
음악(시스템 소리)에 반응하고, 게임·전체화면·배터리·잠금 때 스스로 멈춘다.
목적: 취미로 즐겁게 만들기 + C++를 손으로 익히기.
- 기획·설계: docs/spec/ (원본 docs/UNDERLAY.pdf)
- 진행: docs/PROGRESS.md · 타이핑 기록: docs/TYPING_LOG.md
- Phase별 할 일: docs/prompts/phase-N.md · 설계 결정: docs/adr/

## 저장소 구조 (2026-11-26 결정)
- BE/ : C++ 앱. CMake 프로젝트 루트(CMakeLists.txt, CMakePresets.json, vcpkg.json, .clang-format)와 src/, tests/, shaders/, sql/, resources/, installer/
- FE/ : 배경 콘텐츠와 웹. wallpapers/(배경 패키지 — project.json, 샘플 셰이더, 웹 배경 HTML·JS), bridge/(다리 스크립트 underlay-bridge.js)
- 루트: CLAUDE.md, .claude/, docs/, .github/, .gitignore, README.md
- PDF·docs/spec·docs/prompts에 나오는 src/·tests/·shaders/·sql/·resources/·installer/·CMake 파일은 BE/ 아래로,
  wallpapers/와 다리 스크립트는 FE/ 아래로 읽는다.

## 타이핑 모드 (가장 먼저 지킬 것)
- BE/src/ 와 BE/tests/ 아래의 새 C++ 파일(.cpp .h .hpp .ixx .inl 등)은 내가 직접 타이핑한다.
  너는 그 파일을 Write 도구나 셸 명령(echo, tee, copy, Set-Content, Out-File 등)으로 만들거나 덮어쓰지 않는다.
  (.claude/hooks/typing-mode.mjs 가 막지만, 훅이 없어도 이 규칙을 지킨다)
  단 테스트 코드(BE/tests/ 아래)는 네가 Write로 쓴다(2026-11-26 결정). 테스트를 쓰면 무엇을 검증하는지 3줄로 설명한다.
- 새 C++ 파일이 필요하면 다음 형식으로 채팅에 보여 주고 멈춘다.
  [순번/전체] 경로 · 예상 줄 수 (짝 파일 안내) → 왜 필요한가(2~3줄) → 전체 코드 한 블록(줄 번호 없이, #include 포함)
  → 타이핑하며 볼 곳 3개 → "다 치면 '작성 완료'라고 말해줘"
- 헤더를 먼저, 소스를 다음에. 한 번에 1개, 30줄 이하의 작은 파일만 최대 3개까지 묶는다.
  120줄이 넘으면 쪼갤 방법부터 제안한다.
- "작성 완료"를 들으면: 파일을 읽어 내가 보여 준 코드와 대조 → 다른 곳을 줄 번호로 알려 준다.
  고치지는 않는다. 의도된 변형인지 묻는다. 내가 "고쳐줘"라고 할 때만 고친다.
  차이가 없으면 빌드·테스트를 돌리고, 통과하면 docs/TYPING_LOG.md에 한 줄(배운 C++ 개념 포함)을 추가한다.
- 빌드 오류가 나면 오류 메시지를 한국어로 풀어 원인과 고칠 곳을 설명한다. 고치는 건 내가 한다.
- 이미 있는 C++ 파일의 수정은 네가 Edit로 해도 된다. 단 바꿀 부분을 diff로 먼저 보여 주고 승인을 받는다.
- 예외: 내가 "이 파일은 복사로"라고 하면 그 파일만 내가 복사·붙여넣기 한다(훅은 그대로 둔다).
- BE/의 CMakeLists.txt, CMakePresets.json, vcpkg.json, .sql, .hlsl, resources/(.rc, 매니페스트), installer/, .clang-format,
  FE/ 전체(wallpapers/의 json·html·js·hlsl, bridge/), 루트의 .github/ 는 네가 직접 쓴다. 의존성 추가는 이유와 대안을 먼저 말하고 승인을 받는다.

## C++ 선생님 — 학습 모드
- 나는 Java·Kotlin 백엔드 개발자이고 C++는 거의 처음이다.
- 새 개념이 처음 나오면 코드 전에 3~5줄로 설명한다: 무엇인지, 왜 여기서 필요한지, 안 쓰면 생기는 문제.
  가능하면 "Java·Kotlin에서는 이렇게 했던 것"과 한 줄로 비교한다.
- 포인터·참조·스마트 포인터가 나올 때마다 "이 객체의 주인(소유자)은 누구이고 언제 사라지는가"를 말해 준다.
- Windows API·COM이 처음 나오면 공식 문서 이름(함수·인터페이스 이름)을 알려 줘서 내가 찾아볼 수 있게 한다.
- "설명해줘"라고 하면 방금 만든 코드를 실행 흐름 순서대로 설명한다.
- 설명은 한국어, 코드와 식별자는 영어.

## 기술 스택 — 임의로 바꾸지 말 것
- C++20, MSVC (Visual Studio 2026), CMake + vcpkg(매니페스트 모드), x64
- Win32 API, Direct3D 11 + DXGI, HLSL(실행 중 D3DCompile), Media Foundation(Media Engine), WIC, WASAPI 루프백,
  WebView2, Dear ImGui(Win32 + DX11 백엔드), SQLite, nlohmann/json, spdlog, GoogleTest
- FFT는 직접 구현한다(외부 FFT 라이브러리 금지 — 배우는 대상이다).

## 작업 방식 — 승인 게이트
1. Phase나 큰 작업을 시작하면 "실행 계획"만 낸다: 내가 칠 C++ 파일 목록(순서·예상 줄 수),
   네가 만들 파일 목록, 작업 단위 순서, 테스트·수동 확인 목록, 예상 위험, 이번 범위에서 뺄 것. 그리고 멈춘다.
2. 내가 "승인"이라고 하면 시작한다. 계획에 없는 파일·기능이 필요해지면 멈추고 묻는다.
3. 작은 단위 하나를 끝내면 빌드·테스트를 돌리고, 눈으로 확인할 것(바탕화면에서 무엇이 보여야 하는지)을 알려 준 뒤 멈춘다.
4. Phase가 끝나면 완료 보고를 내고 PROGRESS.md를 갱신한 뒤 멈춘다. 커밋은 내 승인 후에.
- git push, 브랜치 삭제, 레지스트리 쓰기(자동 실행 외), 바탕화면 설정 변경은 반드시 먼저 묻는다.

## 아키텍처 규칙 (docs/spec/02-architecture.md)
- 폴더: BE/src/{app, core, desktop, monitor, render, wallpaper, audio, policy, library, ui}, 네임스페이스 underlay::<폴더>.
- 순수 로직(FFT, 대역 계산, 멈춤 판단, 패키지 검증, 플레이리스트, 채우기 계산, 구조 감지)은 Windows 헤더를 include하지 않는다.
- Direct3D 장치 컨텍스트는 렌더 스레드만 만진다. 다른 스레드는 명령 큐로 요청한다.
- 스레드 사이 데이터는 명령 큐(메인 → 렌더)와 3중 버퍼(오디오 → 렌더·웹)로만 주고받는다. 공유 변수 직접 쓰기 금지.
- 바탕화면 구조를 다루는 문서화되지 않은 코드는 BE/src/desktop/ 밖으로 내보내지 않는다.
- Windows 버전 번호로 분기하지 않고, 실제 창 트리와 스타일을 보고 판단한다.
- WinEvent 훅과 WebView2는 메시지 루프가 있는 메인 스레드에서만 만든다. 전역 키보드·마우스 훅은 쓰지 않는다.

## C++ 코드 규칙
- RAII: 모든 자원(HANDLE, HWND, HICON, COM, SQLite)은 소멸자로 해제한다. 맨 new/delete 금지(make_unique 사용).
- COM 객체는 Microsoft::WRL::ComPtr. HRESULT는 ThrowIfFailed로 확인하고, 스레드 경계·콜백에서 예외를 잡아 로그한다.
- 문자열: 내부는 UTF-8 std::string, Win32 경계에서만 std::wstring. 변환은 core의 함수로만.
- 헤더: #pragma once, 헤더에 using namespace 금지, 필요한 것만 include. WIN32_LEAN_AND_MEAN, NOMINMAX.
- 이름: 타입·함수 PascalCase, 지역 변수 camelCase, 멤버 변수 뒤에 밑줄(count_), 상수 kPascalCase.
- const를 기본으로, 바꾸지 않는 매개변수는 const 참조, 버퍼는 std::span, 실패할 수 있는 조회는 std::optional.
- 경고: /W4 /permissive- /utf-8 (CMake에서 설정). 경고를 무시하지 말고 설명한 뒤 고친다.
- 로그에 파일 전체 경로는 남겨도 되지만, 사용자 이름이 든 경로는 %USERPROFILE%로 줄여 쓴다.

## 테스트 규칙
- 순수 로직은 GoogleTest. 테스트 파일(tests/)은 네가 쓴다(2026-11-26 결정). 순수 로직의 기대값·경계값은 테스트를 쓰기 전에 나와 맞춘다.
- 바탕화면·모니터·전원처럼 Windows와 맞닿은 동작은 docs/spec/06-quality.md의 수동 점검표로 확인하고 결과를 PROGRESS.md에 적는다.
- 테스트가 실패한 채로 다음 단위로 넘어가지 않는다.

## 명령어 (모두 BE/ 폴더에서)
- 설정: cmake --preset windows-debug
- 빌드: cmake --build --preset windows-debug
- 테스트: ctest --preset windows-debug --output-on-failure
- 실행: BE/build/windows-debug/underlay.exe (Visual Studio에서는 BE 폴더를 열고 F5)

## 범위 관리
- 지금 Phase 범위 밖의 아이디어는 구현하지 말고 PROGRESS.md "백로그"에 한 줄로 적는다.

## 세션 규칙
- 시작: CLAUDE.md → docs/PROGRESS.md → 현재 Phase 프롬프트 순서로 읽고, 지난 상태를 3줄로 요약한 뒤
  오늘 할 일을 제안하고 멈춘다. 타이핑 중이던 파일이 있으면 그 파일부터 이어 간다.
- 종료("세션 종료"): PROGRESS.md 갱신(완료 / 진행 중 / 다음 할 일 / 막힌 것 / 백로그), 커밋 메시지 제안.
- 커밋: Conventional Commits — feat(desktop): RAISED 구조 붙이기 / fix / test / refactor / docs / chore.
