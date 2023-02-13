// Copyright (c) 2022 Dennis Hezel
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef DEHE_TEST_UTILITY_HPP
#define DEHE_TEST_UTILITY_HPP

namespace test
{
struct MoveOnly
{
    int v{};

    MoveOnly() = default;

    constexpr explicit MoveOnly(int v) noexcept : v(v) {}

    MoveOnly(const MoveOnly&) = delete;

    MoveOnly(MoveOnly&& other) = default;

    MoveOnly& operator=(const MoveOnly&) = delete;

    MoveOnly& operator=(MoveOnly&& other) = default;

    friend bool operator==(const MoveOnly&, const MoveOnly&) = default;
};
}  // namespace test

#endif  // DEHE_TEST_UTILITY_HPP
