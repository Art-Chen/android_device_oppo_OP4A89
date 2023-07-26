#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <android/binder_interface_utils.h>
#include <aidl/vendor/chen/aidl/syshelper/ScreenShotInfo.h>
#ifdef BINDER_STABILITY_SUPPORT
#include <android/binder_stability.h>
#endif  // BINDER_STABILITY_SUPPORT

namespace aidl {
namespace vendor {
namespace chen {
namespace aidl {
namespace syshelper {
class IALSHelperCallback : public ::ndk::ICInterface {
public:
  static const char* descriptor;
  IALSHelperCallback();
  virtual ~IALSHelperCallback();

  static constexpr uint32_t TRANSACTION_takeScreenShot = FIRST_CALL_TRANSACTION + 0;

  static std::shared_ptr<IALSHelperCallback> fromBinder(const ::ndk::SpAIBinder& binder);
  static binder_status_t writeToParcel(AParcel* parcel, const std::shared_ptr<IALSHelperCallback>& instance);
  static binder_status_t readFromParcel(const AParcel* parcel, std::shared_ptr<IALSHelperCallback>* instance);
  static bool setDefaultImpl(const std::shared_ptr<IALSHelperCallback>& impl);
  static const std::shared_ptr<IALSHelperCallback>& getDefaultImpl();
  virtual ::ndk::ScopedAStatus takeScreenShot(::aidl::vendor::chen::aidl::syshelper::ScreenShotInfo* _aidl_return) = 0;
private:
  static std::shared_ptr<IALSHelperCallback> default_impl;
};
class IALSHelperCallbackDefault : public IALSHelperCallback {
public:
  ::ndk::ScopedAStatus takeScreenShot(::aidl::vendor::chen::aidl::syshelper::ScreenShotInfo* _aidl_return) override;
  ::ndk::SpAIBinder asBinder() override;
  bool isRemote() override;
};
}  // namespace syshelper
}  // namespace aidl
}  // namespace chen
}  // namespace vendor
}  // namespace aidl
