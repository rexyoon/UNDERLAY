# Phase 3 — 셰이더 배경 (W6–7)

> 원본: `docs/UNDERLAY.pdf` 21장. 아래 블록을 그대로 복사해서 Claude Code에 붙여넣는다.

```text
Phase 3 — 셰이더 배경과 핫 리로드를 시작한다.
읽을 것: CLAUDE.md, docs/PROGRESS.md, docs/spec/01-product.md(배경 종류·패키지), docs/spec/04-render-audio.md(상수 버퍼)

[목표] wallpapers/ 폴더의 셰이더 패키지를 띄우고, .hlsl 파일을 저장하는 순간 바탕화면이 바뀐다.

[범위]
1. wallpaper: IWallpaper 인터페이스(10장), FrameInput(시간, 커서 위치, 오디오 대역 자리 — 지금은 0), ShaderWallpaper
2. 상수 버퍼: 10장의 cbuffer와 바이트 단위로 같은 C++ 구조체(static_assert로 크기·정렬 확인), 조절값 iProps 배정
3. 셰이더 컴파일: 실행 중 D3DCompile, 실패하면 이전 셰이더를 유지하고 오류 문자열을 로그 + 디버그 오버레이
4. 핫 리로드: 패키지 폴더 감시(ReadDirectoryChangesW), 200ms 디바운스, 렌더 스레드에 "다시 컴파일" 명령
5. library: project.json 읽기·검증(형식 번호, 종류, entry 존재, 조절값 타입) — 순수 로직
6. 커서 위치: 매 프레임 커서 좌표를 모니터 기준 0~1로 변환 (훅 금지)
7. 트레이 메뉴 "다음 배경"으로 wallpapers/ 안의 셰이더를 순서대로 바꾸기 (라이브러리 UI는 Phase 8)

[내가 칠 파일 — 예상]
wallpaper: IWallpaper.h, FrameInput.h, ShaderWallpaper.h/.cpp, FrameConstants.h
render: ShaderCompiler.h/.cpp
core: FileWatcher.h/.cpp
library: Package.h/.cpp, PropertyValue.h
테스트: PackageTest.cpp(정상·비정상 json), FrameConstantsTest.cpp(크기·오프셋)

[네가 만들 파일] wallpapers/에 샘플 셰이더 패키지 3개(aurora, noise-flow, ring-visualizer — 오디오 값 0에서도 보기 좋게),
각 project.json과 preview.jpg, vcpkg.json에 nlohmann-json 추가

[범위 밖] 영상, 오디오 분석

[완료 조건]
- 샘플 3개가 모든 모니터에서 돌고, 트레이 "다음 배경"으로 바뀐다
- 셰이더에 일부러 오타를 내고 저장 → 이전 화면 유지 + 오버레이에 오류, 고치고 저장 → 바로 반영
- 테스트 통과

[학습 포인트]
- 순수 가상 함수와 가상 소멸자 — 왜 virtual ~IWallpaper()가 필요한가
- std::unique_ptr로 다형 객체 소유하기
- 구조체 메모리 배치, alignas, static_assert
- 파일 감시와 비동기 I/O 기초, JSON 파싱

먼저 실행 계획을 내고 멈춰라.
```
