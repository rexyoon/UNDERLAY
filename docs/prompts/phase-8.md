# Phase 8 — 라이브러리 · 설정 UI (W16–17)

> 원본: `docs/UNDERLAY.pdf` 22장. 아래 블록을 그대로 복사해서 Claude Code에 붙여넣는다.

```text
Phase 8 — 라이브러리와 설정 화면을 시작한다.
읽을 것: CLAUDE.md, docs/PROGRESS.md, docs/spec/05-policy-library.md(라이브러리·SQLite), docs/spec/01-product.md(화면 구조)

[목표] 설정 창에서 배경을 고르고, 모니터별로 지정하고, 조절값·멈춤 규칙·플레이리스트를 다룬다. 다음 실행 때 그대로 복원된다.

[범위]
1. SQLite: RAII 래퍼(DB, 문장), 마이그레이션 실행기(리소스로 묶인 .sql을 user_version 기준으로 적용)
2. 라이브러리 스캐너: 기본·사용자 폴더 훑기 → 패키지 검증 → DB 반영, 썸네일 캐시
3. 플레이리스트: 다음 배경 계산(순서·셔플·간격) — 순수 로직
4. 설정 창(Dear ImGui, 보통 최상위 창, 열려 있을 때만 그림):
   라이브러리 격자, 모니터 배치도, 조절값 자동 UI(project.json 선언 기반), 멈춤 규칙 토글, 오디오 미터, 성능 패널, 일반(자동 실행)
5. 자동 실행: 현재 사용자 Run 레지스트리 (켜고 끄기, 쓰기 전 확인)
6. 시작 시 DB에서 모니터 배정·조절값 복원

[내가 칠 파일 — 예상]
library: Sqlite.h/.cpp, Migrator.h/.cpp, LibraryDb.h/.cpp, LibraryScanner.h/.cpp, Playlist.h/.cpp
ui: SettingsWindow.h/.cpp, LibraryPanel.cpp, MonitorPanel.cpp, PropertiesPanel.cpp, PolicyPanel.cpp, AudioPanel.cpp
app: AutoStart.h/.cpp
테스트: PlaylistTest.cpp, LibraryDbTest.cpp(메모리 DB), MigratorTest.cpp

[네가 만들 파일] sql/001_init.sql(13장), 필요하면 002_*.sql, .sql을 리소스로 묶는 CMake 규칙, vcpkg.json에 sqlite3·imgui 추가

[범위 밖] 웹 UI, 테마

[완료 조건]
- 설정 창에서 모니터별로 배경을 바꾸고 조절값을 움직이면 즉시 반영
- 앱을 껐다 켜면 같은 상태
- 플레이리스트 간격 교체 동작
- 테스트 통과

[학습 포인트]
- 외부 C 라이브러리를 RAII로 감싸기(sqlite3*, sqlite3_stmt*)
- 템플릿 기초 — 문장 바인딩 함수를 타입별로
- 즉시 모드 UI의 사고방식(매 프레임 UI를 다시 그린다)

먼저 실행 계획을 내고 멈춰라.
```
