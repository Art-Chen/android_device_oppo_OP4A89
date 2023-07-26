#pragma once

#include "aidl/vendor/chen/aidl/syshelper/IALSHelperCallback.h"

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
class BnALSHelperCallback : public ::ndk::BnCInterface<IALSHelperCallback> {
public:
  BnALSHelperCallback();
  virtual ~BnALSHelperCallback();
protected:
  ::ndk::SpAIBinder createBinder() override;
private:
};
class IALSHelperCallbackDelegator : public BnALSHelperCallback {
public:
  explicit IALSHelperCallbackDelegator(const std::shared_ptr<IALSHelperCallback> &impl) : _impl(impl) {
  }

  ::ndk::ScopedAStatus takeScreenShot(::aidl::vendor::chen::aidl::syshelper::ScreenShotInfo* _aidl_return) override {
    return _impl->takeScreenShot(_aidl_return);
  }
protected:
private:
  std::shared_ptr<IALSHelperCallback> _impl;
};

}  // namespace syshelper
}  // namespace aidl
}  // namespace chen
}  // namespace vendor
}  // namespace aidl
