# Phase 1 — 바탕화면에 붙이기 → M1 (W2–3)

> 원본: `docs/UNDERLAY.pdf` 21장. 아래 블록을 그대로 복사해서 Claude Code에 붙여넣는다.

```text
Phase 1 — 바탕화면에 붙이기를 시작한다. 끝나면 M1 "아이콘 뒤에 색이 깔린다"다.
읽을 것: CLAUDE.md, docs/PROGRESS.md, docs/spec/03-desktop.md (전부)

[목표] 바탕화면 아이콘은 그대로 두고, 그 아래에 내 창을 끼워 그라데이션을 그린다. 종료·탐색기 재시작에도 바탕화면이 깨지지 않는다.

[범위]
1. core: RAII 래퍼(HWND, HANDLE, HICON — unique_ptr + 사용자 삭제자), UTF-8 ↔ UTF-16 변환, ThrowIfFailed
2. desktop: IWindowTree 인터페이스 + Win32 구현, 구조 감지(CLASSIC / RAISED / UNKNOWN), 붙이기(두 방식),
   붙인 뒤 보이는지 검증, 5초 자기 점검, 종료 시 복구, 감지·붙이기 실패 시 창 트리 전체를 로그로
3. 화면: 주 모니터 하나에 GDI로 그라데이션(WM_PAINT) — Direct3D는 다음 Phase
4. app: 트레이 아이콘(메뉴: 일시정지 자리, 종료), 단일 실행(이름 있는 뮤텍스, 두 번째 실행은 기존 앱에 알림),
   TaskbarCreated 메시지로 탐색기 재시작 감지 → 다시 붙이기 + 트레이 재등록
5. 마지막 단위 — 검증 스파이크: 붙인 창 위에 Direct3D 11 스왑체인 하나를 만들어 단색으로 Present 했을 때
   바탕화면에 보이는지 확인. RAISED에서 안 보이면 DirectComposition 방식으로 시도하고 결과를 ADR로 남긴다

[내가 칠 파일 — 예상]
core: Win32Handle.h, StringConvert.h/.cpp, ThrowIfFailed.h
desktop: IWindowTree.h, Win32WindowTree.h/.cpp, LayoutDetector.h/.cpp, DesktopHost.h/.cpp
app: TrayIcon.h/.cpp, SingleInstance.h/.cpp, App.h/.cpp (Main.cpp 수정은 diff로)
테스트: LayoutDetectorTest.cpp (가짜 트리: CLASSIC, RAISED, Progman 없음), StringConvertTest.cpp

[네가 만들 파일] 트레이 아이콘·메뉴 리소스, vcpkg.json 갱신(필요 시), 스파이크 결과 ADR 초안

[범위 밖] 멀티 모니터(다음 Phase), 셰이더·영상

[완료 조건]
- 내 PC에서 아이콘 뒤에 그라데이션이 보이고, 아이콘 클릭·드래그가 정상
- 디버그 로그에 감지된 구조(CLASSIC 또는 RAISED)와 창 트리가 남는다
- 작업 관리자에서 탐색기 재시작 → 5초 안에 다시 보임, 트레이 아이콘도 복구
- 트레이에서 종료 → 원래 배경으로 돌아가고 잔상 없음
- LayoutDetectorTest, StringConvertTest 통과
- Direct3D 스파이크 결과 기록

[학습 포인트]
- HWND와 창 트리, 부모·자식·Z 순서
- RAII와 사용자 삭제자 — "스코프를 벗어나면 반드시 해제" (Java의 try-with-resources와 차이)
- 인터페이스(순수 가상 함수)로 Windows를 감싸 테스트 가능하게 만드는 법
- 람다를 Win32 콜백(EnumWindows)으로 넘기는 법과 LPARAM으로 상태 전달하기
- 이름 있는 뮤텍스, 등록된 윈도 메시지

먼저 실행 계획을 내고 멈춰라. 감지 로직(LayoutDetector + 테스트)을 붙이기보다 먼저 치는 순서로 제안해라.
```
