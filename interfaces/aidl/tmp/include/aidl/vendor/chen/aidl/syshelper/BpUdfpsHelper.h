#pragma once

#include "aidl/vendor/chen/aidl/syshelper/IUdfpsHelper.h"

#include <android/binder_ibinder.h>

namespace aidl {
namespace vendor {
namespace chen {
namespace aidl {
namespace syshelper {
class BpUdfpsHelper : public ::ndk::BpCInterface<IUdfpsHelper> {
public:
  explicit BpUdfpsHelper(const ::ndk::SpAIBinder& binder);
  virtual ~BpUdfpsHelper();

  ::ndk::ScopedAStatus touchDown() override;
  ::ndk::ScopedAStatus touchUp() override;
  ::ndk::ScopedAStatus registerCallback(const std::shared_ptr<::aidl::vendor::chen::aidl::syshelper::IUdfpsHelperCallback>& in_callback) override;
  ::ndk::ScopedAStatus unregisterCallback(const std::shared_ptr<::aidl::vendor::chen::aidl::syshelper::IUdfpsHelperCallback>& in_callback) override;
};
}  // namespace syshelper
}  // namespace aidl
}  // namespace chen
}  // namespace vendor
}  // namespace aidl
