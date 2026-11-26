// Phase 0 연기 테스트: GoogleTest가 빌드·실행되는지와 C++20 설정이 켜져 있는지만 확인한다.
#include <gtest/gtest.h>

#include <span>
#include <string_view>

TEST(Smoke, GoogleTestRuns) {
    EXPECT_EQ(1 + 1, 2);
}

TEST(Smoke, Cpp20IsEnabled) {
    // MSVC는 /Zc:__cplusplus 없이 __cplusplus를 199711L로 두므로 _MSVC_LANG을 본다.
#if defined(_MSVC_LANG)
    EXPECT_GE(_MSVC_LANG, 202002L);
#else
    EXPECT_GE(__cplusplus, 202002L);
#endif
    constexpr int kValues[] = {1, 2, 3};
    const std::span<const int> view{kValues};
    EXPECT_EQ(view.size(), 3u);
}

TEST(Smoke, SourceIsUtf8) {
    // /utf-8 옵션이 없으면 한글 리터럴의 바이트 수가 달라진다 ("한" = UTF-8 3바이트).
    constexpr std::string_view kHangul = "한";
    EXPECT_EQ(kHangul.size(), 3u);
}
