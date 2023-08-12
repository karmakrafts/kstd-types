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
 * @since 04/07/2023
 */

#pragma once

#include "defaults.hpp"
#include "types.hpp"

namespace kstd {
    /**
     * A dummy type which can be used in unions or variants to
     * implement an is_empty behaviour. Can also be used as a passable
     * void type, if that is ever needed.
     * This type is exactly <b>alignof(u8)</b> bytes in size.
     */
    class Void final {
        [[maybe_unused]] u8 _value = 0;

        public:
        KSTD_DEFAULT_MOVE_COPY(Void, Void, constexpr)

        constexpr Void() noexcept = default;

        ~Void() noexcept = default;
    };
}// namespace kstd