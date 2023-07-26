#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <android/binder_interface_utils.h>
#ifdef BINDER_STABILITY_SUPPORT
#include <android/binder_stability.h>
#endif  // BINDER_STABILITY_SUPPORT

namespace aidl {
namespace vendor {
namespace chen {
namespace aidl {
namespace syshelper {
class IUdfpsHelperCallback : public ::ndk::ICInterface {
public:
  static const char* descriptor;
  IUdfpsHelperCallback();
  virtual ~IUdfpsHelperCallback();

  static constexpr uint32_t TRANSACTION_onUdfpsTouchStatusChanged = FIRST_CALL_TRANSACTION + 0;

  static std::shared_ptr<IUdfpsHelperCallback> fromBinder(const ::ndk::SpAIBinder& binder);
  static binder_status_t writeToParcel(AParcel* parcel, const std::shared_ptr<IUdfpsHelperCallback>& instance);
  static binder_status_t readFromParcel(const AParcel* parcel, std::shared_ptr<IUdfpsHelperCallback>* instance);
  static bool setDefaultImpl(const std::shared_ptr<IUdfpsHelperCallback>& impl);
  static const std::shared_ptr<IUdfpsHelperCallback>& getDefaultImpl();
  virtual ::ndk::ScopedAStatus onUdfpsTouchStatusChanged(bool in_isDown) = 0;
private:
  static std::shared_ptr<IUdfpsHelperCallback> default_impl;
};
class IUdfpsHelperCallbackDefault : public IUdfpsHelperCallback {
public:
  ::ndk::ScopedAStatus onUdfpsTouchStatusChanged(bool in_isDown) override;
  ::ndk::SpAIBinder asBinder() override;
  bool isRemote() override;
};
}  // namespace syshelper
}  // namespace aidl
}  // namespace chen
}  // namespace vendor
}  // namespace aidl
