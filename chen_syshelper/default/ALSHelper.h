#pragma once

#include <aidl/vendor/chen/aidl/syshelper/BnALSHelper.h>
// #include <aidl/vendor/chen/aidl/syshelper/IALSHelper.h>
#include <aidl/vendor/chen/aidl/syshelper/IALSHelperCallback.h>
#include <aidl/vendor/chen/aidl/syshelper/ScreenShotInfo.h>

#include <mutex>

namespace aidl {
namespace vendor {
namespace chen {
namespace aidl {
namespace syshelper {

class ALSHelper : public BnALSHelper {
   public:
    ::ndk::ScopedAStatus takeScreenShot(::aidl::vendor::chen::aidl::syshelper::ScreenShotInfo* _aidl_return) override;
    ::ndk::ScopedAStatus registerCallback(const std::shared_ptr<::aidl::vendor::chen::aidl::syshelper::IALSHelperCallback>& in_callback) override {
        unregisterCallback(in_callback);
        callback_ = std::move(in_callback);
        return ::ndk::ScopedAStatus::ok();
    };
    ::ndk::ScopedAStatus unregisterCallback(const std::shared_ptr<::aidl::vendor::chen::aidl::syshelper::IALSHelperCallback>& /*in_callback*/) override {
        callback_ = nullptr;
        return ::ndk::ScopedAStatus::ok();
    };

   private:
    std::mutex callback_mutex_lock;
    std::shared_ptr<::aidl::vendor::chen::aidl::syshelper::IALSHelperCallback> callback_;
};

}  // namespace syshelper
}  // namespace aidl
}  // namespace chen
}  // namespace vendor
}  // namespace aidl