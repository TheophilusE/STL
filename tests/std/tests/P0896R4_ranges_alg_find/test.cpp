// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <algorithm>
#include <cassert>
#include <concepts>
#include <ranges>
#include <utility>

#include <range_algorithm_support.hpp>
using namespace std;
using P = pair<int, int>;

// Validate dangling story
static_assert(same_as<decltype(ranges::find(borrowed<false>{}, 42)), ranges::dangling>);
static_assert(same_as<decltype(ranges::find(borrowed<true>{}, 42)), int*>);

struct instantiator {
    static constexpr P haystack[3] = {{0, 42}, {2, 42}, {4, 42}};

    template <ranges::input_range Read>
    static constexpr void call() {
        using ranges::find, ranges::iterator_t, ranges::begin, ranges::end;

        for (const auto& [value, _] : haystack) {
            { // Validate range overload [found case]
                Read wrapped_input{haystack};
                auto result = find(wrapped_input, value, get_first);
                static_assert(same_as<decltype(result), iterator_t<Read>>);
                assert(result.peek()->first == value);
            }
            { // Validate iterator + sentinel overload [found case]
                Read wrapped_input{haystack};
                auto result = find(wrapped_input.begin(), wrapped_input.end(), value, get_first);
                static_assert(same_as<decltype(result), iterator_t<Read>>);
                assert(result.peek()->first == value);
            }
        }
        { // Validate range overload [not found case]
            Read wrapped_input{haystack};
            auto result = find(wrapped_input, 42, get_first);
            static_assert(same_as<decltype(result), iterator_t<Read>>);
            assert(result == wrapped_input.end());
        }
        { // Validate iterator + sentinel overload [not found case]
            Read wrapped_input{haystack};
            auto result = find(wrapped_input.begin(), wrapped_input.end(), 42, get_first);
            static_assert(same_as<decltype(result), iterator_t<Read>>);
            assert(result == wrapped_input.end());
        }
        { // Validate memchr case
            char arr[5]{4, 8, 1, -15, 125};

            // found case
            auto result = find(arr, 1);
            assert(*result == 1);

            // not found case
            result = find(arr, 10);
            assert(result == end(arr));

            // unreachable_sentinel case
            result = find(begin(arr), unreachable_sentinel, 1);
            assert(*result == 1);
        }
    }
};

int main() {
    static_assert((test_in<instantiator, const P>(), true));
    test_in<instantiator, const P>();
}
