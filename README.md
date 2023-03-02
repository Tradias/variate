# variate

[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=Tradias_variate&metric=alert_status)](https://sonarcloud.io/summary/new_code?id=Tradias_variate)

Deduce the template parameter of a variant based on every return path. Ideally, the C++ language would provide something like:

```c++
std::variant auto func(bool ok)
{
    if (ok)
        return 1.5f;
    return "example";
}

static_assert(std::is_same_v<decltype(func()), std::variant<float, const char*>>);
```

This library attempts to mimic such functionality with minimal boilerplate and (zero) overhead:

```c++
auto func(bool ok)
{
    static constexpr dehe::Variate var;
    if (ok)
        return var(1.5f);
    return var("example");
}

static_assert(std::is_same_v<decltype(dehe::make_variant(func())), std::variant<float, const char*>>);
```

Play with it on [godbolt](https://godbolt.org/z/hdadaqjdT).

# Motivation

With the advent of [std::execution](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2300r6.html), C++ developers are faced with more and more unnamable types. All sender types are considered implementation details and can only be identified using something like `decltype(std::execution::just(42))`. Additionally, we often must write functional-style code when dealing with `std::execution`, meaning we must return the same sender type from all return paths of our functions. This makes it difficult to express conditional branches. [Libunifex](https://github.com/facebookexperimental/libunifex) provides a `variant_sender` class template that we can hook into this library to ease the process:

```c++
// Code to bridge unifex to this library
static constexpr auto variant_sender_factory = []<std::size_t, class... T, class Arg>(Arg && arg)
{
    return unifex::variant_sender<T...>{std::forward<Arg>(arg)};
};
auto make_variant_sender(auto&& erased)
{
    return dehe::make(std::forward<decltype(erased)>(erased), variant_sender_factory);
}

// Our actual business logic containing branches. Without this library we would need to spell out the return type:
// `unifex::variant_sender<decltype(unifex::just(5)), decltype(unifex::just() | unifex::then(lambda))>`
// and move the lambda into a separate place.
auto business_logic(bool okbusiness_logic)
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

// Imagine some chain of senders:
    | unifex::let_value([](bool ok)
                        {
                            return make_variant_sender(business_logic(ok));
                        });
```

# Installation

Copy the headers from `src/variate` into your project.

Alternatively, use CMake to install the project. From the root of the repository:

```cmake
cmake -B build -S .
cmake --install build --prefix build/out
```

And in your CMake project's CMakeLists.txt:

```cmake
# Add build/out to the CMAKE_PREFIX_PATH
find_package(variate)
target_link_libraries(my_app PRIVATE variate::variate)
```

# Usage

Include the single header:

```cpp
#include <variate/variate.hpp>
```

# Requirements

The only requirement is a small subset of C++20.

The following compilers are continuously tested by Github Actions:

* GCC 10
* Clang 12
* MSVC 19.34
* AppleClang 14
