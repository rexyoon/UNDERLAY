# Phase 4 — 영상 · 이미지 배경 → M2 (W8–9)

> 원본: `docs/UNDERLAY.pdf` 21장. 아래 블록을 그대로 복사해서 Claude Code에 붙여넣는다.

```text
Phase 4 — 영상과 이미지 배경을 시작한다. 끝나면 M2 "영상 배경 앱"이다.
읽을 것: CLAUDE.md, docs/PROGRESS.md, docs/spec/04-render-audio.md(영상·채우기 계산)

[목표] mp4를 하드웨어 디코딩으로 반복 재생하고, 이미지도 배경으로 쓴다. 화면 채우기 방식을 고른다.

[범위]
1. VideoWallpaper: Media Foundation Media Engine을 같은 Direct3D 장치(DXGI 장치 관리자)로 열기, 반복 재생, 음소거,
   새 프레임마다 텍스처로 옮겨 그리기, 일시정지 시 디코딩도 정지, 재생 오류 로그
2. Media Engine 알림 콜백 객체(COM 인터페이스 구현)
3. ImageWallpaper: WIC로 이미지 읽어 텍스처
4. 채우기 계산(cover / contain / stretch) — 순수 함수, 10장 울트라와이드 예시로 테스트
5. 텍스처를 화면에 그리는 공용 패스(위치·배율 상수)
6. 하드웨어 디코딩 여부를 로그와 디버그 오버레이에 표시

[내가 칠 파일 — 예상]
wallpaper: VideoWallpaper.h/.cpp, MediaEngineNotify.h/.cpp, ImageWallpaper.h/.cpp
render: FitMode.h/.cpp(순수), TexturedQuad.h/.cpp
테스트: FitModeTest.cpp

[네가 만들 파일] shaders/quad_ps.hlsl, 샘플 영상(ffmpeg로 직접 만든 짧은 반복 영상 — 저작권 문제 없게), 샘플 이미지,
각 project.json

[범위 밖] 영상 소리, 재생 목록(Phase 8)

[완료 조건]
- 1080p 샘플 영상이 끊김 없이 반복 재생, CPU 사용률을 측정해 README에 기록
- 채우기 방식 3종이 울트라와이드·일반 모니터에서 맞게 보인다
- FitModeTest 통과
- M2: 하루 동안 영상 배경을 켜 두고 쓰기 — 불편한 점을 PROGRESS.md에

[학습 포인트]
- COM 인터페이스를 내가 구현하기(IUnknown: QueryInterface·AddRef·Release)
- 콜백이 다른 스레드에서 올 때의 주의점
- 텍스처, 셰이더 리소스 뷰, 샘플러
- 정수·실수 변환과 반올림이 화면 픽셀에 주는 영향

먼저 실행 계획을 내고 멈춰라.
```
