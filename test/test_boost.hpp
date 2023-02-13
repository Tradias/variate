// Copyright (c) 2022 Dennis Hezel
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef DEHE_TEST_TEST_BOOST_HPP
#define DEHE_TEST_TEST_BOOST_HPP

#include "boost_variant_adaptation.hpp"

#include <test/framework.hpp>
#include <test/utility.hpp>

#include <string>
#include <string_view>

namespace test
{
inline void test_boost_variant()
{
    auto func = [](int ok)
    {
        static constexpr auto& var = dehe::variate<>;
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
    CHECK(std::is_same_v<decltype(v), boost::variant2::variant<float, MoveOnly, std::string>>);
    CHECK_EQ(1.5f, boost::variant2::get<0>(v));
    auto v2 = dehe::make_variant(func(6));
    CHECK_EQ(MoveOnly{42}, boost::variant2::get<1>(v2));
    auto v3 = dehe::make_variant(func(11));
    CHECK_EQ(std::string_view("a very very long test test"), boost::variant2::get<2>(v3));
}
}  // namespace test

#endif  // DEHE_TEST_TEST_BOOST_HPP
