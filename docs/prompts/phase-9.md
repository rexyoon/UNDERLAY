# Phase 9 — 마무리 · 배포 → M4 (W18)

> 원본: `docs/UNDERLAY.pdf` 22장. 아래 블록을 그대로 복사해서 Claude Code에 붙여넣는다.

```text
Phase 9 — 마무리와 배포를 시작한다. 끝나면 M4 "설치 파일로 배포"다.
읽을 것: CLAUDE.md, docs/PROGRESS.md, docs/spec/06-quality.md

[목표] 하루 종일 켜 두어도 안전하고, 다른 사람이 설치해서 쓸 수 있는 상태.

[범위]
1. 크래시 처리: 처리되지 않은 예외 필터 → 덤프 파일 → 바탕화면 복구 시도, 다음 실행 때 비정상 종료 알림
2. 장치 손실·절전 복귀 대응(기존 파일 수정은 diff로)
3. 성능 점검: 06장 성능 지표를 하나씩 측정해 README 표로
4. 수동 점검표 전체 1회 (가능하면 CLASSIC 가상 머신 포함)
5. 배포: Release 빌드, zip, 설치 파일(Inno Setup), GitHub Actions 릴리스, README(데모 GIF, 구조 그림, 배운 것)

[내가 칠 파일 — 예상]
core: CrashHandler.h/.cpp
render: DeviceLostHandler.h/.cpp
테스트: (필요 시) CrashHandler의 순수 부분

[네가 만들 파일] installer/underlay.iss, 릴리스 워크플로, README, 데모 GIF 촬영 안내, 라이선스 파일

[범위 밖] 코드 서명, 자동 업데이트

[완료 조건]
- 크래시 테스트 메뉴로 강제 크래시 → 덤프 생성 + 바탕화면 정상
- 성능 지표 표 완성, 수동 점검표 전체 통과
- GitHub 릴리스에 설치 파일

[학습 포인트]
- 예외 안전성과 noexcept, 소멸자에서 예외를 던지면 안 되는 이유
- 크래시 덤프를 Visual Studio로 열어 보는 법
- Debug와 Release 빌드의 차이(최적화, 디버그 레이어)

먼저 실행 계획을 내고 멈춰라.
```
