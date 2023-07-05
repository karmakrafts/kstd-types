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
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @author Alexander Hinze
 * @since 09/05/2023
 */

#pragma once

#include <algorithm>

#include "assert.hpp"
#include "defaults.hpp"
#include "libc.hpp"
#include "meta.hpp"
#include "utils.hpp"
#include "void.hpp"

namespace kstd {
    /*
     * A Box is a union-friendly way to store a pointer, reference or owned value.
     * Primarily used for things like kstd::Option and kstd::Result to make the code
     * a little easier on the eyes and more digestible.
     */
    template<typename T>
    struct Box {
        using ValueType = T;
        using Self = Box<ValueType>;
        using BorrowedValueType = ValueType&;
        using ConstBorrowedValueType = const ValueType&;
        using Pointer = ValueType*;
        using ConstPointer = const ValueType*;

        private:
        std::variant<ValueType, Void> _value;

        public:
        KSTD_DEFAULT_MOVE_COPY(Box)

        constexpr Box() noexcept :
                _value(Void()) {
        }

        constexpr Box(ValueType value) noexcept :// NOLINT
                _value(utils::move_or_copy(value)) {
        }

        ~Box() noexcept = default;

        [[nodiscard]] constexpr auto is_empty() const noexcept -> bool {
            return std::holds_alternative<Void>(_value);
        }

        [[nodiscard]] constexpr auto get() noexcept -> BorrowedValueType {
            assert_false(is_empty());
            return std::get<ValueType>(_value);
        }

        [[nodiscard]] constexpr auto get() const noexcept -> ConstBorrowedValueType {
            assert_false(is_empty());
            return std::get<ValueType>(_value);
        }

        [[nodiscard]] constexpr auto operator*() noexcept -> BorrowedValueType {
            return get();
        }

        [[nodiscard]] constexpr auto operator*() const noexcept -> ConstBorrowedValueType {
            return get();
        }

        [[nodiscard]] constexpr auto operator->() noexcept -> Pointer {
            return &get();
        }

        [[nodiscard]] constexpr auto operator->() const noexcept -> ConstPointer {
            return &get();
        }

        [[nodiscard]] constexpr auto operator==(const Self& other) const noexcept -> bool {
            return is_empty() == other.is_empty() && std::get<ValueType>(_value) == std::get<ValueType>(other._value);
        }

        [[nodiscard]] constexpr auto operator!=(const Self& other) const noexcept -> bool {
            return is_empty() != other.is_empty() || std::get<ValueType>(_value) != std::get<ValueType>(other._value);
        }

        [[nodiscard]] constexpr auto operator==(const ValueType& value) const noexcept -> bool {
            return !is_empty() && std::get<ValueType>(_value) == value;
        }

        [[nodiscard]] constexpr auto operator!=(const ValueType& value) const noexcept -> bool {
            return is_empty() || std::get<ValueType>(_value) != value;
        }
    };

    /*
     * Specialization for pointers, always pass-by-value
     */
    template<typename T>
    struct Box<T*> final {
        using ValueType = T*;
        using Self = Box<ValueType>;
        using BorrowedValueType = ValueType&;
        using ConstBorrowedValueType = ValueType const&;
        using Pointer = ValueType;
        using ConstPointer = const ValueType;

        private:
        ValueType _value;

        public:
        KSTD_DEFAULT_MOVE_COPY(Box)

        constexpr Box() noexcept :
                _value(nullptr) {
        }

        constexpr Box(ValueType value) noexcept :// NOLINT
                _value(value) {
        }

        ~Box() noexcept = default;

        [[nodiscard]] constexpr auto is_empty() const noexcept -> bool {
            return _value == nullptr;
        }

        [[nodiscard]] constexpr auto get() noexcept -> BorrowedValueType {
            assert_false(is_empty());
            return _value;
        }

        [[nodiscard]] constexpr auto get() const noexcept -> ConstBorrowedValueType {
            assert_false(is_empty());
            return _value;
        }

        [[nodiscard]] constexpr auto operator*() noexcept -> BorrowedValueType {
            return get();
        }

        [[nodiscard]] constexpr auto operator*() const noexcept -> ConstBorrowedValueType {
            return get();
        }

        [[nodiscard]] constexpr auto operator->() noexcept -> Pointer {
            return get();
        }

        [[nodiscard]] constexpr auto operator->() const noexcept -> ConstPointer {// NOLINT
            return get();
        }

        [[nodiscard]] constexpr auto operator==(const Self& other) const noexcept -> bool {
            return is_empty() == other.is_empty() && _value == other._value;
        }

        [[nodiscard]] constexpr auto operator!=(const Self& other) const noexcept -> bool {
            return is_empty() != other.is_empty() || _value != other._value;
        }

        [[nodiscard]] constexpr auto operator==(ConstPointer pointer) const noexcept -> bool {
            return !is_empty() && _value == pointer;
        }

        [[nodiscard]] constexpr auto operator!=(ConstPointer pointer) const noexcept -> bool {
            return is_empty() || _value != pointer;
        }
    };

    /*
     * Specialization for references, stores a pointer
     */
    template<typename T>
    struct Box<T&> final {
        using ValueType = T&;
        using BorrowedValueType = T&;
        using ConstBorrowedValueType = const T&;
        using NakedValueType = meta::Naked<ValueType>;
        using Pointer = NakedValueType*;
        using ConstPointer = const NakedValueType*;
        using Self = Box<ValueType>;
        using StoredValueType = meta::If<meta::is_const<ValueType>, ConstPointer, Pointer>;

        private:
        StoredValueType _value;

        public:
        KSTD_DEFAULT_MOVE_COPY(Box)

        constexpr Box() noexcept :
                _value(nullptr) {
        }

        constexpr Box(ValueType value) noexcept :// NOLINT
                _value(&value) {
        }

        ~Box() noexcept = default;

        [[nodiscard]] constexpr auto is_empty() const noexcept -> bool {
            return _value == nullptr;
        }

        [[nodiscard]] constexpr auto get() noexcept -> BorrowedValueType {
            assert_false(is_empty());
            return *_value;
        }

        [[nodiscard]] constexpr auto get() const noexcept -> ConstBorrowedValueType {// NOLINT
            assert_false(is_empty());
            return *_value;
        }

        [[nodiscard]] constexpr auto operator*() noexcept -> BorrowedValueType {
            return get();
        }

        [[nodiscard]] constexpr auto operator*() const noexcept -> ConstBorrowedValueType {// NOLINT
            return get();
        }

        [[nodiscard]] constexpr auto operator->() noexcept -> Pointer {
            return &get();
        }

        [[nodiscard]] constexpr auto operator->() const noexcept -> ConstPointer {
            return &get();
        }

        [[nodiscard]] constexpr auto operator==(const Self& other) const noexcept -> bool {
            return is_empty() == other.is_empty() && _value == other._value;
        }

        [[nodiscard]] constexpr auto operator!=(const Self& other) const noexcept -> bool {
            return is_empty() != other.is_empty() || _value != other._value;
        }

        [[nodiscard]] constexpr auto operator==(ValueType ref) const noexcept -> bool {
            return !is_empty() && _value == &ref;
        }

        [[nodiscard]] constexpr auto operator!=(ValueType ref) const noexcept -> bool {
            return is_empty() || _value != &ref;
        }
    };

    template<typename T>
    [[nodiscard]] constexpr auto make_box(T value) noexcept -> Box<T> {
        return Box<T>(utils::move_or_copy(value));
    }
#ifdef BUILD_DEBUG
#endif
}// namespace kstd