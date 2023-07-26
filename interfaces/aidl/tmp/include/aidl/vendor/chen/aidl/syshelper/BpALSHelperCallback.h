#pragma once

#include "aidl/vendor/chen/aidl/syshelper/IALSHelperCallback.h"

#include <android/binder_ibinder.h>

namespace aidl {
namespace vendor {
namespace chen {
namespace aidl {
namespace syshelper {
class BpALSHelperCallback : public ::ndk::BpCInterface<IALSHelperCallback> {
public:
  explicit BpALSHelperCallback(const ::ndk::SpAIBinder& binder);
  virtual ~BpALSHelperCallback();

  ::ndk::ScopedAStatus takeScreenShot(::aidl::vendor::chen::aidl::syshelper::ScreenShotInfo* _aidl_return) override;
};
}  // namespace syshelper
}  // namespace aidl
}  // namespace chen
}  // namespace vendor
}  // namespace aidl
