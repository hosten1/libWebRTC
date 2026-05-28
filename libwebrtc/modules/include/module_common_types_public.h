/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_INCLUDE_MODULE_COMMON_TYPES_PUBLIC_H_
#define MODULES_INCLUDE_MODULE_COMMON_TYPES_PUBLIC_H_

#include <stdint.h>

#include <memory>

#include "absl/types/optional.h"

#if !defined(_WIN32)
#include <limits>
#endif

namespace webrtc {

// Utility class to unwrap a number to a larger type. The numbers will never be
// unwrapped to a negative value.
template <typename U>
class Unwrapper {
#if !defined(_WIN32)
  static_assert(!std::numeric_limits<U>::is_signed, "U must be unsigned");
  static_assert(std::numeric_limits<U>::max() <=
                    std::numeric_limits<uint32_t>::max(),
                "U must not be wider than 32 bits");
#endif

 public:
  // Get the unwrapped value, but don't update the internal state.
  int64_t UnwrapWithoutUpdate(U value) const {
    if (!last_value_)
      return value;

#if defined(_WIN32)
    constexpr int64_t kMaxPlusOne =
        static_cast<int64_t>(static_cast<U>(-1)) + 1;
#else
    constexpr int64_t kMaxPlusOne =
        static_cast<int64_t>(std::numeric_limits<U>::max()) + 1;
#endif

    U cropped_last = static_cast<U>(*last_value_);
    int64_t delta = value - cropped_last;
    if (IsNewer(value, cropped_last)) {
      if (delta < 0)
        delta += kMaxPlusOne;  // Wrap forwards.
    } else if (delta > 0 && (*last_value_ + delta - kMaxPlusOne) >= 0) {
      // If value is older but delta is positive, this is a backwards
      // wrap-around. However, don't wrap backwards past 0 (unwrapped).
      delta -= kMaxPlusOne;
    }

    return *last_value_ + delta;
  }

  // Only update the internal state to the specified last (unwrapped) value.
  void UpdateLast(int64_t last_value) { last_value_ = last_value; }

  // Unwrap the value and update the internal state.
  int64_t Unwrap(U value) {
    int64_t unwrapped = UnwrapWithoutUpdate(value);
    UpdateLast(unwrapped);
    return unwrapped;
  }

 private:
#if defined(_WIN32)
  bool IsNewer(U value, U prev_value) const {
    constexpr U kMaxValue = static_cast<U>(-1);
    constexpr U kHalfMaxValue = kMaxValue / 2 + 1;
    return (value != prev_value) &&
           ((value > prev_value && value - prev_value <= kHalfMaxValue) ||
            (value < prev_value && prev_value - value > kHalfMaxValue));
  }
#else
  bool IsNewer(U value, U prev_value) const {
    constexpr U kMaxValue = std::numeric_limits<U>::max();
    constexpr U kHalfMaxValue = kMaxValue / 2 + 1;
    return (value != prev_value) &&
           ((value > prev_value && value - prev_value <= kHalfMaxValue) ||
            (value < prev_value && prev_value - value > kHalfMaxValue));
  }
#endif

  absl::optional<int64_t> last_value_;
};

using SequenceNumberUnwrapper = Unwrapper<uint16_t>;
using TimestampUnwrapper = Unwrapper<uint32_t>;

}  // namespace webrtc

#endif  // MODULES_INCLUDE_MODULE_COMMON_TYPES_PUBLIC_H_
