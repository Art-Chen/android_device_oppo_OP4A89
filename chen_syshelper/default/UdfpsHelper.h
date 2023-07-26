#pragma once

#include <memory>
#include <optional>

#include <aidl/vendor/chen/aidl/syshelper/BnUdfpsHelper.h>
// #include <aidl/vendor/chen/aidl/syshelper/IUdfpsHelper.h>
#include <aidl/vendor/chen/aidl/syshelper/IUdfpsHelperCallback.h>

#include <mutex>
#include <thread>
#include <condition_variable>

namespace aidl {
namespace vendor {
namespace chen {
namespace aidl {
namespace syshelper {

class LinkedCallback;

class UdfpsHelper : public BnUdfpsHelper {
   public:
    UdfpsHelper();
    ::ndk::ScopedAStatus touchDown() override;
    ::ndk::ScopedAStatus touchUp() override;
    ::ndk::ScopedAStatus getTouchStatus(bool *aidl_return) override;
    ::ndk::ScopedAStatus registerCallback(const std::shared_ptr<::aidl::vendor::chen::aidl::syshelper::IUdfpsHelperCallback>& in_callback) override;
    ::ndk::ScopedAStatus unregisterCallback(const std::shared_ptr<::aidl::vendor::chen::aidl::syshelper::IUdfpsHelperCallback>& in_callback) override;

   private:
    friend LinkedCallback;  // for exposing death_recipient_

    static void startThread(UdfpsHelper* helper);
    void run();
    void OnUdfpsTouchStatusChanged(bool isDowned);
    bool currentIsDownState;
    std::mutex fpdown_mutex_lock;
    std::mutex run_mutex_lock;
    std::mutex callback_mutex_lock;
    std::vector<std::unique_ptr<LinkedCallback>> callbacks_;
    std::thread mThread;
    std::condition_variable mWaitCV;
    ndk::ScopedAIBinder_DeathRecipient death_recipient_;
};

}  // namespace syshelper
}  // namespace aidl
}  // namespace chen
}  // namespace vendor
}  // namespace aidl