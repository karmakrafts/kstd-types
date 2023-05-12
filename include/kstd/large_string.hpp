// Copyright 2023 Karma Krafts & associates
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either expressor implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @author Alexander Hinze
 * @since 22/04/2023
 */

#pragma once

#include <type_traits>
#include <cstring>
#include <memory>
#include <array>
#include "string_slice.hpp"
#include "small_string.hpp"

namespace kstd {
    namespace {
        template<typename CHAR> //
        KSTD_REQUIRES(concepts::Char<CHAR>)
        struct StringInner final {
            CHAR* data;
            usize capacity;
            usize size : (sizeof(usize) << 3) - 1;
            bool is_large : 1;
        };
    }

    template<typename CHAR, typename ALLOCATOR> //
    KSTD_REQUIRES(concepts::Char<CHAR> && concepts::Allocator<ALLOCATOR>)
    union BasicString final {
        using ValueType = CHAR;
        using Self = BasicString<ValueType, ALLOCATOR>;
        using InnerType = StringInner<ValueType>;
        using SmallType = BasicSmallString<ValueType, sizeof(InnerType) / sizeof(ValueType)>;
        using SliceType = BasicStringSlice<CHAR>;
        using Pointer = ValueType*;
        using ConstPointer = const ValueType*;
        using SizeType = usize;
        using Iterator = Pointer;
        using ConstIterator = ConstPointer;

        static_assert(sizeof(SmallType) == sizeof(InnerType)); // Make sure size is calculated right

        private:

        InnerType _inner;
        SmallType _small;

        [[nodiscard]] constexpr auto is_small() const noexcept -> bool {
            return !_inner.is_large;
        }

        public:

        constexpr BasicString() noexcept :
                _small() {
        }

        // This allows for implicit assignment of string slices to owning strings
        constexpr BasicString(SliceType view) noexcept : // NOLINT
                _small() {
            *this = std::move(view.to_owning());
        }

        constexpr BasicString(const Self& other) noexcept :
                _small() {
        }

        constexpr BasicString(Self&& other) noexcept :
                _small() {
        }

        constexpr auto operator =(const Self& other) noexcept -> Self& {
            return *this;
        }

        constexpr auto operator =(Self&& other) noexcept -> Self& {
            return *this;
        }

        [[nodiscard]] constexpr auto get_data() noexcept -> Pointer {
            if (is_small()) {
                return _small.get_data();
            }

            return _inner.data;
        }

        [[nodiscard]] constexpr auto get_c_str() const noexcept -> ConstPointer {
            if (is_small()) {
                return _small.get_c_str();
            }

            return _inner.data;
        }

        [[nodiscard]] constexpr auto get_size() const noexcept -> SizeType {
            if (is_small()) {
                return _small.get_size();
            }

            return _inner.size;
        }

        [[nodiscard]] constexpr auto get_capacity() const noexcept -> SizeType {
            if (is_small()) {
                return SmallType::capacity;
            }

            return _inner.capacity;
        }

        [[nodiscard]] constexpr auto get_capacity_in_bytes() const noexcept -> SizeType {
            if (is_small()) {
                return SmallType::capacity * sizeof(CHAR);
            }

            return _inner.capacity * sizeof(CHAR);
        }

        [[nodiscard]] constexpr auto is_empty() const noexcept -> bool {
            return get_size() == 0;
        }

        [[nodiscard]] constexpr auto begin() noexcept -> Iterator {
            return get_data();
        }

        [[nodiscard]] constexpr auto end() noexcept -> Iterator {
            return get_data() + get_size(); // Pointer to last char
        }

        [[nodiscard]] constexpr auto cbegin() noexcept -> ConstIterator {
            return get_c_str();
        }

        [[nodiscard]] constexpr auto cend() noexcept -> ConstIterator {
            return get_c_str() + get_size(); // Pointer to last char
        }

        constexpr auto clear() noexcept -> void {
            if (is_small()) {
                _small.clear();
                return;
            }

            std::memset(_inner.data, 0, get_capacity_in_bytes());
        }

        [[nodiscard]] constexpr auto get_allocator() const noexcept -> ALLOCATOR {
            return {};
        }

        constexpr auto reserve(SizeType new_capacity, bool collapse = true) noexcept -> void {

        }

        [[nodiscard]] constexpr auto compact() noexcept -> Self& {
            reserve(get_capacity());
            return *this;
        }

        [[nodiscard]] constexpr auto operator [](SizeType index) noexcept -> ValueType& {
            #ifdef BUILD_DEBUG
            if (index > get_size()) {
                throw std::runtime_error("String index out of bounds");
            }
            #endif
            return *(get_data() + index);
        }
    };

    using String = BasicString<char>;
    using WString = BasicString<wchar_t>;

    #ifdef KSTD_SIZED_CHAR_TYPES

    using String8 = BasicString<char8_t>;
    using String16 = BasicString<char16_t>;
    using String32 = BasicString<char32_t>;

    #endif // KSTD_SIZED_CHAR_TYPES
}