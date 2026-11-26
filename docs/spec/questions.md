# 질문 모음 (킥오프 세션, 2026-11-26)

PDF를 옮기면서 애매했던 곳이다. **답을 받기 전에는 어느 것도 저장소에 반영하지 않았다.** 답이 정해지면 각 항목 아래에 "결정:"을 적는다.

---

## Q1. 훅이 PowerShell 도구를 검사하지 않는다 — 결정됨(2026-11-26): PowerShell도 검사하도록 수정

**현황.** 19장 `settings.json`의 matcher가 `Write|Bash`이고, 훅 스크립트도 `tool === 'Bash'`만 셸 명령으로 검사한다. 이 환경(Windows의 Claude Code)에는 별도의 `PowerShell` 도구가 있어서, 그 도구로 쓰면 훅이 아예 실행되지 않는다.

**킥오프 훅 확인 결과 (2026-11-26).**

| 시도 | 명령 | 결과 |
|---|---|---|
| Write 도구 | `src/HookCheck.cpp` 쓰기 | **막힘** — `[타이핑 모드] …/src/HookCheck.cpp 는 사용자가 직접 타이핑하는 C++ 파일입니다.` |
| Bash 도구 | `echo "// hook check" > src/HookCheck.cpp` | **막힘** — `[타이핑 모드] 셸 명령으로 src·tests의 C++ 파일을 만들거나 바꿀 수 없습니다.` |
| PowerShell 도구 | `Set-Content -Path src\HookCheck.cpp …` | **막히지 않음** — 파일이 실제로 만들어졌다. 확인 후 바로 지웠다 (`src/` 폴더째, 그 파일 하나뿐이었다) |

19장 "훅에 대해 알아 둘 것"은 "PowerShell 명령(`Set-Content`, `Out-File`, `New-Item`)으로 쓰는 경우까지 막는다"고 적고 있지만, 이는 Bash 도구 안에서 그 명령을 쓸 때만 해당한다.

**참고 사례.** 사용자의 다른 프로젝트 SEASON에서는 matcher를 `Write|Bash|PowerShell`로 바꾸고, 훅 스크립트의 검사 조건도 `tool === 'Bash' || tool === 'PowerShell'`로 바꿨다.

**질문.** UNDERLAY에도 같은 수정을 할까? (PDF 원문에서 벗어나는 수정이라 결정을 기다린다. 결정 전까지는 CLAUDE.md 규칙이 1차 장치로 남는다.)

결정: **결정됨(2026-11-26): PowerShell도 검사하도록 수정.** `.claude/settings.json` matcher → `Write|Bash|PowerShell`, 훅의 셸 검사 조건 → `tool === 'Bash' || tool === 'PowerShell'`, 훅 맨 위에 변경 주석 한 줄.
재확인(2026-11-26): `src/HookCheck.cpp` 쓰기 — Write 막힘 / Bash 막힘 / PowerShell 막힘. 남은 파일 없음.

---

## Q2. 테스트 파일도 사용자가 직접 칠까? — 결정됨(2026-11-26): 테스트는 Claude가 작성

**현황.** PDF 원문(18·19·20장)은 `tests/`의 GoogleTest 파일도 새 C++ 파일이므로 타이핑 모드를 따른다. 훅도 `src/`와 `tests/`를 같이 막는다.

**참고 사례.** SEASON은 2026-11-23에 "테스트 코드는 Claude가 Write로 쓰고, 무엇을 검증하는지 3줄로 설명"하는 방식으로 바꿨다.

**질문.** UNDERLAY도 그렇게 바꿀까? 바꾼다면 함께 손볼 곳:
- CLAUDE.md "테스트 규칙" 첫 줄
- 훅의 `isTypedCpp`·Bash 검사 정규식에서 `tests` 제외
- Phase 0의 `tests/SmokeTest.cpp`를 "내가 칠 파일"에서 "네가 만들 파일"로 옮기기

결정: **결정됨(2026-11-26): 테스트는 Claude가 작성.** (사용자가 이 세션에서 직접 확인)
- CLAUDE.md: 타이핑 모드 첫 항목에 tests/ 예외와 "무엇을 검증하는지 3줄 설명", 테스트 규칙 첫 줄을 "테스트 파일(tests/)은 네가 쓴다. 순수 로직의 기대값·경계값은 테스트를 쓰기 전에 나와 맞춘다"로 수정
- 훅: Write 검사 대상을 `src/`로만 좁힘. 셸(Bash·PowerShell) 쓰기 차단은 `src`·`tests` 둘 다 유지
- docs/prompts/phase-0.md 맨 위에 결정 한 줄 (각 Phase 프롬프트 원문은 그대로)
- 재확인(2026-11-26): `src/HookCheck.cpp` — Write·Bash·PowerShell 모두 막힘 / `tests/HookCheckTest.cpp` — Write 허용(확인 후 지움) / Bash로 `tests/` 쓰기 — 막힘

---

## Q3. (기록) 훅 정규식의 `New-` / `Item` 줄바꿈 복원

19장 `typing-mode.mjs`의 `writes` 정규식은 PDF에서 `…|Out-File|New-` 다음 줄 `Item)/i.test(cmd);`로 나뉘어 있다. 이는 PDF 줄바꿈이므로 `New-Item`으로 이어 붙였다. 내용 변경이 아니라 줄바꿈 복원이다. (질문 아님)

---

## Q4. Phase 7의 `web:` 폴더가 폴더 규칙에 없다

Phase 7 프롬프트(22장)의 "내가 칠 파일"에 `web: BridgeMessage.h/.cpp, WebBridge.h/.cpp`가 있다. 그런데 CLAUDE.md "아키텍처 규칙"과 8장 폴더 표의 폴더는 `src/{app, core, desktop, monitor, render, wallpaper, audio, policy, library, ui}`이고 `web/`이 없다. 8장 표에서는 `wallpaper/`가 "웹 호스트"를 맡고, 8장 그림에는 메인 스레드 안에 "web · WebView2 호스트" 상자가 있다.

**질문.** Phase 7 때 `src/web/`(네임스페이스 `underlay::web`)을 새로 둘까, 아니면 `src/wallpaper/` 안에 둘까? (Phase 7에서 정해도 된다)

결정:

---

## Q5. spec 매핑에 없는 장(1·18·19·24장)의 내용

킥오프 지시는 spec 파일을 2~17장에만 매핑했다. 그래서 아래 내용은 docs/spec/에 옮기지 않았다(일부는 CLAUDE.md·훅·prompts에 이미 들어 있다).

| 장 | 옮기지 않은 내용 | 다른 곳에 있나 |
|---|---|---|
| 1장 | "내가 먼저 정해둔 기본값" 표(이름·플랫폼·언어·렌더링·설정 화면·빌드·저장·타이핑 모드·속도와 대안·근거 위치), "이 문서의 한계" | 대부분 02-architecture.md 7장 표와 겹친다. 표 자체는 없다 |
| 18장 | 준비물 표(Visual Studio 2026 워크로드, Node.js LTS 등), 저장소 구조 그림, "자주 쓰는 한마디" 표 | 없다 (킥오프 프롬프트만 이번 세션에서 실행) |
| 19장 | "누가 무엇을 쓰나" 표, "한 파일이 지나가는 길" 그림, 파일 제시 형식 예시, 규칙 표, "훅에 대해 알아 둘 것" | 규칙은 CLAUDE.md 타이핑 모드에 거의 다 있다. 표·예시는 없다 |
| 24장 | 출처 링크 (바탕화면 구조 4개, Windows 영상 배경, Visual Studio 2026, Claude Code 훅 문서) | 없다. Phase 1(03-desktop.md)의 근거 자료다 |

**질문.** 앞으로 PDF 없이 일하려면 이 내용도 저장소에 있는 편이 낫다. 예를 들어 `docs/spec/00-overview.md`(1·18·19·24장, 원문 그대로)를 추가할까? 아니면 지금처럼 둘까?

결정:

---

## Q6. (낮음) 훅의 `>` 검사가 읽기 전용 명령도 막을 수 있다

Bash 검사의 `writes` 정규식에 `>`가 들어 있어서, `src/…/*.cpp` 경로가 들어간 **읽기 전용** 명령에 `2>/dev/null`이나 `2>&1` 같은 리디렉션이 붙어도 막힌다. 예: `cat src/app/Main.cpp 2>/dev/null`. 동작 확인은 정규식만 보고 판단했고 실제로 돌려 보지는 않았다. 막혀도 안전한 쪽이라 원문 그대로 두었다.

**실제로 발생(2026-11-26).** 첫 커밋 때 메시지에 `tests/SmokeTest.cpp`와 `<…@…>`(Co-Authored-By 줄의 `>`)가 함께 들어가 `git commit`이 막혔다. 메시지를 파일로 빼고 `git commit -F`로 우회했다.

**질문.** 그대로 둘까? (불편해지면 그때 고쳐도 된다)

결정:

---

## Q8. (기록) 저장소 구조를 BE/·FE/로 나눔 — 결정됨(2026-11-26)

사용자 요청 "폴더 이름은 BE, 프론트는 FE로". 원문(18장)의 루트 구조를 이렇게 옮겼다.
- `BE/`: CMakeLists.txt, CMakePresets.json, vcpkg.json, .clang-format, src/, tests/, shaders/, sql/, resources/, installer/
- `FE/`: wallpapers/(배경 패키지), bridge/(다리 스크립트 underlay-bridge.js)
- 루트에 남김: CLAUDE.md, .claude/, docs/, .github/, .gitignore, README.md
- docs/spec·docs/prompts의 원문 경로는 고치지 않고, CLAUDE.md "저장소 구조"에 읽는 규칙을 적었다.
- C++ 앱 전체를 BE로, HTML·JS·배경 패키지를 FE로 나누는 해석을 사용자가 확인했다(2026-11-26).

---

## Q7. (기록) 원문끼리 표기가 다른 곳 — 아래처럼 읽었다

- 8장 그림은 `IWallpaper — update(dt) · draw(target)`라고 줄여 적었고, 10장 코드는 `Update(const FrameInput& input)` · `Draw(ID3D11RenderTargetView* target)`다. 그림은 요약으로 보고 **10장 코드를 기준**으로 읽었다.
- 8장 폴더 표에서 원문은 순수 로직을 굵게 표시했다고 하는데, 텍스트 추출로는 어느 칸이 굵은지 확인할 수 없었다. 02-architecture.md에서는 구조 규칙 1번이 열거한 항목(FFT, 대역 계산, 멈춤 판단, 패키지 검증, 플레이리스트)에 굵게 표시했다. CLAUDE.md의 순수 로직 목록에는 채우기 계산과 구조 감지도 들어 있다.
