#pragma once

#include "aidl/vendor/chen/aidl/syshelper/IUdfpsHelperCallback.h"

#include <android/binder_ibinder.h>

namespace aidl {
namespace vendor {
namespace chen {
namespace aidl {
namespace syshelper {
class BpUdfpsHelperCallback : public ::ndk::BpCInterface<IUdfpsHelperCallback> {
public:
  explicit BpUdfpsHelperCallback(const ::ndk::SpAIBinder& binder);
  virtual ~BpUdfpsHelperCallback();

  ::ndk::ScopedAStatus onUdfpsTouchStatusChanged(bool in_isDown) override;
};
}  // namespace syshelper
}  // namespace aidl
}  // namespace chen
}  // namespace vendor
}  // namespace aidl
