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
 * @since 16/05/2023
 */

#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>

#include "math.hpp"
#include "meta.hpp"
#include "types.hpp"
#include "utils.hpp"

namespace kstd::libc {
    namespace iob {
#if defined(PLATFORM_LINUX)
        inline auto out = stdout;// NOLINT
        inline auto err = stderr;// NOLINT
        inline auto in = stdin;  // NOLINT
#elif defined(PLATFORM_WINDOWS)
        inline auto out = ::__acrt_iob_func(1);// NOLINT
        inline auto err = ::__acrt_iob_func(2);// NOLINT
        inline auto in = ::__acrt_iob_func(0); // NOLINT
#else
#ifdef __DARWIN_UNIX03
        inline auto out = ::__stdoutp;// NOLINT
        inline auto err = ::__stderrp;// NOLINT
        inline auto in = ::__stdinp;  // NOLINT
#else
        inline auto out = &::__sF[1];// NOLINT
        inline auto err = &::__sF[2];// NOLINT
        inline auto in = &::__sF[0]; // NOLINT
#endif
#endif
    }// namespace iob

    using std::exit;

    using std::calloc;
    using std::free;
    using std::malloc;
    using std::realloc;

    using std::memchr;
    using std::memcmp;
    using std::memcpy;
    using std::memmove;
    using std::memset;

    using std::strcat;
    using std::strchr;
    using std::strcmp;
    using std::strcpy;
    using std::strlen;
    using std::strncmp;
    using std::strncpy;
    using std::strrchr;
    using std::strstr;
    using std::strtok;
    using std::strxfrm;

    using std::wcscat;
    using std::wcschr;
    using std::wcscmp;
    using std::wcscpy;
    using std::wcslen;
    using std::wcsncmp;
    using std::wcsncpy;
    using std::wcsrchr;
    using std::wcsstr;
    using std::wcstok;
    using std::wcsxfrm;

    // Formatting
    using std::scanf;

    template<typename... ARGS>
    constexpr auto fprintf(auto file, const char* format, ARGS&&... args) noexcept {
        std::fprintf(file, format, utils::forward<ARGS>(args)...);// NOLINT
    }

    template<typename... ARGS>
    constexpr auto printf(const char* format, ARGS&&... args) noexcept {
        std::printf(format, utils::forward<ARGS>(args)...);// NOLINT
    }

    template<typename... ARGS>
    constexpr auto sprintf(char* buffer, const char* format, ARGS&&... args) noexcept {
        std::sprintf(buffer, format, utils::forward<ARGS>(args)...);// NOLINT
    }

    // Wide formatting
    using std::swprintf;

    template<typename... ARGS>
    constexpr auto fwprintf(auto file, const wchar_t* format, ARGS&&... args) noexcept {
        std::fwprintf(file, format, utils::forward<ARGS>(args)...);// NOLINT
    }

    template<typename... ARGS>
    constexpr auto wprintf(const wchar_t* format, ARGS&&... args) noexcept {
        std::wprintf(format, utils::forward<ARGS>(args)...);// NOLINT
    }

    template<typename... ARGS>
    constexpr auto wscanf(wchar_t* buffer, const wchar_t* format, ARGS&&... args) noexcept {
        std::wscanf(buffer, format, utils::forward<ARGS>(args)...);// NOLINT
    }

    // Strings
    template<typename T>
    [[nodiscard]] constexpr auto get_string_length(const T* str) noexcept -> usize {
        if constexpr(meta::is_same<T, char>) { return strlen(str); }
        else if constexpr(meta::is_same<T, wchar_t>) { return wcslen(str); }
        else {
            auto* ptr = str;
            while(*ptr != static_cast<T>('\0')) { ++ptr; }
            return static_cast<usize>(ptr - str);// Use pointer difference
        }
    }

    template<typename T>
    constexpr auto copy_string(T* dst, const T* src) noexcept -> void {
        if constexpr(meta::is_same<T, char>) { return strcpy(dst, src); }
        else if constexpr(meta::is_same<T, wchar_t>) { return wcscpy(dst, src); }
        else {
            const auto dst_length = get_string_length(dst);
            const auto src_length = get_string_length(src);
            const auto length = min(dst_length, src_length);
            memcpy(dst, src, (length + 1) * sizeof(T));
        }
    }

    template<typename T>
    constexpr auto copy_string(T* dst, const T* src, usize size) noexcept -> void {
        if constexpr(meta::is_same<T, char>) { strcpy(dst, src); }
        else if constexpr(meta::is_same<T, wchar_t>) { wcscpy(dst, src); }
        else {
            const auto dst_length = get_string_length(dst);
            const auto src_length = get_string_length(src);
            const auto length = min(dst_length, src_length);
            memcpy(dst, src, (length + 1) * sizeof(T));
        }
    }

    template<typename T>
    constexpr auto compare_string(const T* a, const T* b) noexcept -> i32 {
        if constexpr(meta::is_same<T, char>) { return strcmp(a, b); }
        else if constexpr(meta::is_same<T, wchar_t>) { return wcscmp(a, b); }
        else {
            const auto a_length = get_string_length(a);
            const auto b_length = get_string_length(b);

            if(a_length != b_length) {
                return static_cast<i32>(b_length - a_length);// Narrowing conversion -.-
            }

            return memcmp(a, b, a_length);
        }
    }

    template<typename T>
    constexpr auto concat_string(T* dst, const T* src) noexcept -> T* {
        if constexpr(meta::is_same<T, char>) { return strcat(dst, src); }
        else if constexpr(meta::is_same<T, wchar_t>) { return wcscat(dst, src); }
        else {
            memcpy(dst + get_string_length(dst), src, get_string_length(src) + 1);
            return dst;
        }
    }
}// namespace kstd::libc