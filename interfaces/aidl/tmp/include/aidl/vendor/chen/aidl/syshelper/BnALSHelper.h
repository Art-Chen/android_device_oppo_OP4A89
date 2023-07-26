#pragma once

#include "aidl/vendor/chen/aidl/syshelper/IALSHelper.h"

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
class BnALSHelper : public ::ndk::BnCInterface<IALSHelper> {
public:
  BnALSHelper();
  virtual ~BnALSHelper();
protected:
  ::ndk::SpAIBinder createBinder() override;
private:
};
class IALSHelperDelegator : public BnALSHelper {
public:
  explicit IALSHelperDelegator(const std::shared_ptr<IALSHelper> &impl) : _impl(impl) {
  }

  ::ndk::ScopedAStatus registerCallback(const std::shared_ptr<::aidl::vendor::chen::aidl::syshelper::IALSHelperCallback>& in_callback) override {
    return _impl->registerCallback(in_callback);
  }
  ::ndk::ScopedAStatus unregisterCallback(const std::shared_ptr<::aidl::vendor::chen::aidl::syshelper::IALSHelperCallback>& in_callback) override {
    return _impl->unregisterCallback(in_callback);
  }
protected:
private:
  std::shared_ptr<IALSHelper> _impl;
};

}  // namespace syshelper
}  // namespace aidl
}  // namespace chen
}  // namespace vendor
}  // namespace aidl
