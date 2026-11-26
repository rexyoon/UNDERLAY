# Phase 7 — 웹 배경 (W14–15)

> 원본: `docs/UNDERLAY.pdf` 22장. 아래 블록을 그대로 복사해서 Claude Code에 붙여넣는다.

```text
Phase 7 — 웹 배경을 시작한다.
읽을 것: CLAUDE.md, docs/PROGRESS.md, docs/spec/04-render-audio.md(웹 배경), docs/spec/03-desktop.md

[목표] HTML·JS로 만든 배경을 바탕화면에 띄우고, 웹 배경도 음악 대역 값과 조절값을 받는다.

[범위]
1. 먼저 검증: RAISED 구조의 레이어드 자식 창 안에서 WebView2가 보이는지 확인. 안 되면 대안(시각 호스팅 방식 등)을 조사해 ADR로
2. WebWallpaper: WebView2 환경·컨트롤러 비동기 생성(메인 스레드), 패키지 폴더를 가상 호스트 이름에 연결,
   시작 시 다리 스크립트 주입, 크기 변경, 일시정지 시 pause 이벤트(+ 가능하면 일시 중단)
3. 다리: 3중 버퍼에서 대역 값을 읽어 초당 30회 JSON 메시지로 전송, 조절값 변경 전송
4. 렌더 스레드와의 경계: 웹 배경인 모니터는 스왑체인 대신 WebView2를 쓰도록 모니터 창 구성 전환

[내가 칠 파일 — 예상]
wallpaper: WebWallpaper.h/.cpp
web: BridgeMessage.h/.cpp(JSON 조립 — 순수), WebBridge.h/.cpp
테스트: BridgeMessageTest.cpp

[네가 만들 파일] wallpapers/web-ring(캔버스로 그린 원형 음파 웹 버전), wallpapers/minimal-clock, 다리 스크립트(underlay-bridge.js),
vcpkg.json에 webview2 추가

[범위 밖] 인터넷 데이터를 쓰는 위젯(날씨 등)

[완료 조건]
- 웹 원형 음파가 음악에 반응하고, 게임을 켜면 멈춘다
- 웹 배경 메모리 사용량을 측정해 README에 기록
- BridgeMessageTest 통과, 검증 결과 ADR

[학습 포인트]
- 비동기 완료 핸들러 사슬(콜백 안에서 다음 객체 만들기)
- STA(단일 스레드 아파트)와 "UI 스레드에서만" 규칙
- JSON 만들기와 문자열 이스케이프

먼저 실행 계획을 내고 멈춰라. 1번 검증을 첫 단위로 해라.
```
