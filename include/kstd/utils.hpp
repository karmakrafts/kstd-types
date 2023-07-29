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
 * @since 11/05/2023
 */

#pragma once

#include <cstring>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace kstd::utils {
    /**
     * Transmutes the type of the given value while retaining the exact memory layout.
     * This is equal to performing an std::bitcast, but since that function is not portable
     * on C++17, we need to implement it ourselves.
     *
     * @tparam R The type to transmute the given reference into.
     * @tparam T The type of the reference to transmute. Usually deduced automatically.
     * @param value The reference to transmute into a different type.
     * @return A reference to the given object of the specified return type.
     */
    template<typename R, typename T>
    [[nodiscard]] constexpr auto transmute(T& value) noexcept -> R& {
        return *reinterpret_cast<R*>(&value);// NOLINT
    }

    /**
     * Transmutes the type of the given value while retaining the exact memory layout.
     * This is equal to performing an std::bitcast, but since that function is not portable
     * on C++17, we need to implement it ourselves.
     *
     * @tparam R The type to transmute the given reference into.
     * @tparam T The type of the reference to transmute. Usually deduced automatically.
     * @param value The reference to transmute into a different type.
     * @return A reference to the given object of the specified return type.
     */
    template<typename R, typename T>
    [[nodiscard]] constexpr auto transmute(const T& value) noexcept -> const R& {
        return *reinterpret_cast<const R*>(&value);// NOLINT
    }

    /**
     * Creates a new multibyte UTF-8 string from the contents of the given wide UTF-8 string.
     *
     * @param value The wide UTF-8 string to copy data from into the newly created string.
     * @return A new multibyte UTF-8 string with the contents of the given wide UTF-8 string.
     */
    [[nodiscard]] inline auto to_mbs(const std::wstring& value) noexcept -> std::string {
        const auto length = value.size();
        const auto terminated_length = length + 1;
        const auto size = terminated_length * sizeof(wchar_t);

        const std::string locale = std::setlocale(LC_ALL, nullptr);
        std::setlocale(LC_ALL, "en_US.UTF-8");

        std::string result(size, ' ');
        result.resize(std::wcstombs(result.data(), value.c_str(), size));

        std::setlocale(LC_ALL, locale.c_str());
        return result;
    }

    /**
     * Creates a new multibyte UTF-8 string from the contents of the given wide UTF-8 C-string.
     *
     * @param value The wide UTF-8 C-string to copy data from into the newly created string.
     * @return A new multibyte UTF-8 string with the contents of the given wide UTF-8 C-string.
     */
    [[nodiscard]] inline auto to_mbs(const wchar_t* value) noexcept -> std::string {
        const auto length = std::wcslen(value);
        const auto terminated_length = length + 1;
        const auto size = terminated_length * sizeof(wchar_t);

        const std::string locale = std::setlocale(LC_ALL, nullptr);
        std::setlocale(LC_ALL, "en_US.UTF-8");

        std::string result(size, ' ');
        result.resize(std::wcstombs(result.data(), value, size));

        std::setlocale(LC_ALL, locale.c_str());
        return result;
    }

    /**
     * Creates a new wide UTF-8 string from the contents of the given UTF-8 string.
     *
     * @param value The multibyte UTF-8 string to copy data from into the newly created string.
     * @return A new wide UTF-8 string with the contents of the given multibyte UTF-8 string.
     */
    [[nodiscard]] inline auto to_wcs(const std::string& value) noexcept -> std::wstring {
        const auto length = value.size();
        const auto size = length + 1;

        const std::string locale = std::setlocale(LC_ALL, nullptr);
        std::setlocale(LC_ALL, "en_US.UTF-8");

        std::wstring result(size, ' ');
        result.resize(std::mbstowcs(result.data(), value.c_str(), size));

        std::setlocale(LC_ALL, locale.c_str());
        return result;
    }

    /**
     * Creates a new wide UTF-8 string from the contents of the given UTF-8 C-string.
     *
     * @param value The multibyte UTF-8 C-string to copy data from into the newly created string.
     * @return A new wide UTF-8 string with the contents of the given multibyte UTF-8 C-string.
     */
    [[nodiscard]] inline auto to_wcs(const char* value) noexcept -> std::wstring {
        const auto length = std::strlen(value);
        const auto size = length + 1;

        const std::string locale = std::setlocale(LC_ALL, nullptr);
        std::setlocale(LC_ALL, "en_US.UTF-8");

        std::wstring result(size, ' ');
        result.resize(std::mbstowcs(result.data(), value, size));

        std::setlocale(LC_ALL, locale.c_str());
        return result;
    }
}// namespace kstd::utils