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
 * @since 12/05/2023
 */

#pragma once

#include "types.hpp"
#include "allocator.hpp"

namespace kstd {
    namespace {
        template<typename T, typename COUNTER_TYPE>
        struct RcInner final {
            T* value;
            COUNTER_TYPE count;

            constexpr RcInner() noexcept :
                    value(),
                    count() {
            }

            ~RcInner() noexcept = default;
        };
    }

    template<typename T, typename COUNTER_TYPE, template<typename> typename ALLOCATOR> //
    struct BasicRc final {
        using ElementType = T;
        using CounterType = COUNTER_TYPE;
        using Allocator = ALLOCATOR<ElementType>;
        using InnerType = RcInner<ElementType, CounterType>;
        using Self = BasicRc<ElementType, CounterType, ALLOCATOR>;

        private:

        InnerType* _inner;

        public:

        constexpr BasicRc() noexcept :
                _inner() {
        }

        explicit constexpr BasicRc(T* value) noexcept :
                _inner(ALLOCATOR<InnerType>().construct()) {
            _inner->value = value;
            _inner->count = 1;
        }

        constexpr BasicRc(const Self& other) noexcept :
                _inner(other._inner) {
            ++_inner->count;
        }

        constexpr BasicRc(Self&& other) noexcept :
                _inner(other._inner) {
            // Count stays the same
        }

        ~BasicRc() noexcept {
            drop();
        }

        constexpr auto drop() noexcept -> void {
            if (_inner != nullptr) {
                if (_inner->count > 0) {
                    --_inner->count;
                }
                else {
                    Allocator().destroy(_inner->value);
                    ALLOCATOR<InnerType>().destroy(_inner);
                }

                _inner = nullptr; // Avoid dangling pointer
            }
        }

        constexpr auto reset(T* pointer) noexcept -> void {
            drop();

            if (_inner == nullptr) {
                _inner = ALLOCATOR<InnerType>().construct();
            }

            _inner->value = pointer;
            _inner->count = 1;
        }

        constexpr auto operator =(const Self& other) noexcept -> Self& {
            if (this == &other) {
                return *this;
            }

            _inner = other._inner;

            if (_inner) {
                ++_inner->count;
            }

            return *this;
        }

        constexpr auto operator =(Self&& other) noexcept -> Self& {
            _inner = other._inner;
            return *this;
        }

        [[nodiscard]] constexpr auto has_value() const noexcept -> bool {
            return _inner != nullptr && _inner->value != nullptr;
        }

        [[nodiscard]] constexpr auto get_count() const noexcept -> usize {
            if (_inner == nullptr) {
                return 0;
            }

            return static_cast<usize>(_inner->count);
        }

        [[nodiscard]] constexpr operator bool() const noexcept { // NOLINT
            return has_value();
        }

        [[nodiscard]] constexpr operator T*() noexcept { // NOLINT
            return _inner->value;
        }

        [[nodiscard]] constexpr operator const T*() const noexcept { // NOLINT
            return _inner->value;
        }

        [[nodiscard]] constexpr auto operator *() noexcept -> T& {
            return *_inner->value;
        }

        [[nodiscard]] constexpr auto operator *() const noexcept -> const T& {
            return *_inner->value;
        }

        [[nodiscard]] constexpr auto operator ->() noexcept -> T* {
            return _inner->value;
        }

        [[nodiscard]] constexpr auto operator ->() const noexcept -> const T* {
            return _inner->value;
        }

        [[nodiscard]] constexpr auto operator ==(decltype(nullptr)) const noexcept -> bool {
            return !has_value();
        }

        [[nodiscard]] constexpr auto operator !=(decltype(nullptr)) const noexcept -> bool {
            return has_value();
        }
    };

    template<typename T, template<typename> typename ALLOCATOR = Allocator> //
    using Rc = BasicRc<T, usize, ALLOCATOR>;

    template<typename T, template<typename> typename ALLOCATOR = Allocator> //
    using Arc = BasicRc<T, Atomic<usize>, ALLOCATOR>;

    template<typename T, template<typename> typename ALLOCATOR = Allocator, typename... ARGS>
    [[nodiscard]] constexpr auto make_rc(ARGS&& ... args) noexcept -> Rc<T, ALLOCATOR> {
        ALLOCATOR<T> allocator;
        return Rc<T, ALLOCATOR>(allocator.construct(forward<ARGS>(args)...));
    }

    template<typename T, template<typename> typename ALLOCATOR = Allocator, typename... ARGS>
    [[nodiscard]] constexpr auto make_arc(ARGS&& ... args) noexcept -> Arc<T, ALLOCATOR> {
        ALLOCATOR<T> allocator;
        return Arc<T, ALLOCATOR>(allocator.construct(forward<ARGS>(args)...));
    }
}