#pragma once

#include "aidl/vendor/chen/aidl/syshelper/IALSHelper.h"

#include <android/binder_ibinder.h>

namespace aidl {
namespace vendor {
namespace chen {
namespace aidl {
namespace syshelper {
class BpALSHelper : public ::ndk::BpCInterface<IALSHelper> {
public:
  explicit BpALSHelper(const ::ndk::SpAIBinder& binder);
  virtual ~BpALSHelper();

  ::ndk::ScopedAStatus registerCallback(const std::shared_ptr<::aidl::vendor::chen::aidl::syshelper::IALSHelperCallback>& in_callback) override;
  ::ndk::ScopedAStatus unregisterCallback(const std::shared_ptr<::aidl::vendor::chen::aidl::syshelper::IALSHelperCallback>& in_callback) override;
};
}  // namespace syshelper
}  // namespace aidl
}  // namespace chen
}  // namespace vendor
}  // namespace aidl
