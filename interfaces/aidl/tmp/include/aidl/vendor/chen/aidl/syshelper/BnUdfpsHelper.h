#pragma once

#include "aidl/vendor/chen/aidl/syshelper/IUdfpsHelper.h"

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
class BnUdfpsHelper : public ::ndk::BnCInterface<IUdfpsHelper> {
public:
  BnUdfpsHelper();
  virtual ~BnUdfpsHelper();
protected:
  ::ndk::SpAIBinder createBinder() override;
private:
};
class IUdfpsHelperDelegator : public BnUdfpsHelper {
public:
  explicit IUdfpsHelperDelegator(const std::shared_ptr<IUdfpsHelper> &impl) : _impl(impl) {
  }

  ::ndk::ScopedAStatus touchDown() override {
    return _impl->touchDown();
  }
  ::ndk::ScopedAStatus touchUp() override {
    return _impl->touchUp();
  }
  ::ndk::ScopedAStatus registerCallback(const std::shared_ptr<::aidl::vendor::chen::aidl::syshelper::IUdfpsHelperCallback>& in_callback) override {
    return _impl->registerCallback(in_callback);
  }
  ::ndk::ScopedAStatus unregisterCallback(const std::shared_ptr<::aidl::vendor::chen::aidl::syshelper::IUdfpsHelperCallback>& in_callback) override {
    return _impl->unregisterCallback(in_callback);
  }
protected:
private:
  std::shared_ptr<IUdfpsHelper> _impl;
};

}  // namespace syshelper
}  // namespace aidl
}  // namespace chen
}  // namespace vendor
}  // namespace aidl
