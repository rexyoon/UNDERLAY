# 04. 렌더링 파이프라인과 오디오 반응 (PDF 10~11장)

> 원본: `docs/UNDERLAY.pdf` 10~11장. 표·숫자·코드 조각은 원문 그대로, 그림은 글로 풀어 썼다.

---

## 10. 렌더링 파이프라인

Direct3D 장치는 하나, 모니터마다 창과 스왑체인(화면에 내보낼 버퍼 묶음)이 하나씩이다. 배경은 모두 `IWallpaper` 약속을 따르고, 렌더 스레드는 매 프레임 "멈추지 않은 모니터"만 그린다.

```cpp
// 모든 배경이 따르는 약속. 엔진은 지금 어떤 종류를 그리는지 모른다.
class IWallpaper {
public:
    virtual ~IWallpaper() = default;
    virtual void Load(const Package& pkg, RenderContext& ctx) = 0;
    virtual void Resize(UINT width, UINT height) = 0;
    virtual void SetProperty(std::string_view key, const PropertyValue& value) = 0;
    virtual void Update(const FrameInput& input) = 0; // 시간, 마우스, 오디오 대역
    virtual void Draw(ID3D11RenderTargetView* target) = 0;
    virtual void SetPaused(bool paused) = 0;           // 영상은 디코딩까지 멈춘다
};
```

### 프레임 루프

| 순서 | 하는 일 |
|---|---|
| 1 | 명령 큐를 비운다 (배경 교체, 일시정지, 모니터 변경, 조절값) |
| 2 | 오디오 스냅샷에서 최신 대역 값을 읽는다 (기다리지 않음) |
| 3 | 멈추지 않은 모니터마다 Update → Draw → Present |
| 4 | FPS 상한(기본 60, 모니터별 설정)에 맞춰 다음 프레임까지 쉰다 |
| 5 | 모든 모니터가 멈췄으면 명령이 올 때까지 잠든다 — GPU·CPU 거의 0 |

### Phase 2에서 측정해서 정할 것

모니터마다 수직 동기화(vsync)를 걸고 차례로 내보내면 서로 기다리느라 FPS가 반토막 날 수 있다. 주 모니터 기준으로 박자를 맞출지, 타이머로 맞출지는 두 방식을 다 재 보고 ADR로 남긴다.

### 셰이더 배경 — 상수 버퍼

셰이더는 매 프레임 상수 버퍼로 값을 받는다. 상수 버퍼는 16바이트 단위로 정렬되므로 `float4`로 묶고, C++ 구조체와 바이트 단위로 똑같이 맞춘다. 이 "메모리 배치를 맞추는 일"이 C++에서 처음 배우는 중요한 감각이다.

```hlsl
cbuffer Frame : register(b0) {
    float4 iTimeRes;   // x: 시간(초) y: 너비 z: 높이 w: 저음 에너지(0~1)
    float4 iMouse;     // xy: 커서 위치(0~1, 모니터 기준) zw: 예비
    float4 iBands[16]; // 64개 주파수 대역(0~1), float4 하나에 4개씩
    float4 iProps[4];  // project.json의 조절값 (float 16칸, 색은 4칸씩 차지)
};
```

| 조절값 연결 | 방식 |
|---|---|
| 셰이더 | `project.json`의 `properties` 순서대로 `iProps`의 16칸에 배정. 색은 4칸(RGBA), 슬라이더는 1칸 |
| 영상 | 재생 속도, 채우기 방식 |
| 웹 | JS 이벤트 `property`로 전달 |

마우스는 훅을 쓰지 않는다. 내 창은 아이콘 아래에 있어 마우스 메시지를 받지 못하므로, 매 프레임 커서 위치만 읽어 넘긴다. 전역 훅은 백신 오탐의 흔한 원인이기도 하다.

### 영상 배경 — 화면 채우기 계산

| 방식 | 계산 | 예: 1920×1080 영상 → 3440×1440 울트라와이드 |
|---|---|---|
| 꽉 채우기 (cover) | 배율 = max(가로비, 세로비), 넘치는 부분 잘라냄 | 배율 1.792 → 3440×1935, 위아래 247.5px씩 잘림 |
| 맞추기 (contain) | 배율 = min(가로비, 세로비), 남는 곳은 검게 | 배율 1.333 → 2560×1440, 좌우 440px씩 검은 띠 |
| 늘리기 (stretch) | 가로·세로 따로 | 3440×1440, 비율 왜곡 |

영상은 Media Foundation의 Media Engine으로 연다. 같은 Direct3D 장치를 넘겨주면 하드웨어 디코딩된 프레임을 텍스처로 바로 받을 수 있다. 반복 재생, 음소거, 일시정지는 엔진 기능으로 처리한다.

### 웹 배경 — WebView2와 JS 다리

웹 배경은 모니터 창 안에 WebView2를 띄운다. 패키지 폴더를 가상 호스트 이름에 연결해 로컬 파일을 웹 페이지처럼 열고, 시작 시 작은 다리 스크립트를 넣어 아래 API를 제공한다(다리 스크립트와 샘플 웹 배경은 Claude Code가 쓴다).

```js
window.underlay.on('audio', (bands) => { /* 길이 64 배열, 0~1, 초당 30회 */ });
window.underlay.on('property', ({ key, value }) => { /* 설정 화면에서 바꾼 조절값 */ });
window.underlay.on('pause', (paused) => { /* true면 애니메이션 멈추기 */ });
```

---

## 11. 오디오 반응

지금 스피커로 나가는 소리를 받아(루프백) 주파수별 세기로 바꾸고, 부드럽게 다듬어 화면에 넘긴다. 음악을 듣는 앱이 무엇이든 상관없다. 이 장이 C++ 학습의 하이라이트다. 스레드, 원자 연산, 메모리 순서, 그리고 FFT.

### 처리 단계 (그림을 글로 풀어 씀)

| 단계 | 이름 | 설명 |
|---|---|---|
| ① | 루프백 캡처 | 48kHz 스테레오 → 모노 float |
| ② | 슬라이딩 버퍼 | 최근 1024 샘플, 512마다 분석 |
| ③ | 창 함수 | Hann window, 경계 튐 줄이기 |
| ④ | FFT 1024 | 직접 구현, 512칸 × 46.9Hz |
| ⑤ | 64 대역 | 40Hz~16kHz 로그 간격, dB → 0~1 |
| ⑥ | 다듬기 | 빨리 오르고 천천히 내림 |
| ⑦ | 3중 버퍼 공개 | 렌더 · 웹이 최신 값만 읽음 |

①~⑥은 오디오 스레드 하나에서 돈다. 렌더 스레드와는 ⑦ 하나로만 만난다.

### 숫자로 보는 설계

| 항목 | 값 | 이유 |
|---|---|---|
| 샘플링 · FFT 크기 | 48,000Hz · 1,024 샘플 | 한 칸 폭 = 48,000 ÷ 1,024 = 46.9Hz. 창 길이 21.3ms |
| 분석 간격 | 512 샘플(10.7ms)마다 | 창을 절반씩 겹쳐 초당 약 94번 갱신 |
| 대역 | 64개, 40Hz~16kHz 로그 간격 | k번째 경계 = 40 × 400^(k/64). 가운데(k=32)는 800Hz — 사람 귀처럼 저음을 촘촘히 |
| 세기 | dB로 바꾼 뒤 −60~0dB → 0~1 | 작은 소리도 보이게 |
| 다듬기 | 오를 때 빠르게, 내릴 때 천천히 (지수 평활) | 비트에 즉각 반응하면서도 깜빡이지 않게 |
| 지연 예산 | 창 21ms + 캡처 주기 10ms + 프레임 17ms ≈ 50ms | 06장 목표와 같다 |

### 직접 부딪혀 볼 함정 두 가지

- **저음 대역은 FFT 한 칸보다 좁다.** 40~60Hz 사이에 대역이 여러 개인데 칸 폭은 46.9Hz다. 대역마다 최소 한 칸은 보장하고, 같은 칸을 나눠 쓰는 대역은 보간한다.
- **아무 소리도 안 나면 캡처 데이터가 아예 안 올 수 있다.** 일정 시간 데이터가 없으면 무음으로 보고 대역을 0으로 내린다. 출력 장치가 바뀌면(이어폰 연결 등) 캡처를 다시 연다.

### 3중 버퍼 — 기다리지 않는 전달

오디오 스레드는 절대 기다리면 안 되고(소리를 놓친다), 렌더 스레드도 기다리면 안 된다(프레임이 밀린다). 그래서 칸 세 개를 원자적으로 돌려 쓰는 3중 버퍼를 쓴다. 쓰는 쪽은 언제나 빈 칸에 쓰고, 읽는 쪽은 언제나 가장 최근 완성본을 본다. 락이 없다.

```cpp
// 쓰는 쪽 1개, 읽는 쪽 1개. 둘 다 서로를 기다리지 않는다.
template <typename T>
class TripleBuffer {
    std::array<T, 3> slots_{};
    std::atomic<int> middle_{1}; // 하위 2비트: 칸 번호, 4: 새 값 있음
    int back_ = 0;               // 쓰는 쪽 전용
    int front_ = 2;              // 읽는 쪽 전용
public:
    T& Back() { return slots_[back_]; }
    void Publish() {             // 다 쓴 back과 middle을 맞바꾼다
        back_ = middle_.exchange(back_ | 4, std::memory_order_acq_rel) & 3;
    }
    const T& Read() {            // 새 값이 있을 때만 middle과 front를 맞바꾼다
        if (middle_.load(std::memory_order_acquire) & 4) {
            front_ = middle_.exchange(front_, std::memory_order_acq_rel) & 3;
        }
        return slots_[front_];
    }
};
```
