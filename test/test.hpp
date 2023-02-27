// Copyright (c) 2022 Dennis Hezel
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef DEHE_TEST_TEST_HPP
#define DEHE_TEST_TEST_HPP

#include <test/framework.hpp>
#include <test/utility.hpp>
#include <variate/variate.hpp>

#include <string>
#include <string_view>

namespace test
{
void run_boost_tests();

inline void test_make_single_element_variant()
{
    auto func = []
    {
        static constexpr dehe::Variate var;
        return var(42);
    };
    auto v = dehe::make_variant(func());
    CHECK(std::is_same_v<decltype(v), std::variant<int>>);
    CHECK_EQ(42, std::get<0>(v));
}

inline void test_make_c_style_array_decay()
{
    auto func = [](bool ok)
    {
        static constexpr dehe::Variate var;
        if (ok)
        {
            return var(1.5f);
        }
        return var("a very very long test test");
    };
    auto v = dehe::make_variant(func(false));
    CHECK(std::is_same_v<decltype(v), std::variant<float, const char*>>);
    CHECK_EQ(std::string_view("a very very long test test"), std::get<1>(v));
}

inline void test_make_move_only()
{
    auto func = [](int ok)
    {
        static constexpr dehe::Variate var;
        if (ok <= 5)
        {
            return var(1.5f);
        }
        if (ok > 5 && ok <= 10)
        {
            return var(MoveOnly{42});
        }
        return var(std::string("a very very long test test"));
    };
    auto v = dehe::make_variant(func(1));
    CHECK(std::is_same_v<decltype(v), std::variant<float, MoveOnly, std::string>>);
    CHECK_EQ(1.5f, std::get<0>(v));
    auto v2 = dehe::make_variant(func(6));
    CHECK_EQ(MoveOnly{42}, std::get<1>(v2));
    auto v3 = dehe::make_variant(func(11));
    CHECK_EQ(std::string_view("a very very long test test"), std::get<2>(v3));
}

inline void test_duplicate_type()
{
    auto func = [](bool ok)
    {
        static constexpr dehe::Variate var;
        if (ok)
        {
            return var("1");
        }
        return var("2");
    };
    auto v = dehe::make_variant(func(true));
    CHECK(std::is_same_v<decltype(v), std::variant<const char*, const char*>>);
    CHECK_EQ(std::string_view{"1"}, std::get<0>(v));
    auto v2 = dehe::make_variant(func(false));
    CHECK_EQ(std::string_view{"2"}, std::get<1>(v2));
}

inline void test_too_small_size()
{
    static constexpr dehe::Variate<sizeof(std::int32_t)> var;
    CHECK(std::is_invocable_v<decltype(var), std::int32_t>);
    static constexpr dehe::Variate<1> var2;
    CHECK_FALSE(std::is_invocable_v<decltype(var2), std::int32_t>);
}

inline void test_too_small_alignment()
{
    static constexpr dehe::Variate<1024, alignof(std::int32_t)> var;
    CHECK(std::is_invocable_v<decltype(var), std::int32_t>);
    static constexpr dehe::Variate<1024, 1> var2;
    CHECK_FALSE(std::is_invocable_v<decltype(var2), std::int32_t>);
}
}  // namespace test

#endif  // DEHE_TEST_TEST_HPP
