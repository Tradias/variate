// Copyright (c) 2023 Dennis Hezel
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

/*
Usage:

``c++
auto func(bool ok)
{
    static constexpr dehe::Variate var;
    if (ok)
    {
        return var(1.5f);
    }
    return var("example");
}

void use()
{
    auto variant = dehe::make_variant(func(true));
    static_assert(std::is_same_v<decltype(variant), std::variant<float, const char*>>);
    assert(1.5f == std::get<0>(variant));
}
```


If a get a contraint error due to sizeof or alignof in var() then specify a sufficiently large size during instantation
of variate:

```c++
    static constexpr dehe::Variate<512, 32> var;  // size: 512, alignment: 32
```


In template functions `Variate` must be made dependent on the template parameter:

```c++
template <typename T>
auto func(T param) {

    static constexpr dehe::DependentVariate<T> var;

    // or if sizeof/alignof customization is required:
    //static constexpr typename dehe::DependentVariate<T>::template Type<512> var;

    if constexpr (std::is_integral_v<T>) {
        return var(1);
    } else if (param > 0.0) {
        return var(1.0);
    } else {
        return var("Hello World");
    }
}
```


Implementation details:

* Create a unique type during instantiation of `constexpr dehe::Variate var` by using a lambda.
* For each call to `var(T v)`
    * add T to a global typelist map using the unique type as key.
    * instantiate a friend function that appends T to the typelist stored at key.
    * store v and its index in the type-erased wrapper returned from `var(T v)`.
* Move the type-erased wrapper returned from `var(T v)` into `make_variant()`.
* Iterate over the types in the global typelist map and compare their index to the runtime index stored in the
type-erased wrapper.
* Upon match, move the value stored in the type-erased wrapper to the final variant.
*/

#ifndef DEHE_VARIATE_VARIATE_HPP
#define DEHE_VARIATE_VARIATE_HPP

#include <cstddef>
#include <type_traits>
#include <variant>

namespace dehe
{
namespace detail
{
// Variant index type
using std::size_t;

template <class...>
struct TypeList
{
};

// Append T to a type list.
template <class List, class T>
struct AppendTypeToList;

template <template <class...> class List, class T, class... U>
struct AppendTypeToList<List<U...>, T>
{
    using Type = List<U..., T>;
};

template <class List, class T>
using AppendTypeToListT = typename AppendTypeToList<List, T>::Type;

// Conceptually this acts as a global type map from Key to TypeList<Types...>
template <class Key, detail::size_t Index>
struct TypeListMap
{
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-template-friend"
#endif

    // Implementation is provided through friend injection performed by TypeListMapAppender.
    friend constexpr auto get_types_up_to_index(const TypeListMap&);

#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
};

template <class Key, detail::size_t Index>
using TypesUpToIndex = decltype(get_types_up_to_index(TypeListMap<Key, Index>{}));

// Instantiating this type will append T to the TypeList<Types...> at Key in TypeListMap if Index is equal to
// sizeof...(Types), otherwise it will fail to compile.
template <class T, class Key, detail::size_t Index>
struct TypeListMapAppender
{
    static constexpr detail::size_t index = Index;

    // Returns TypeList<Types[0], ..., Types[Index]>.
    friend constexpr auto get_types_up_to_index(const TypeListMap<Key, Index>&)
    {
        if constexpr (Index > 0)
        {
            using Previous = TypesUpToIndex<Key, Index - 1>;
            using NextList = AppendTypeToListT<Previous, T>;
            return NextList{};
        }
        else
        {
            return detail::TypeList<T>{};
        }
    }
};

template <class Key, detail::size_t Index>
concept ValidTypeListMapIndex = requires(const TypeListMap<Key, Index>& v) { get_types_up_to_index(v); };

// Get TypeList<Types...> for a Key in TypeListMap.
template <class Key, detail::size_t I = 0, bool = ValidTypeListMapIndex<Key, I + 1>>
struct GetTypesFromMap : GetTypesFromMap<Key, I + 1>
{
};

template <class Key, detail::size_t I>
struct GetTypesFromMap<Key, I, false>
{
    using Type = TypesUpToIndex<Key, I>;
};

// Instantiate a TypeListMapAppender by first recursing to the current size of the TypeList at Key in TypeListMap using
// int-long overload resolution and sfinae on TypesUpToIndex.
template <class T, class Key, detail::size_t Index = 0>
constexpr detail::size_t type_map_append(long)
{
    return TypeListMapAppender<T, Key, Index>{}.index;
}

template <class T, class Key, detail::size_t Index = 0, class = TypesUpToIndex<Key, Index>>
constexpr detail::size_t type_map_append(int)
{
    return detail::type_map_append<T, Key, Index + 1>(int{});
}

// Type erased return type of a variate function.
template <class Key, std::size_t Size, std::size_t Alignment>
struct Erased
{
    detail::size_t index;
    alignas(Alignment) unsigned char value[Size];
};

// Turn a list of types into `std::variant` (or any other type produced by `factory`) based on the runtime index and
// value stored in `erased`.
template <class, class...>
struct ToVariant;

template <template <class...> class List, class Current, class... Next, class... Previous>
struct ToVariant<List<Current, Next...>, Previous...>
{
    template <class Key, std::size_t Size, std::size_t Alignment, class Factory>
    static auto apply(Erased<Key, Size, Alignment>& erased, Factory&& factory)
    {
        static constexpr detail::size_t index = sizeof...(Previous);
        if (index == erased.index)
        {
            return static_cast<Factory&&>(factory).template operator()<index, Previous..., Current, Next...>(
                static_cast<Current&&>(*reinterpret_cast<Current*>(erased.value)));
        }
        return ToVariant<List<Next...>, Previous..., Current>::apply(erased, static_cast<Factory&&>(factory));
    }
};

template <template <class...> class List, class First, class... Rest>
struct ToVariant<List<>, First, Rest...>
{
    template <class Key, std::size_t Size, std::size_t Alignment, class Factory>
    [[noreturn]] static auto apply(Erased<Key, Size, Alignment>&, Factory&& factory)
        -> decltype(static_cast<Factory&&>(factory).template operator()<0, First, Rest...>(std::declval<First>()))
    {
// Possible implementation of C++23 std::unreachable
#ifdef __GNUC__
        __builtin_unreachable();
#elif defined(_MSC_VER)
        __assume(false);
#endif
    }
};

template <class...>
struct DependentUniqueType
{
    template <auto L = [] {}>
    static constexpr auto value = L;
};

struct StdVariantFactory
{
    template <detail::size_t Index, class... T, class Arg>
    auto operator()(Arg&& arg)
    {
        return std::variant<T...>{std::in_place_index<Index>, static_cast<Arg&&>(arg)};
    }
};
}  // namespace detail

template <std::size_t Size = 256, std::size_t Alignment = alignof(double), auto Key = [] {}>
class Variate
{
  private:
    using KeyT = decltype(Key);

  public:
    template <class VariantAlternative,
              detail::size_t Index = detail::type_map_append<std::decay_t<VariantAlternative>, KeyT>(int{})>
    requires(sizeof(std::decay_t<VariantAlternative>) <= Size && alignof(std::decay_t<VariantAlternative>) <= Alignment)
    [[nodiscard]] auto operator()(VariantAlternative&& alternative) const
    {
        detail::Erased<KeyT, Size, Alignment> erased;
        erased.index = Index;
        using T = std::decay_t<VariantAlternative>;
        ::new (static_cast<void*>(erased.value)) T(static_cast<VariantAlternative&&>(alternative));
        return erased;
    }
};

template <class... T>
struct DependentVariate : Variate<256, alignof(double), detail::DependentUniqueType<T...>::template value<>>
{
    template <std::size_t Size = 256, std::size_t Alignment = alignof(double)>
    using Type = Variate<Size, alignof(double), detail::DependentUniqueType<T...>::template value<>>;
};

// Factory must be a callable type with signature:
//
// template <detail::size_t Index, class... T, class Arg>
// auto operator()(Arg&& arg);
//
// Where Index is the index of the runtime value Arg in the types of the resulting variant<T...>.
template <class Key, std::size_t Size, std::size_t Alignment, class Factory>
[[nodiscard]] auto make(detail::Erased<Key, Size, Alignment>&& erased, Factory&& factory)
{
    using Types = typename detail::GetTypesFromMap<Key>::Type;
    return detail::ToVariant<Types>::apply(erased, static_cast<Factory&&>(factory));
}

template <class Key, std::size_t Size, std::size_t Alignment>
[[nodiscard]] auto make_variant(detail::Erased<Key, Size, Alignment>&& erased)
{
    return dehe::make(static_cast<detail::Erased<Key, Size, Alignment>&&>(erased), detail::StdVariantFactory{});
}
}  // namespace dehe

#endif  // DEHE_VARIATE_VARIATE_HPP
