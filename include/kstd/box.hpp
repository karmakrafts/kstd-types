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

#include "meta.hpp"
#include "utils.hpp"
#include "kstd/concepts.hpp"

namespace kstd {
    namespace {
        template<typename T> //
        using def_if_ptr = meta::def_if<meta::is_ptr<T>>;

        template<typename T> //
        using def_if_ref = meta::def_if<meta::is_ref<T>>;

        template<typename T> //
        using def_if_val = meta::def_if<!meta::is_ref<T> && !meta::is_ptr<T>>;
    }

    /*
     * A Box is a union-friendly way to store a pointer, reference or owned value.
     * Primarily used for things like kstd::Option and kstd::Result to make the code
     * a little easier on the eyes and more digestible.
     */
    template<typename T, typename = void>
    struct Box;

    /*
     * Specialization for pointers, always pass-by-value
     */
    template<typename T> //
    KSTD_REQUIRES(!meta::is_void<T>)
    struct Box<T, def_if_ptr<T>> final {
        [[maybe_unused]] static constexpr bool is_pointer = true;
        [[maybe_unused]] static constexpr bool is_reference = false;
        [[maybe_unused]] static constexpr bool is_value = false;

        using Self = Box<T, def_if_ptr<T>>;
        using ValueType = T;
        using StoredValueType = T;
        using BorrowedValueType = T;
        using ConstBorrowedValueType = const T;
        using PointerType = T;
        using ConstPointerType = const T;

        private:

        StoredValueType _value;

        public:

        constexpr Box() noexcept :
                _value(nullptr) {
        }

        constexpr Box(ValueType value) noexcept : // NOLINT
                _value(value) {
        }

        constexpr Box(const Self& other) noexcept = default;

        constexpr Box(Self&& other) noexcept = default;

        ~Box() noexcept = default;

        constexpr auto drop() noexcept -> void {
        }

        [[nodiscard]] constexpr auto borrow() noexcept -> BorrowedValueType {
            return _value;
        }

        [[nodiscard]] constexpr auto borrow() const noexcept -> ConstBorrowedValueType { // NOLINT
            return _value;
        }

        [[nodiscard]] constexpr auto get() noexcept -> ValueType {
            return _value;
        }

        constexpr auto operator =(const Self& other) noexcept -> Self& = default;

        constexpr auto operator =(Self&&) noexcept -> Self& = default;

        constexpr auto operator =(ValueType value) noexcept -> Self& {
            _value = value;
            return *this;
        }

        [[nodiscard]] constexpr auto operator *() noexcept -> BorrowedValueType {
            return borrow();
        }

        [[nodiscard]] constexpr auto operator *() const noexcept -> ConstBorrowedValueType { // NOLINT
            return borrow();
        }

        [[nodiscard]] constexpr auto operator ->() noexcept -> PointerType {
            return borrow();
        }

        [[nodiscard]] constexpr auto operator ->() const noexcept -> ConstPointerType { // NOLINT
            return borrow();
        }
    };

    /*
     * Specialization for references, stores a pointer
     */
    template<typename T> //
    KSTD_REQUIRES(!meta::is_void<T>)
    struct Box<T, def_if_ref<T>> final {
        [[maybe_unused]] static constexpr bool is_pointer = false;
        [[maybe_unused]] static constexpr bool is_reference = true;
        [[maybe_unused]] static constexpr bool is_value = false;

        using Self = Box<T, def_if_ref<T>>;
        using ValueType = T;
        using naked_type = meta::remove_ref<typename std::remove_const<T>::type>;
        using StoredValueType = naked_type*;
        using BorrowedValueType = naked_type&;
        using ConstBorrowedValueType = const naked_type&;
        using PointerType = naked_type*;
        using ConstPointerType = const naked_type*;

        private:

        StoredValueType _value;

        public:

        constexpr Box() noexcept :
                _value(nullptr) {
        }

        constexpr Box(ValueType value) noexcept : // NOLINT
                _value(&value) {
        }

        constexpr Box(const Self& other) noexcept = default;

        constexpr Box(Self&& other) noexcept = default;

        ~Box() noexcept = default;

        constexpr auto drop() noexcept -> void {
        }

        [[nodiscard]] constexpr auto borrow() noexcept -> BorrowedValueType {
            return *_value;
        }

        [[nodiscard]] constexpr auto borrow() const noexcept -> ConstBorrowedValueType {
            return *_value;
        }

        [[nodiscard]] constexpr auto get() noexcept -> ValueType {
            return *_value;
        }

        constexpr auto operator =(const Self& other) noexcept -> Self& = default;

        constexpr auto operator =(Self&&) noexcept -> Self& = default;

        constexpr auto operator =(ValueType value) noexcept -> Self& {
            _value = &value;
            return *this;
        }

        [[nodiscard]] constexpr auto operator *() noexcept -> BorrowedValueType {
            return *_value;
        }

        [[nodiscard]] constexpr auto operator *() const noexcept -> ConstBorrowedValueType {
            return *_value;
        }

        [[nodiscard]] constexpr auto operator ->() noexcept -> PointerType {
            return _value;
        }

        [[nodiscard]] constexpr auto operator ->() const noexcept -> ConstPointerType {
            return _value;
        }
    };

    /*
     * Specialization for owned values, uses move semantics
     */
    template<typename T>  //
    KSTD_REQUIRES(!meta::is_void<T>)
    struct Box<T, def_if_val<T>> final {
        static_assert(meta::is_move_assignable<T>, "Box type is not move assignable");
        static_assert(meta::is_move_constructible<T>, "Box type is not move constructible");
        static_assert(meta::is_default_constructible<T>, "Box type is not default constructible");

        [[maybe_unused]] static constexpr bool is_pointer = false;
        [[maybe_unused]] static constexpr bool is_reference = false;
        [[maybe_unused]] static constexpr bool is_value = true;

        using Self = Box<T, def_if_val<T>>;
        using ValueType = T;
        using BorrowedValueType = ValueType&;
        using ConstBorrowedValueType = const ValueType&;
        using PointerType = ValueType*;
        using ConstPointerType = const ValueType*;

        private:

        ValueType _value;

        public:

        constexpr Box() noexcept :
                _value(ValueType()) {
        }

        constexpr Box(const ValueType& value) noexcept : // NOLINT
                _value(value) {
        }

        constexpr Box(ValueType&& value) noexcept : // NOLINT
                _value(move(value)) {
        }

        constexpr Box(const Self& other) noexcept :
                _value(other._value) {
        }

        constexpr Box(Self&& other) noexcept :
                _value(move(other._value)) {
        }

        ~Box() noexcept {
            drop();
        }

        constexpr auto drop() noexcept -> void {
            if constexpr (meta::is_destructible<T>) {
                _value.~ValueType();
            }
        }

        [[nodiscard]] constexpr auto borrow() noexcept -> BorrowedValueType {
            return _value;
        }

        [[nodiscard]] constexpr auto borrow() const noexcept -> ConstBorrowedValueType {
            return _value;
        }

        [[nodiscard]] constexpr auto get() noexcept -> ValueType&& {
            return move(_value);
        }

        constexpr auto operator =(const Self& other) noexcept -> Self& {
            drop();
            _value = other._value;
            return *this;
        }

        constexpr auto operator =(Self&& other) noexcept -> Self& {
            drop();
            _value = move(other._value);
            return *this;
        }

        constexpr auto operator =(ValueType value) noexcept -> Self& {
            drop();
            _value = move(value);
            return *this;
        }

        [[nodiscard]] constexpr auto operator *() noexcept -> BorrowedValueType {
            return _value;
        }

        [[nodiscard]] constexpr auto operator *() const noexcept -> ConstBorrowedValueType {
            return _value;
        }

        [[nodiscard]] constexpr auto operator ->() noexcept -> PointerType {
            return _value;
        }

        [[nodiscard]] constexpr auto operator ->() const noexcept -> ConstPointerType {
            return _value;
        }
    };

    template<typename T>
    //
    KSTD_REQUIRES(!meta::is_void<T>)
    [[nodiscard]] constexpr auto make_box(T value) noexcept -> decltype(auto) {
        return Box<T>(move_or_copy(value));
    }
}