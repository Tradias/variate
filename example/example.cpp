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

// Code to bridge unifex to this library
static constexpr auto variant_sender_factory = []<std::size_t, class... T, class Arg>(Arg && arg)
{
    return unifex::variant_sender<T...>{std::forward<Arg>(arg)};
};
auto make_variant_sender(auto&& erased)
{
    return dehe::make(std::forward<decltype(erased)>(erased), variant_sender_factory);
}

// If this function wanted to return a variant_sender directly then we would have to move the lambda inside unifex::then
// into a separate function, otherwise we won't be able to decltype it. In any case, the return type would look like:
// `unifex::variant_sender<decltype(unifex::just(5)), decltype(unifex::just() | unifex::then(lambda))>`
// Notice how we have to duplicate most of the body of this function and this is just a small example. That's where this
// library comes into play.
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
    auto sender1 = unifex::let_value(unifex::just(true),
                                     [](bool ok)
                                     {
                                         return make_variant_sender(code(ok));
                                     });
    auto result = unifex::sync_wait(std::move(sender1));
    assert(result == 5);

    auto sender2 = make_variant_sender(code(false));
    auto result2 = unifex::sync_wait(std::move(sender2));
    assert(result2 == 42);
}