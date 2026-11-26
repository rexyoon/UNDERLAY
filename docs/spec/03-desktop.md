# 03. 바탕화면에 붙이기 (PDF 9장)

> 원본: `docs/UNDERLAY.pdf` 9장. 표·숫자·코드 조각은 원문 그대로, 그림은 글로 풀어 썼다.

이 앱에서 가장 "남의 집" 같은 부분이다. 바탕화면은 탐색기(Explorer)가 그리는 창들의 묶음이고, Microsoft는 그 안에 끼어드는 방법을 공식 문서로 제공하지 않는다. 그래서 공개된 구현 사례를 따르되, 구조를 감지해 분기하고, 붙인 뒤에는 실제로 보이는지 확인한다.

## 구조 그림 (글로 풀어 씀)

### 위에서 내려다본 바탕화면

세 겹의 층이 위에서 아래로 쌓여 있다.

1. **아이콘 레이어** — `SHELLDLL_DefView`
2. **UNDERLAY 창 — 여기에 그린다** — 내 창 (모니터마다 하나)
3. **기본 배경 레이어** — `WorkerW`

아이콘은 그대로 클릭되고, 기본 배경 대신 내 창이 보인다.

### CLASSIC — Windows 10 ~ 11 23H2

```
Progman
WorkerW ── SHELLDLL_DefView (아이콘)
WorkerW (배경)
   └─ UNDERLAY 창   ← 자식으로
```

`0x052C` 메시지 후 아이콘이 별도 `WorkerW`로 옮겨 간다.

### RAISED — Windows 11 24H2 이후

```
Progman (리디렉션 비트맵 없음)
├─ SHELLDLL_DefView (레이어드)
├─ UNDERLAY 창 (자식 + 레이어드)
└─ WorkerW (배경)
```

아이콘이 `Progman` 안에 남는다. 레이어드가 아니면 안 보인다.

## 붙이는 순서

| 단계 | CLASSIC | RAISED (24H2+) |
|---|---|---|
| 1 | `Progman`을 찾고, 배경 레이어가 아직 없을 때만 `0x052C` 메시지를 보낸다 | 같음 |
| 2 | 최상위 창들 중 `SHELLDLL_DefView`를 품은 창을 찾고, 그 바로 다음 `WorkerW`를 고른다 | `Progman`의 확장 스타일과 자식 트리로 RAISED임을 확인한다 |
| 3 | 내 창을 그 `WorkerW`의 자식으로 붙인다 | 내 창에 레이어드 스타일(불투명도 255)을 먼저 주고, 팝업 대신 자식 스타일로 바꾼 뒤 `Progman`의 자식으로 붙인다 |
| 4 | 모니터 크기로 위치 지정 | `SHELLDLL_DefView` 바로 아래, `WorkerW` 위로 Z 순서를 놓는다 |
| 5 | 공통: 붙였다고 믿지 말고 창 사각형·Z 순서·보이는지 확인. 실패하면 로그에 창 트리를 통째로 남긴다 | (공통) |

## 원칙 — 버전 번호가 아니라 구조를 본다

"24H2면 이 방식"처럼 Windows 버전으로 분기하지 않는다. 같은 버전이라도 업데이트에 따라 구조가 다를 수 있다. 실제 창 트리와 `Progman`의 스타일을 보고 판단한다. 이 판단 로직은 창 트리를 인터페이스(`IWindowTree`)로 감싸, 가짜 트리로 단위 테스트한다.

```cpp
// 공개 구현 사례 기준 — Phase 1에서 내 PC의 창 트리를 로그로 찍어 확인한다
enum class DesktopLayout { Classic, Raised, Unknown };

DesktopLayout DetectLayout(const IWindowTree& tree) {
    const std::optional<WindowRef> progman = tree.FindTopLevel(L"Progman");
    if (!progman) return DesktopLayout::Unknown;

    // 24H2+: Progman이 리디렉션 비트맵 없이 합성되고, 아이콘 뷰가 그 안에 남아 있다
    const bool iconsInProgman = tree.FindChild(*progman, L"SHELLDLL_DefView").has_value();
    if (iconsInProgman && tree.HasExStyle(*progman, WS_EX_NOREDIRECTIONBITMAP)) {
        return DesktopLayout::Raised;
    }
    // 그 이전: 아이콘 뷰를 품은 최상위 창 다음에 배경용 WorkerW가 있다
    return tree.FindWorkerWAfterIconHost() ? DesktopLayout::Classic : DesktopLayout::Unknown;
}
```

## 놓치기 쉬운 상황들

| 상황 | 무슨 일이 생기나 | 대응 |
|---|---|---|
| 탐색기 재시작 | 바탕화면 창들이 다 새로 만들어져 내 창이 사라진다 | `TaskbarCreated` 메시지를 받으면 다시 붙이고 트레이 아이콘도 다시 등록 |
| 탐색기가 배경 창을 다시 만듦 | 새 배경 창이 내 창 위로 올라온다 | 5초마다 자기 점검: 크기·Z 순서가 틀어졌으면 다시 맞춘다 |
| 주 모니터 왼쪽의 모니터 | 좌표가 음수다 | 가상 화면 원점 기준으로 바꿔 부모 창 안의 좌표로 배치 |
| 정상 종료 | 내 창이 사라져도 마지막 프레임이 남을 수 있다 | 창을 없앤 뒤 현재 배경 이미지를 다시 적용해 바탕화면을 다시 그리게 한다 |
| 크래시 | 위와 같음 + 정리 코드가 안 돈다 | 처리되지 않은 예외 필터에서 복구 시도, 다음 실행 때 한 번 더 정리 |
| 레이어드 자식 창과 Direct3D | 스왑체인이 화면에 합성되지 않을 수 있다 | Phase 1 마지막에 먼저 검증한다. 안 되면 DirectComposition 방식 스왑체인으로 바꾼다 |
