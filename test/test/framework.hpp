// Copyright (c) 2022 Dennis Hezel
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef DEHE_TEST_FRAMEWORK_HPP
#define DEHE_TEST_FRAMEWORK_HPP

#include <test/source_location.hpp>
#include <test/type_name.hpp>

namespace test
{
struct Expression
{
    const char* name;
    const char* fragment0;
    const char* fragment1{""};
    const char* fragment2{""};
};

struct FailedCheck
{
    Expression expression;
    SourceLocation location;
};

struct Context
{
    int tests_run;
    int failed_tests;
    int current_failed_checks;
    FailedCheck failed_checks[128];
};

extern Context context;

void run_test_impl(void (*test)(), StringView test_name);

template <auto Test>
void run_test()
{
    test::run_test_impl(Test, test::function_pointer_name_v<Test>);
}

void print_test_results();

void check(bool result, Expression expression, SourceLocation location = TEST_DEHE_CURRENT_LOCATION);
}  // namespace test

#define CHECK(...) ::test::check(static_cast<bool>(__VA_ARGS__), {"CHECK", #__VA_ARGS__})

#define CHECK_FALSE(...) ::test::check(!static_cast<bool>(__VA_ARGS__), {"CHECK_FALSE", #__VA_ARGS__})

#define CHECK_EQ(lhs, rhs) ::test::check(static_cast<bool>((lhs) == (rhs)), {"CHECK_EQ", #lhs, ", ", #rhs})

#define CHECK_NE(lhs, rhs) ::test::check(!static_cast<bool>((lhs) == (rhs)), {"CHECK_NE", #lhs, ", ", #rhs})

#endif  // DEHE_TEST_FRAMEWORK_HPP
