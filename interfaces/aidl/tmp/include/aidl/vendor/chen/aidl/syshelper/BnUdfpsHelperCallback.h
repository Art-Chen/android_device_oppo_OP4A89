#pragma once

#include "aidl/vendor/chen/aidl/syshelper/IUdfpsHelperCallback.h"

#include <android/binder_ibinder.h>
#include <cassert>

#ifndef __BIONIC__
#ifndef __assert2
#define __assert2(a,b,c,d) ((void)0)
#endif
#endif

namespace aidl {
namespace vendor {
namespace chen {
namespace aidl {
namespace syshelper {
class BnUdfpsHelperCallback : public ::ndk::BnCInterface<IUdfpsHelperCallback> {
public:
  BnUdfpsHelperCallback();
  virtual ~BnUdfpsHelperCallback();
protected:
  ::ndk::SpAIBinder createBinder() override;
private:
};
class IUdfpsHelperCallbackDelegator : public BnUdfpsHelperCallback {
public:
  explicit IUdfpsHelperCallbackDelegator(const std::shared_ptr<IUdfpsHelperCallback> &impl) : _impl(impl) {
  }

  ::ndk::ScopedAStatus onUdfpsTouchStatusChanged(bool in_isDown) override {
    return _impl->onUdfpsTouchStatusChanged(in_isDown);
  }
protected:
private:
  std::shared_ptr<IUdfpsHelperCallback> _impl;
};

}  // namespace syshelper
}  // namespace aidl
}  // namespace chen
}  // namespace vendor
}  // namespace aidl
