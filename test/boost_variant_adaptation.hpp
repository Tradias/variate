// Copyright (c) 2022 Dennis Hezel
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef DEHE_TEST_BOOST_VARIANT_ADAPTATION_HPP
#define DEHE_TEST_BOOST_VARIANT_ADAPTATION_HPP

#include <boost/variant2/variant.hpp>

namespace dehe::detail
{
using boost::variant2::in_place_index;
using boost::variant2::variant;
using std::size_t;
}  // namespace dehe::detail

#include <variate/variate_impl.hpp>

#endif  // DEHE_TEST_BOOST_VARIANT_ADAPTATION_HPP
