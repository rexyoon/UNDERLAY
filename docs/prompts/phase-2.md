# Phase 2 — Direct3D와 멀티 모니터 (W4–5)

> 원본: `docs/UNDERLAY.pdf` 21장. 아래 블록을 그대로 복사해서 Claude Code에 붙여넣는다.

```text
Phase 2 — Direct3D 렌더링과 멀티 모니터를 시작한다.
읽을 것: CLAUDE.md, docs/PROGRESS.md, docs/spec/04-render-audio.md(렌더링), docs/spec/03-desktop.md(좌표)

[목표] 모든 모니터의 아이콘 아래에서, 시간에 따라 움직이는 테스트 패턴이 Direct3D로 그려진다.

[범위]
1. render: Direct3D 11 장치 1개(디버그 레이어는 Debug 빌드만), 모니터별 스왑체인(Phase 1 스파이크 결과 방식)
2. monitor: 모니터 목록, 모니터별 DPI, 재부팅해도 같은 모니터 식별자, 꽂기/빼기·배율 변경 감지(WM_DISPLAYCHANGE 등),
   모니터별 창을 desktop 모듈로 붙이기, 가상 화면 원점 기준 좌표 변환(음수 좌표)
3. 렌더 스레드: 명령 큐(뮤텍스 + 조건 변수), 프레임 루프, FPS 상한, 모든 모니터 멈춤 시 잠들기
4. 화면: 전체 화면 삼각형 + 테스트 패턴 셰이더(시간으로 색이 흐름) — 셰이더 파일은 네가 쓴다
5. 프레임 박자: 모니터별 vsync 방식과 타이머 방식을 둘 다 재 보고 결과를 ADR로

[내가 칠 파일 — 예상]
core: CommandQueue.h
render: Device.h/.cpp, SwapTarget.h/.cpp, RenderThread.h/.cpp, FullscreenPass.h/.cpp
monitor: MonitorInfo.h, MonitorManager.h/.cpp, MonitorWindow.h/.cpp, VirtualScreen.h/.cpp(좌표 계산, 순수)
테스트: CommandQueueTest.cpp, VirtualScreenTest.cpp

[네가 만들 파일] shaders/fullscreen_vs.hlsl, shaders/test_pattern_ps.hlsl, CMake 셰이더 복사 규칙

[범위 밖] 배경 패키지, 셰이더 핫 리로드(다음 Phase)

[완료 조건]
- 모니터 2대에서 각각 패턴이 움직이고, 보조 모니터를 뽑았다 꽂으면 자동 복구
- 배율 100% / 150%에서 흐릿하지 않음
- 렌더 스레드 종료·재시작 시 누수 경고 없음(디버그 레이어 출력 확인)
- 테스트 통과, 프레임 박자 ADR

[학습 포인트]
- COM과 ComPtr — 참조 카운트, AddRef/Release를 직접 부르지 않는 이유
- HRESULT와 ThrowIfFailed
- 값·참조·포인터, 그리고 "누가 이 객체의 주인인가"
- std::thread, std::mutex, std::condition_variable — Java의 BlockingQueue를 직접 만든다면
- 스왑체인, 렌더 타깃 뷰, Present

먼저 실행 계획을 내고 멈춰라.
```
