// Copyright (c) 2023 Dennis Hezel
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef DEHE_VARIATE_VARIATE_IMPL_HPP
#define DEHE_VARIATE_VARIATE_IMPL_HPP

/*
Usage:

``c++
auto func(bool ok)
{
    static constexpr auto& var = dehe::variate<>;
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
*/

#include <type_traits>

namespace dehe
{
namespace detail
{
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

// A global type map from Key to TypeList<Types...>
template <class Key, detail::size_t Index>
struct TypeListMap
{
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnon-template-friend"
#endif

    // Implementation is provided through friend injection performed by TypeListMapAppender.
    friend constexpr auto get_types_up_to_index(const TypeListMap&);

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
};

template <class Key, detail::size_t Index>
using TypesUpToIndex = decltype(get_types_up_to_index(TypeListMap<Key, Index>{}));

// Instantiating this type will append T to the TypeList<Types...> at Key in the global TypeListMap if Index is equal to
// sizeof...(Types), otherwise it will fail to compile.
template <class T, class Key, detail::size_t Index>
struct TypeListMapAppender
{
    static constexpr auto index = Index;

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

// Get TypeList<Types...> for a Key in the global TypeListMap.
template <class Key, detail::size_t I = 0, bool = ValidTypeListMapIndex<Key, I + 1>>
struct GetTypesFromMap : GetTypesFromMap<Key, I + 1>
{
};

template <class Key, detail::size_t I>
struct GetTypesFromMap<Key, I, false>
{
    using Type = TypesUpToIndex<Key, I>;
};

// Instantiate a TypeListMapAppender by first recursing to the current size of the TypeList at Key in the global
// TypeListMap using int-long overload resolution.
template <class T, class Key, detail::size_t Index = 0>
constexpr auto type_map_append(long)
{
    return TypeListMapAppender<T, Key, Index>{};
}

template <class T, class Key, detail::size_t Index = 0, auto = get_types_up_to_index(TypeListMap<Key, Index>{})>
constexpr auto type_map_append(int)
{
    return detail::type_map_append<T, Key, Index + 1>(int{});
}

// Type erased result of a variate function.
template <class Key, detail::size_t Size, detail::size_t Alignment>
struct Erased
{
    detail::size_t index;
    alignas(Alignment) char value[Size];
};

// Turn a list of types into `detail::variant` based on the runtime index stored in `erased`.
template <class, class...>
struct ToVariant;

template <template <class...> class List, class Current, class... Next, class... Previous>
struct ToVariant<List<Current, Next...>, Previous...>
{
    template <class Key, detail::size_t Size, detail::size_t Alignment>
    static auto apply(Erased<Key, Size, Alignment>& erased)
    {
        static constexpr detail::size_t index = sizeof...(Previous);
        if (index == erased.index)
        {
            return detail::variant<Previous..., Current, Next...>{
                detail::in_place_index<index>, static_cast<Current&&>(*reinterpret_cast<Current*>(erased.value))};
        }
        return ToVariant<List<Next...>, Previous..., Current>::apply(erased);
    }
};

template <template <class...> class List, class... T>
struct ToVariant<List<>, T...>
{
    template <class Key, detail::size_t Size, detail::size_t Alignment>
    [[noreturn]] static detail::variant<T...> apply(Erased<Key, Size, Alignment>&)
    {
// Possible implementation of C++23 std::unreachable
#ifdef __GNUC__
        __builtin_unreachable();
#elif defined(_MSC_VER)
        __assume(false);
#endif
    }
};

template <class Key, detail::size_t Size, detail::size_t Alignment>
struct VariateFn
{
    // Each invocation of this function will add VariantAlternative to the global TypeList at Key.
    template <class VariantAlternative,
              detail::size_t Index = detail::type_map_append<std::decay_t<VariantAlternative>, Key>(int{}).index>
    requires(sizeof(std::decay_t<VariantAlternative>) <= Size && alignof(std::decay_t<VariantAlternative>) <= Alignment)
    auto operator()(VariantAlternative&& alternative) const
    {
        Erased<Key, Size, Alignment> erased;
        erased.index = Index;
        using T = std::decay_t<VariantAlternative>;
        ::new (static_cast<void*>(erased.value)) T(static_cast<VariantAlternative&&>(alternative));
        return erased;
    }
};
}  // namespace detail

template <detail::size_t Size = 256, detail::size_t Alignment = alignof(double), auto Key = [] {}>
inline constexpr detail::VariateFn<decltype(Key), Size, Alignment> variate{};

template <class Key, detail::size_t Size, detail::size_t Alignment>
auto make_variant(detail::Erased<Key, Size, Alignment>&& erased)
{
    using Types = typename detail::GetTypesFromMap<Key>::Type;
    return detail::ToVariant<Types>::apply(erased);
}
}  // namespace dehe

#endif  // DEHE_VARIATE_VARIATE_IMPL_HPP