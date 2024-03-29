// Copyright (c) 2022 Dennis Hezel
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <test.hpp>
#include <test/framework.hpp>

int main()
{
    using namespace test;

    run_test<&test_make_single_element_variant>();
    run_test<&test_make_c_style_array_decay>();
    run_test<&test_make_move_only>();
    run_test<&test_duplicate_type>();
    run_test<&test_too_small_size>();
    run_test<&test_too_small_alignment>();
    run_test<&test_dependent_variate>();
    run_test<&test_dependent_variate_shorthand>();

    return finalize_test_results() ? 0 : 1;
}