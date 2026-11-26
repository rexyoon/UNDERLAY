# 05. 일시정지 정책과 라이브러리·저장 (PDF 12~13장)

> 원본: `docs/UNDERLAY.pdf` 12~13장. 표·숫자·코드 조각은 원문 그대로 옮겼다.

---

## 12. 일시정지 정책

좋은 라이브 월페이퍼의 조건은 "예쁘다"보다 "필요 없을 때 확실히 쉰다"다. 조건을 이벤트로 모으는 부분(Windows 의존)과, 모은 사실로 판단하는 부분(순수 함수)을 나눈다. 판단 함수는 표 한 장으로 테스트한다.

### 멈추는 조건과 감지 방법

| 조건 | 감지 | 범위 |
|---|---|---|
| 전체화면 앱·게임 | 포그라운드 창 변화 이벤트(WinEvent 훅) → 그 창이 모니터를 통째로 덮는지 확인 + 전체화면 상태 API로 보조 확인 | 해당 모니터 |
| 최대화 창이 화면을 가림 | 같은 이벤트 + 모니터별 보이는 최대화 창 확인. 다른 가상 데스크톱에 숨은(클로킹) 창은 제외 | 해당 모니터 (설정으로 끌 수 있음) |
| 배터리 사용 | 전원 변경 메시지 + 전원 상태 조회 | 전체 (설정) |
| 화면 잠금 | 세션 변경 알림 (잠금·해제) | 전체 |
| 화면 꺼짐 | 디스플레이 상태 전원 알림 | 전체 |
| 원격 데스크톱 | 원격 세션 여부 + 세션 변경 | 전체 |
| 사용자가 멈춤 | 트레이 메뉴 | 전체 또는 모니터 |

Alt+Tab처럼 짧게 오가는 순간에 깜빡이지 않도록 상태 변화는 300ms 동안 유지될 때만 반영한다(디바운스).

```cpp
// 운영체제를 모르는 순수 함수 — 표 하나로 단위 테스트한다
enum class PauseReason { None, UserPaused, Locked, DisplayOff, Remote, Fullscreen, Covered, Battery };

struct SystemState { bool onBattery, sessionLocked, remoteSession, displayOff; };
struct MonitorState { bool fullscreenApp, coveredByMaximized; };
struct PolicySettings { bool pauseWhenCovered = true; bool pauseOnBattery = true; };

PauseReason Decide(const SystemState& s, const MonitorState& m,
                   const PolicySettings& p, bool userPaused) {
    if (userPaused)                                 return PauseReason::UserPaused;
    if (s.sessionLocked)                            return PauseReason::Locked;
    if (s.displayOff)                               return PauseReason::DisplayOff;
    if (s.remoteSession)                            return PauseReason::Remote;
    if (m.fullscreenApp)                            return PauseReason::Fullscreen;
    if (p.pauseWhenCovered && m.coveredByMaximized) return PauseReason::Covered;
    if (p.pauseOnBattery && s.onBattery)            return PauseReason::Battery;
    return PauseReason::None;
}
```

### "멈춘다"의 뜻 — 배경 종류마다 다르다

| 대상 | 멈출 때 | 다시 켤 때 |
|---|---|---|
| 셰이더 | 그리기·내보내기(Present) 중단 | 시간 값을 이어서 재개 (멈춘 시간만큼 튀지 않게) |
| 영상 | Media Engine 일시정지 — 디코딩도 멈춘다 | 같은 위치에서 재생 |
| 웹 | JS에 `pause` 이벤트, 가능하면 WebView2 일시 중단 기능 | `pause(false)` 이벤트 |
| 오디오 분석 | 모든 모니터가 멈추면 캡처도 중지 | 하나라도 재생되면 다시 연다 |
| 렌더 스레드 | 모든 모니터가 멈추면 명령이 올 때까지 잠든다 | 명령 큐 신호로 깨어남 |

---

## 13. 라이브러리와 저장

배경 자체는 폴더(패키지)로, 내 선택과 설정은 SQLite 한 파일로 저장한다. 스키마 파일(.sql)은 Claude Code가 쓰고, 그것을 읽어 적용하는 C++ 코드는 내가 친다.

| 위치 | 내용 |
|---|---|
| `설치 폴더\wallpapers\` | 기본 제공 샘플 배경 |
| 사용자가 추가한 폴더 | 내 배경들 (여러 개 등록 가능) |
| `%LOCALAPPDATA%\UNDERLAY\library.db` | 라이브러리 목록, 모니터 배정, 조절값, 플레이리스트, 설정 |
| `%LOCALAPPDATA%\UNDERLAY\logs\` · `crashes\` · `cache\` | 로그, 크래시 덤프, 썸네일 캐시 |

### SQLite 스키마 (sql/001_init.sql — Claude Code 작성)

```sql
CREATE TABLE wallpaper (
    id             INTEGER PRIMARY KEY,
    path           TEXT NOT NULL UNIQUE,
    type           TEXT NOT NULL CHECK (type IN ('shader', 'video', 'image', 'web')),
    title          TEXT NOT NULL,
    audio_reactive INTEGER NOT NULL DEFAULT 0,
    favorite       INTEGER NOT NULL DEFAULT 0,
    added_at       TEXT NOT NULL,
    last_used_at   TEXT
);

CREATE TABLE property_value (
    wallpaper_id INTEGER NOT NULL REFERENCES wallpaper(id) ON DELETE CASCADE,
    key          TEXT NOT NULL,
    value_json   TEXT NOT NULL,
    PRIMARY KEY (wallpaper_id, key)
);

CREATE TABLE monitor_assignment (
    monitor_key  TEXT PRIMARY KEY,           -- 재부팅해도 바뀌지 않는 모니터 식별자
    wallpaper_id INTEGER REFERENCES wallpaper(id) ON DELETE SET NULL,
    fit_mode     TEXT NOT NULL DEFAULT 'cover',
    fps_cap      INTEGER NOT NULL DEFAULT 60
);

CREATE TABLE playlist (id INTEGER PRIMARY KEY, name TEXT NOT NULL, interval_minutes INTEGER NOT NULL, shuffle INTEGER NOT NULL DEFAULT 0);

CREATE TABLE playlist_item (playlist_id INTEGER NOT NULL REFERENCES playlist(id) ON DELETE CASCADE,
                            wallpaper_id INTEGER NOT NULL REFERENCES wallpaper(id) ON DELETE CASCADE,
                            position INTEGER NOT NULL, PRIMARY KEY (playlist_id, position));

CREATE TABLE setting (key TEXT PRIMARY KEY, value TEXT NOT NULL);
```

### 규칙

| 규칙 | 내용 |
|---|---|
| 마이그레이션 | `sql/001_init.sql`, `002_….sql` 순서로 실행. 적용 번호는 SQLite의 `user_version`에 기록. .sql 파일은 실행 파일 리소스로 묶는다(CMake — Claude Code) |
| 모니터 식별자 | 모니터를 꽂는 순서가 바뀌어도 같은 값이 나오는 장치 경로를 쓴다 (디스플레이 구성 API) |
| 패키지 검증 | `project.json`의 형식 번호·종류·entry 파일 존재·조절값 타입을 검사. 실패하면 라이브러리에 "오류" 표시만 하고 앱은 계속 돈다 |
| SQLite 핸들 | RAII 래퍼로 감싼다. 열기·닫기·문장(statement) 해제를 손으로 하지 않는다 |
