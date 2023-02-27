// Copyright (c) 2022 Dennis Hezel
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <test/framework.hpp>
#include <test/type_name.hpp>

#include <cstdio>
#include <utility>

namespace test
{
static Context context{};

void run_test_impl(void (*test)(), StringView test_name)
{
    context.current_failed_checks = 0;
    test();
    if (context.current_failed_checks > 0)
    {
        ++context.failed_tests;
        for (int i = 0; i < context.current_failed_checks; ++i)
        {
            auto& [expression, location] = context.failed_checks[i];
            ::printf("Failure: %s:(%u) in \"%.*s\"\n  %s(%s%s%s)\n\n", location.file, location.line, test_name.size,
                     test_name.data, expression.name, expression.fragment0, expression.fragment1, expression.fragment2);
        }
    }
    ++context.tests_run;
}

bool finalize_test_results()
{
    if (context.failed_tests == 0)
    {
        ::printf("All tests succeeded: %i", context.tests_run);
    }
    else
    {
        ::printf("======================================\nFailed tests: %i/%i", context.failed_tests,
                 context.tests_run);
    }
    return context.failed_tests == 0;
}

void check(bool result, Expression expression, SourceLocation location)
{
    if (!result)
    {
        context.failed_checks[context.current_failed_checks] = {expression, location};
        ++context.current_failed_checks;
    }
}
}  // namespace test
