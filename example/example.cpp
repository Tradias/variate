// Copyright (c) 2023 Dennis Hezel
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <unifex/just.hpp>
#include <unifex/let_value.hpp>
#include <unifex/sync_wait.hpp>
#include <unifex/then.hpp>
#include <unifex/variant_sender.hpp>
#include <variate/variate.hpp>

#include <cassert>

static constexpr auto variant_sender_factory = []<std::size_t, class... T, class Arg>(Arg && arg)
{
    return unifex::variant_sender<T...>{std::forward<Arg>(arg)};
};

auto make_variant_sender(auto&& erased)
{
    return dehe::make(std::forward<decltype(erased)>(erased), variant_sender_factory);
}

auto code(bool ok)
{
    static constexpr dehe::Variate var;
    if (ok)
    {
        return var(unifex::just(5));
    }
    return var(unifex::just() | unifex::then(
                                    []
                                    {
                                        return 42;
                                    }));
}

int main()
{
    auto f1_sender = unifex::let_value(unifex::just(true),
                                       [](bool ok)
                                       {
                                           return make_variant_sender(code(ok));
                                       });
    auto result = unifex::sync_wait(std::move(f1_sender));
    assert(result == 5);
    auto f2_sender = make_variant_sender(code(false));
    auto result2 = unifex::sync_wait(std::move(f2_sender));
    assert(result2 == 42);
}