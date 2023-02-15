# variate

[![Coverage](https://sonarcloud.io/api/project_badges/measure?project=Tradias_variate&metric=coverage)](https://sonarcloud.io/summary/new_code?id=Tradias_variate)

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
    static constexpr auto& var = dehe::variate<>;
    if (ok)
        return var(1.5f);
    return var("example");
}

static_assert(std::is_same_v<decltype(dehe::make_variant(func())), std::variant<float, const char*>>);
```

Play with it on [godbolt](https://godbolt.org/z/xWKeece14).

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

The type of `std::variant` can also be changed, for example to Boost.Variant2, see [boost_variant_adaptation.hpp](test/boost_variant_adaptation.hpp).

# Requirements

The only requirement is a small subset of C++20.

The following compilers are continuously tested by Github Actions:

* GCC 10
* Clang 12
* MSVC 19.34
* AppleClang 14
