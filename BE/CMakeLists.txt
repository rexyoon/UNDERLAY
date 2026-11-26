cmake_minimum_required(VERSION 3.25)

project(underlay VERSION 0.1.0 LANGUAGES CXX RC)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ─────────────────────────────────────────────────────────────
# 타이핑 모드 지원
# C++ 파일은 사용자가 하나씩 친다. 아직 없는 파일이 있는 타깃은 건너뛰고,
# 새 .cpp/.h가 생기면 다음 빌드 때 CMake가 자동으로 다시 구성되게 한다.
# ─────────────────────────────────────────────────────────────
file(GLOB_RECURSE UNDERLAY_WATCHED_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.h
)

# files 중 디스크에 없는 것을 out_var에 모은다
function(underlay_missing_files out_var)
    set(missing "")
    foreach(f IN LISTS ARGN)
        if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${f})
            list(APPEND missing ${f})
        endif()
    endforeach()
    set(${out_var} "${missing}" PARENT_SCOPE)
endfunction()

# 모든 UNDERLAY 타깃이 공유하는 컴파일 옵션 (CLAUDE.md "C++ 코드 규칙")
add_library(underlay_options INTERFACE)
target_compile_options(underlay_options INTERFACE $<$<COMPILE_LANGUAGE:CXX>:/W4 /permissive- /utf-8>)
target_compile_definitions(underlay_options INTERFACE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    UNICODE
    _UNICODE
)

find_package(spdlog CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

# ── underlay_core: 앱과 테스트가 함께 쓰는 정적 라이브러리 ──
set(UNDERLAY_CORE_FILES
    src/core/Log.h
    src/core/Log.cpp
)
underlay_missing_files(core_missing ${UNDERLAY_CORE_FILES})
if(core_missing)
    message(STATUS "[타이핑 모드] underlay_core 건너뜀 — 아직 없는 파일: ${core_missing}")
else()
    add_library(underlay_core STATIC ${UNDERLAY_CORE_FILES})
    target_include_directories(underlay_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
    target_link_libraries(underlay_core
        PUBLIC underlay_options spdlog::spdlog
        PRIVATE Shell32 Ole32
    )
endif()

# ── underlay.exe: Win32 앱 ──
set(UNDERLAY_APP_FILES
    src/app/MainWindow.h
    src/app/MainWindow.cpp
    src/app/Main.cpp
)
underlay_missing_files(app_missing ${UNDERLAY_APP_FILES})
if(app_missing OR NOT TARGET underlay_core)
    message(STATUS "[타이핑 모드] underlay.exe 건너뜀 — 아직 없는 파일: ${core_missing};${app_missing}")
else()
    add_executable(underlay WIN32
        ${UNDERLAY_APP_FILES}
        resources/app.rc
        resources/app.manifest
    )
    target_include_directories(underlay PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/resources)
    target_link_libraries(underlay PRIVATE underlay_core)
endif()

# ── underlay_tests: GoogleTest ──
enable_testing()
include(GoogleTest)

set(UNDERLAY_TEST_FILES
    tests/SmokeTest.cpp
)
add_executable(underlay_tests ${UNDERLAY_TEST_FILES})
target_link_libraries(underlay_tests PRIVATE underlay_options GTest::gtest_main)
if(TARGET underlay_core)
    target_link_libraries(underlay_tests PRIVATE underlay_core)
endif()
# 테스트 목록은 ctest 실행 때 읽는다 (빌드 직후엔 vcpkg DLL 복사 전일 수 있어서)
gtest_discover_tests(underlay_tests DISCOVERY_MODE PRE_TEST)
