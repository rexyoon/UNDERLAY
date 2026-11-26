# Phase 5 — 일시정지 정책 (W10–11)

> 원본: `docs/UNDERLAY.pdf` 22장. 아래 블록을 그대로 복사해서 Claude Code에 붙여넣는다.

```text
Phase 5 — 일시정지 정책을 시작한다.
읽을 것: CLAUDE.md, docs/PROGRESS.md, docs/spec/05-policy-library.md(일시정지)

[목표] 게임·전체화면·가림·배터리·잠금·화면 꺼짐·원격일 때 스스로 멈추고, 멈춘 이유를 보여 준다. 멈춘 동안 GPU를 쓰지 않는다.

[범위]
1. 순수 판단: PauseReason, SystemState, MonitorState, PolicySettings, Decide() (12장 코드)
2. 디바운스: 300ms 유지될 때만 상태 변경 — 순수 로직, 시간은 주입
3. 신호 수집: 포그라운드·위치 변경 WinEvent 훅, 전원 변경, 세션 잠금 알림, 디스플레이 상태 알림, 원격 세션,
   모니터별 전체화면·최대화 판정(다른 가상 데스크톱에 숨은 창 제외)
4. 정책 엔진: 모니터별로 Decide → 바뀐 것만 렌더 스레드에 명령
5. 배경별 멈춤 동작(12장 표): 셰이더 시간 이어 가기, 영상 일시정지
6. 트레이에 "2번 모니터 정지 · 전체화면 앱" 같은 이유 표시, 수동 일시정지 메뉴

[내가 칠 파일 — 예상]
policy: PauseTypes.h, Decide.h/.cpp, Debouncer.h/.cpp, SignalCollector.h/.cpp, PolicyEngine.h/.cpp
테스트: DecideTest.cpp(조합 표), DebouncerTest.cpp

[네가 만들 파일] 트레이 메뉴 리소스 갱신, 수동 점검표 체크 결과 템플릿

[범위 밖] 오디오 캡처 정지(Phase 6에서 연결)

[완료 조건]
- 전체화면 게임 실행 → 해당 모니터만 정지, 작업 관리자 GPU 사용 ≈ 0
- Alt+Tab 연타에도 깜빡이지 않음, 잠금 → 정지, 해제 → 재개
- DecideTest, DebouncerTest 통과, 수동 점검표 해당 줄 결과 기록

[학습 포인트]
- enum class와 switch, 빠뜨린 경우를 경고로 잡기
- 시간을 주입해 테스트 가능한 코드 만들기
- WinEvent 훅 콜백과 메시지 루프의 관계
- 표 기반(파라미터화) 테스트

먼저 실행 계획을 내고 멈춰라. Decide와 테스트를 먼저 치는 순서로 제안해라.
```
