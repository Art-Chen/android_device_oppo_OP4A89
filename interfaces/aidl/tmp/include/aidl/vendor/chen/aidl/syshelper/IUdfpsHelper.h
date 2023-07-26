#pragma once

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <android/binder_interface_utils.h>
#include <aidl/vendor/chen/aidl/syshelper/IUdfpsHelperCallback.h>
#ifdef BINDER_STABILITY_SUPPORT
#include <android/binder_stability.h>
#endif  // BINDER_STABILITY_SUPPORT

namespace aidl {
namespace vendor {
namespace chen {
namespace aidl {
namespace syshelper {
class IUdfpsHelper : public ::ndk::ICInterface {
public:
  static const char* descriptor;
  IUdfpsHelper();
  virtual ~IUdfpsHelper();

  static constexpr uint32_t TRANSACTION_touchDown = FIRST_CALL_TRANSACTION + 0;
  static constexpr uint32_t TRANSACTION_touchUp = FIRST_CALL_TRANSACTION + 1;
  static constexpr uint32_t TRANSACTION_registerCallback = FIRST_CALL_TRANSACTION + 2;
  static constexpr uint32_t TRANSACTION_unregisterCallback = FIRST_CALL_TRANSACTION + 3;

  static std::shared_ptr<IUdfpsHelper> fromBinder(const ::ndk::SpAIBinder& binder);
  static binder_status_t writeToParcel(AParcel* parcel, const std::shared_ptr<IUdfpsHelper>& instance);
  static binder_status_t readFromParcel(const AParcel* parcel, std::shared_ptr<IUdfpsHelper>* instance);
  static bool setDefaultImpl(const std::shared_ptr<IUdfpsHelper>& impl);
  static const std::shared_ptr<IUdfpsHelper>& getDefaultImpl();
  virtual ::ndk::ScopedAStatus touchDown() = 0;
  virtual ::ndk::ScopedAStatus touchUp() = 0;
  virtual ::ndk::ScopedAStatus registerCallback(const std::shared_ptr<::aidl::vendor::chen::aidl::syshelper::IUdfpsHelperCallback>& in_callback) = 0;
  virtual ::ndk::ScopedAStatus unregisterCallback(const std::shared_ptr<::aidl::vendor::chen::aidl::syshelper::IUdfpsHelperCallback>& in_callback) = 0;
private:
  static std::shared_ptr<IUdfpsHelper> default_impl;
};
class IUdfpsHelperDefault : public IUdfpsHelper {
public:
  ::ndk::ScopedAStatus touchDown() override;
  ::ndk::ScopedAStatus touchUp() override;
  ::ndk::ScopedAStatus registerCallback(const std::shared_ptr<::aidl::vendor::chen::aidl::syshelper::IUdfpsHelperCallback>& in_callback) override;
  ::ndk::ScopedAStatus unregisterCallback(const std::shared_ptr<::aidl::vendor::chen::aidl::syshelper::IUdfpsHelperCallback>& in_callback) override;
  ::ndk::SpAIBinder asBinder() override;
  bool isRemote() override;
};
}  // namespace syshelper
}  // namespace aidl
}  // namespace chen
}  // namespace vendor
}  // namespace aidl
