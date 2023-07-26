#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <android/binder_interface_utils.h>
#include <aidl/vendor/chen/aidl/syshelper/IALSHelperCallback.h>
#ifdef BINDER_STABILITY_SUPPORT
#include <android/binder_stability.h>
#endif  // BINDER_STABILITY_SUPPORT

namespace aidl {
namespace vendor {
namespace chen {
namespace aidl {
namespace syshelper {
class IALSHelper : public ::ndk::ICInterface {
public:
  static const char* descriptor;
  IALSHelper();
  virtual ~IALSHelper();

  static constexpr uint32_t TRANSACTION_registerCallback = FIRST_CALL_TRANSACTION + 0;
  static constexpr uint32_t TRANSACTION_unregisterCallback = FIRST_CALL_TRANSACTION + 1;

  static std::shared_ptr<IALSHelper> fromBinder(const ::ndk::SpAIBinder& binder);
  static binder_status_t writeToParcel(AParcel* parcel, const std::shared_ptr<IALSHelper>& instance);
  static binder_status_t readFromParcel(const AParcel* parcel, std::shared_ptr<IALSHelper>* instance);
  static bool setDefaultImpl(const std::shared_ptr<IALSHelper>& impl);
  static const std::shared_ptr<IALSHelper>& getDefaultImpl();
  virtual ::ndk::ScopedAStatus registerCallback(const std::shared_ptr<::aidl::vendor::chen::aidl::syshelper::IALSHelperCallback>& in_callback) = 0;
  virtual ::ndk::ScopedAStatus unregisterCallback(const std::shared_ptr<::aidl::vendor::chen::aidl::syshelper::IALSHelperCallback>& in_callback) = 0;
private:
  static std::shared_ptr<IALSHelper> default_impl;
};
class IALSHelperDefault : public IALSHelper {
public:
  ::ndk::ScopedAStatus registerCallback(const std::shared_ptr<::aidl::vendor::chen::aidl::syshelper::IALSHelperCallback>& in_callback) override;
  ::ndk::ScopedAStatus unregisterCallback(const std::shared_ptr<::aidl::vendor::chen::aidl::syshelper::IALSHelperCallback>& in_callback) override;
  ::ndk::SpAIBinder asBinder() override;
  bool isRemote() override;
};
}  // namespace syshelper
}  // namespace aidl
}  // namespace chen
}  // namespace vendor
}  // namespace aidl
