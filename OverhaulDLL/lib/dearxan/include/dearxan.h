#pragma once

#ifndef _DEARXAN_H
#define _DEARXAN_H

#ifdef __cplusplus
#include <cstddef>
#include <cstring>
#include <functional>
#include <memory>
#include <string>
#include <utility>

#if __cplusplus >= 201703 || _MSVC_LANG >= 201703
#include <optional>
#else
#include <stdexcept>
#endif

namespace dearxan {
namespace detail {
extern "C" {
#else
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

/// The size of `DearxanResult` in bytes, WITHOUT the trailing padding.
#define DEARXAN_RESULT_SIZE offsetof(DearxanResult, _last_for_offsetof)

/// Verify field `field` is in bounds in a `DearxanResult`.
/// 
/// If the member field is in bounds, `then_expr` is executed, otherwise
/// `else_expr` is executed.
#define DEARXAN_RESULT_FIELD(ptr, field, then_expr, else_expr) do { \
        if (offsetof(DearxanResult, field) < ptr->result_size) {    \
            then_expr;                                              \
        } else {                                                    \
            else_expr;                                              \
        }                                                           \
    } while(0)
#endif

/// Possible values of the field `status` inside `DearxanResult`.
///
/// `DearxanError` and `DearxanPanic` may mean the result contains and error message.
typedef enum DearxanStatus {
    DearxanInvalid,
    DearxanSuccess,
    DearxanError,
    DearxanPanic,
    DearxanMaxStatus
} DearxanStatus;

/// The outcome of a call to `dearxan_neuter_arxan`.
/// 
/// Contains its own size in bytes as the first member field for the purpose
/// of versioning when another instance of `dearxan` handles the call.
/// 
/// To maintain ABI stability, future `dearxan` versions are not permitted to
/// remove or reorder fields, any new fields must be added before `_last_for_offsetof`.
typedef struct DearxanResult {
    /// Size of this `DearxanResult` struct. 
    ///
    /// Only fields whose end offset fit within this size are valid for reads. Consider using
    /// the `DEARXAN_RESULT_FIELD` macro to check this automatically.
    size_t result_size;
    /// The status of `dearxan_neuter_arxan`.
    ///
    /// This is expected to be a value of `DearxanStatus`, but is non-exhaustive as other statuses
    /// may be added in the future. As such it is typed as an `int`.
    int status;
    /// The error or panic message, if `status` is `DearxanError` or `DearxanPanic`. 
    const char* error_msg;
    /// The size of the error or panic message.
    size_t error_msg_size;
    /// Whether Arxan was detected or not.
    bool is_arxan_detected;
    /// If true, the callback execution is blocking the program entry point. If false, the callback
    /// that received this `DearxanResult` is being executed in a separate thread.
    ///
    /// In either case, it is guaranteed that the Arxan entry point stub has finished initializing
    /// once the callback runs.
    bool is_executing_entrypoint;
    char _last_for_offsetof;
} DearxanResult;

/// Callback invoked once arxan has been disabled (or if it wasn't detected).
typedef void (*DearxanUserCallback)(const DearxanResult* result, void* opaque);

/// Single function to neuter all of Arxan's checks.
/// 
/// The callback will be invoked with a pointer to a `DearxanResult` containing
/// fields indicating whether Arxan was detected and whether entry point execution is being
/// blocked while the callback is running. Modulo any reported error, it is safe to assume 
/// that Arxan has been disabled once it executes.
/// 
/// Handles SteamStub 3.1 possibly being applied on top of Arxan.
extern void dearxan_neuter_arxan(DearxanUserCallback callback, void* opaque);

#ifdef __cplusplus
}
} // namespace detail

/// Possible values of `status` inside `DearxanResult`.
///
/// `DearxanError` and `DearxanPanic` may mean the result contains and error message.
using DearxanStatus = detail::DearxanStatus;

/// The size of `DearxanResult` in bytes, WITHOUT the trailing padding.
/// 
/// Internal macro for use in `DearxanResult` below.
#define DEARXAN_RESULT_SIZE \
    offsetof(detail::DearxanResult, _last_for_offsetof)

/// Verify field `field` is in bounds in a `DearxanResult`.
/// 
/// Internal macro for use in `DearxanResult` below.
/// 
/// If the member field is in bounds, `then_expr` is executed, otherwise
/// `else_expr` is executed.
#define DEARXAN_RESULT_FIELD(ptr, field, then_expr, else_expr) do {      \
        if (offsetof(detail::DearxanResult, field) < ptr->result_size) { \
            then_expr;                                                   \
        } else {                                                         \
            else_expr;                                                   \
        }                                                                \
    } while(0)

#if __cplusplus >= 201703 || _MSVC_LANG >= 201703
/// Declare a getter for field `field` that performs bounds checking.
/// 
/// Internal macro for use in `DearxanResult` below.
/// 
/// The field must exist in `detail::DearxanResult`.
#define DEARXAN_DECLARE_FIELD(field) auto field() const {                  \
        DEARXAN_RESULT_FIELD(                                              \
            static_cast<const detail::DearxanResult*>(this),               \
            field,                                                         \
            return std::optional(detail::DearxanResult::field),            \
            return std::optional<decltype(detail::DearxanResult::field)>() \
        );                                                                 \
    }
#else
#define DEARXAN_DECLARE_FIELD(field) auto field() const {       \
        DEARXAN_RESULT_FIELD(                                   \
            static_cast<const detail::DearxanResult*>(this),    \
            field,                                              \
            return detail::DearxanResult::field,                \
            throw std::length_error(                            \
                "old DearxanResult layout lacks field " #field) \
        );                                                      \
    }
#endif

/// The outcome of a call to `dearxan_neuter_arxan`.
/// 
/// Performs bounds checking utilizing its own size in bytes for the purpose
/// of versioning when another instance of `dearxan` handles the call.
/// 
/// To maintain ABI stability, future `dearxan` versions are not permitted to
/// remove or reorder fields.
struct DearxanResult : private detail::DearxanResult {
    explicit DearxanResult(const detail::DearxanResult* ptr) : detail::DearxanResult{} {
        if (ptr == nullptr) {
            return;
        }

        size_t result_size = DEARXAN_RESULT_SIZE < ptr->result_size
            ? DEARXAN_RESULT_SIZE : ptr->result_size;

        std::memcpy(
            static_cast<void*>(static_cast<detail::DearxanResult*>(this)),
            static_cast<const void*>(ptr),
            result_size
        );

        this->result_size = result_size;
    }
    
    /// The status of `dearxan_neuter_arxan`.
    ///
    /// This is expected to be a valid value of `DearxanStatus`, but is non-exhaustive as other statuses
    /// may be added in the future. Hence it is recommended to compare it against `DearxanMaxStatus`.
    DearxanStatus status() const noexcept {
        return static_cast<DearxanStatus>(detail::DearxanResult::status);
    }

    /// The error or panic message, if `status` is `DearxanError` or `DearxanPanic`. 
    std::string error_msg() const noexcept {
        return std::string(detail::DearxanResult::error_msg,
            detail::DearxanResult::error_msg_size);
    }

    /// Whether Arxan was detected or not.
    bool is_arxan_detected() const noexcept {
        return detail::DearxanResult::is_arxan_detected;
    }

    /// If true, the callback execution is blocking the program entry point. If false, the callback
    /// that received this `DearxanResult` is being executed in a separate thread.
    ///
    /// In either case, it is guaranteed that the Arxan entry point stub has finished initializing
    /// once the callback runs.
    bool is_executing_entrypoint() const noexcept {
        return detail::DearxanResult::is_executing_entrypoint;
    }
};

/// Callback invoked once arxan has been disabled (or if it wasn't detected).
using DearxanUserCallback = std::function<void(const DearxanResult&)>;

/// Single function to neuter all of Arxan's checks.
/// 
/// The callback will be invoked with a pointer to a `DearxanResult` containing
/// fields indicating whether Arxan was detected and whether entry point execution is being
/// blocked while the callback is running. Modulo any reported error, it is safe to assume 
/// that Arxan has been disabled once it executes.
/// 
/// Handles SteamStub 3.1 possibly being applied on top of Arxan.
inline void neuter_arxan(DearxanUserCallback f) {
    auto boxed_function =
        std::make_unique<DearxanUserCallback>(std::move(f)).release();

    auto callback = +[](const detail::DearxanResult* result, void* opaque) {
        auto boxed_function =
            std::unique_ptr<DearxanUserCallback>(reinterpret_cast<DearxanUserCallback*>(opaque));
        (*boxed_function.get())(DearxanResult(result));
    };

    detail::dearxan_neuter_arxan(callback, static_cast<void*>(boxed_function));
}
} // namespace dearxan

#undef DEARXAN_RESULT_SIZE
#undef DEARXAN_RESULT_FIELD
#undef DEARXAN_DECLARE_FIELD

#endif

#endif
