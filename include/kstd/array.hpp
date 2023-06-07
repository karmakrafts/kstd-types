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
 * @since 07/06/2023
 */

#pragma once

#include "defaults.hpp"
#include "libc.hpp"
#include "types.hpp"

namespace kstd {
    template<typename T, usize SIZE>
    class Array final {
        T _data[SIZE];// NOLINT

        public:
        static constexpr usize size = SIZE;
        using ValueType = T;
        using Self = Array<T, SIZE>;

        KSTD_DEFAULT_MOVE_COPY(Array)

        constexpr Array() noexcept {                                       // NOLINT
            libc::memset(reinterpret_cast<T*>(_data), 0, sizeof(T) * SIZE);// NOLINT
        }

        ~Array() noexcept = default;

        [[nodiscard]] constexpr auto get_size() const noexcept -> usize {
            return size;
        }

        [[nodiscard]] constexpr auto get_data() noexcept -> T* {
            return reinterpret_cast<T*>(_data);// NOLINT
        }

        [[nodiscard]] constexpr auto get_data() const noexcept -> const T* {
            return reinterpret_cast<const T*>(_data);// NOLINT
        }
    };
}// namespace kstd