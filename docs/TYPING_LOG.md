# TYPING_LOG

빌드·테스트를 통과한, 내가 직접 친 C++ 파일 기록. "이 파일은 복사로" 예외로 넘어간 파일은 표시한다.

| 날짜 | 파일 | 줄 수 | 이 파일에서 배운 C++ 개념 |
|---|---|---|---|
| 2026-11-26 | BE/src/core/Log.h | 6 | `#pragma once`, 중첩 네임스페이스 `underlay::core`, 헤더에는 선언만 |
| 2026-11-26 | BE/src/core/Log.cpp | 54 | 익명 네임스페이스(파일 안 전용), Windows가 준 메모리를 `CoTaskMemFree`로 돌려주기, `std::filesystem::path`의 `/` 연산자, `constexpr` |
| 2026-11-26 | BE/src/app/MainWindow.h | 21 | `explicit` 생성자, `= delete`로 복사 막기, 멤버 기본값(`= nullptr`), `static` 멤버 함수(WndProc) |
| 2026-11-26 | BE/src/app/MainWindow.cpp | 83 | 생성자 초기화 목록, RAII(소멸자가 창·클래스 정리), `this`를 `GWLP_USERDATA`에 묶는 패턴, `static_cast` vs `reinterpret_cast`, `switch` fall-through |
| 2026-11-26 | BE/src/app/Main.cpp | 37 | `wWinMain`, 메시지 루프, 스택 객체의 수명(스코프 끝에서 소멸), 이름 없는 매개변수(C4100 회피), 예외를 `const&`로 잡기 |
