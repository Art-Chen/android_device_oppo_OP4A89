#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <android/binder_interface_utils.h>
#include <android/binder_parcelable_utils.h>
#include <android/binder_to_string.h>
#ifdef BINDER_STABILITY_SUPPORT
#include <android/binder_stability.h>
#endif  // BINDER_STABILITY_SUPPORT

namespace aidl {
namespace vendor {
namespace chen {
namespace aidl {
namespace syshelper {
class ScreenShotInfo {
public:
  typedef std::false_type fixed_size;
  static const char* descriptor;

  int32_t r = 0;
  int32_t g = 0;
  int32_t b = 0;
  int64_t timestamp = 0L;

  binder_status_t readFromParcel(const AParcel* parcel);
  binder_status_t writeToParcel(AParcel* parcel) const;

  inline bool operator!=(const ScreenShotInfo& rhs) const {
    return std::tie(r, g, b, timestamp) != std::tie(rhs.r, rhs.g, rhs.b, rhs.timestamp);
  }
  inline bool operator<(const ScreenShotInfo& rhs) const {
    return std::tie(r, g, b, timestamp) < std::tie(rhs.r, rhs.g, rhs.b, rhs.timestamp);
  }
  inline bool operator<=(const ScreenShotInfo& rhs) const {
    return std::tie(r, g, b, timestamp) <= std::tie(rhs.r, rhs.g, rhs.b, rhs.timestamp);
  }
  inline bool operator==(const ScreenShotInfo& rhs) const {
    return std::tie(r, g, b, timestamp) == std::tie(rhs.r, rhs.g, rhs.b, rhs.timestamp);
  }
  inline bool operator>(const ScreenShotInfo& rhs) const {
    return std::tie(r, g, b, timestamp) > std::tie(rhs.r, rhs.g, rhs.b, rhs.timestamp);
  }
  inline bool operator>=(const ScreenShotInfo& rhs) const {
    return std::tie(r, g, b, timestamp) >= std::tie(rhs.r, rhs.g, rhs.b, rhs.timestamp);
  }

  static const ::ndk::parcelable_stability_t _aidl_stability = ::ndk::STABILITY_VINTF;
  inline std::string toString() const {
    std::ostringstream os;
    os << "ScreenShotInfo{";
    os << "r: " << ::android::internal::ToString(r);
    os << ", g: " << ::android::internal::ToString(g);
    os << ", b: " << ::android::internal::ToString(b);
    os << ", timestamp: " << ::android::internal::ToString(timestamp);
    os << "}";
    return os.str();
  }
};
}  // namespace syshelper
}  // namespace aidl
}  // namespace chen
}  // namespace vendor
}  // namespace aidl
