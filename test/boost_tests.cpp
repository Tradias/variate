// Copyright (c) 2022 Dennis Hezel
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <test/framework.hpp>
#include <test_boost.hpp>

namespace test
{
void run_boost_tests()
{
    using namespace test;

    run_test<&test_boost_variant>();
}
}  // namespace test