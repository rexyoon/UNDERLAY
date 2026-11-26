# Phase 6 — 오디오 반응 → M3 (W12–13)

> 원본: `docs/UNDERLAY.pdf` 22장. 아래 블록을 그대로 복사해서 Claude Code에 붙여넣는다.

```text
Phase 6 — 오디오 반응을 시작한다. 끝나면 M3 "음악에 반응하는 바탕화면"이다. C++ 학습의 하이라이트다.
읽을 것: CLAUDE.md, docs/PROGRESS.md, docs/spec/04-render-audio.md(오디오 반응 전부)

[목표] 지금 PC에서 나오는 소리를 분석해 64개 대역으로 만들고, 원형 음파 셰이더가 음악에 맞춰 움직인다.

[범위]
1. 루프백 캡처: 기본 출력 장치를 WASAPI 루프백으로 열기, 이벤트 방식, 스테레오 → 모노 float, 장치 변경 시 다시 열기,
   데이터가 오지 않으면 일정 시간 후 무음 처리
2. 분석: 슬라이딩 버퍼(최근 1024), 512 샘플마다 Hann 창 → FFT 1024(직접 구현, 반복형 radix-2) → 크기 → dB → 64 로그 대역
   (40Hz~16kHz, 저음 대역 최소 한 칸 보장) → 0~1 정규화 → 다듬기(빠른 상승, 느린 하강)
3. 3중 버퍼(11장 코드)로 공개 → 렌더 스레드가 매 프레임 읽어 iBands, iTimeRes.w(저음)에 넣는다
4. 정책 연결: 모든 모니터가 멈추면 캡처도 정지
5. 설정 화면 전 임시: 디버그 오버레이에 신호 세기(dB)와 대역 막대

[내가 칠 파일 — 예상]
audio: LoopbackCapture.h/.cpp, Fft.h/.cpp, HannWindow.h, BandMapper.h/.cpp, Smoother.h, AudioEngine.h/.cpp
core: TripleBuffer.h
테스트: FftTest.cpp(느린 DFT와 비교, 1kHz 봉우리 위치), BandMapperTest.cpp, SmootherTest.cpp, TripleBufferTest.cpp(두 스레드 스트레스)

[네가 만들 파일] ring-visualizer 셰이더를 오디오 반응으로 업그레이드, 오디오 반응 셰이더 2개 추가(막대 스펙트럼, 저음 펄스)

[범위 밖] 비트 감지, 마이크 입력

[완료 조건]
- 음악을 틀면 원형 음파가 저음·고음에 맞게 움직이고, 멈추면 부드럽게 가라앉는다
- 지연 체감: 메트로놈 영상으로 확인, 결과 기록
- 이어폰을 꽂아 출력 장치가 바뀌어도 계속 반응
- 오디오 테스트 4종 통과

[학습 포인트]
- 오디오 스레드가 절대 기다리면 안 되는 이유
- std::atomic과 메모리 순서(acquire / release) — Java의 volatile·synchronized와 무엇이 다른가
- lock-free 3중 버퍼가 동작하는 원리
- FFT: 비트 반전 순서, 나비 연산, std::complex, constexpr로 상수 만들기
- std::span으로 버퍼를 복사 없이 넘기기

먼저 실행 계획을 내고 멈춰라. FFT는 "느린 DFT + 테스트"를 먼저 치고 빠른 FFT로 바꾸는 순서로 제안해라.
```
