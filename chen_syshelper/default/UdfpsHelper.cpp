#include <UdfpsHelper.h>
#include <android-base/logging.h>

#include "LinkedCallback.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "UdfpsHelper"

namespace aidl {
namespace vendor {
namespace chen {
namespace aidl {
namespace syshelper {

namespace {
// Wrap LinkedCallback::OnCallbackDied() into a void(void*).
void OnCallbackDiedWrapped(void* cookie) {
    LinkedCallback* linked = reinterpret_cast<LinkedCallback*>(cookie);
    linked->OnCallbackDied();
}
}  // namespace

namespace {
bool IsDeadObjectLogged(const ndk::ScopedAStatus& ret) {
    if (ret.isOk()) return false;
    if (ret.getStatus() == ::STATUS_DEAD_OBJECT) return true;
    LOG(ERROR) << "Cannot call onUdfpsTouchStatusChanged on callback: " << ret.getDescription();
    return false;
}
}  // namespace

UdfpsHelper::UdfpsHelper()
    : currentIsDownState(false),
      death_recipient_(AIBinder_DeathRecipient_new(&OnCallbackDiedWrapped)) {
    mThread = std::thread(startThread, this);
}

::ndk::ScopedAStatus UdfpsHelper::touchDown() {
    std::unique_lock<decltype(fpdown_mutex_lock)> lock(fpdown_mutex_lock);

    LOG(INFO) << "FP Touch Down!";
    currentIsDownState = true;
    // OnUdfpsTouchStatusChanged(true);
    mWaitCV.notify_all();
    lock.unlock();

    return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus UdfpsHelper::touchUp() {
    std::unique_lock<decltype(fpdown_mutex_lock)> lock(fpdown_mutex_lock);

    LOG(INFO) << "FP Touch Up!";
    currentIsDownState = false;
    // OnUdfpsTouchStatusChanged(false);
    mWaitCV.notify_all();
    lock.unlock();

    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus UdfpsHelper::getTouchStatus(bool* aidl_return) {
    *aidl_return = currentIsDownState;

    return ndk::ScopedAStatus::ok();
}

void UdfpsHelper::OnUdfpsTouchStatusChanged(bool isDowned) {
    // Notify all callbacks
    std::unique_lock<decltype(callback_mutex_lock)> lock(callback_mutex_lock);

    // is_dead notifies a callback and return true if it is dead.
    auto is_dead = [&](const auto& linked) {
        auto res = linked->callback()->onUdfpsTouchStatusChanged(isDowned);
        return IsDeadObjectLogged(res);
    };
    auto it = std::remove_if(callbacks_.begin(), callbacks_.end(), is_dead);
    callbacks_.erase(it, callbacks_.end());  // calls unlinkToDeath on deleted callbacks.
    lock.unlock();
}

void UdfpsHelper::run() {
    std::unique_lock<std::mutex> runLock(run_mutex_lock);

    while (true) {
        if (!currentIsDownState) {
            mWaitCV.wait(runLock, [&] {
                return currentIsDownState;
            });
        }
        else {
            sleep(3);
            if (currentIsDownState) {
                LOG(WARNING) << "FP Force Touch up because down time is larger than 3s!";
                touchUp();
            }
        }
    }
}

void UdfpsHelper::startThread(UdfpsHelper* helper) {
    helper->run();
}

::ndk::ScopedAStatus UdfpsHelper::registerCallback(const std::shared_ptr<::aidl::vendor::chen::aidl::syshelper::IUdfpsHelperCallback>& callback) {
    if (callback == nullptr) {
        // For now, this shouldn't happen because argument is not nullable.
        return ndk::ScopedAStatus::fromExceptionCode(EX_NULL_POINTER);
    }

    {
        std::lock_guard<decltype(callback_mutex_lock)> lock(callback_mutex_lock);
        callbacks_.emplace_back(LinkedCallback::Make(ref<UdfpsHelper>(), callback));
        // unlock
    }

    if (auto res = callback->onUdfpsTouchStatusChanged(currentIsDownState); IsDeadObjectLogged(res)) {
        LOG(ERROR) << "register Callback is Dead Object, remove!";
        (void)unregisterCallback(callback);
    }

    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus UdfpsHelper::unregisterCallback(const std::shared_ptr<::aidl::vendor::chen::aidl::syshelper::IUdfpsHelperCallback>& in_callback) {
    if (in_callback == nullptr) {
        // For now, this shouldn't happen because argument is not nullable.
        return ndk::ScopedAStatus::fromExceptionCode(EX_NULL_POINTER);
    }

    std::lock_guard<decltype(callback_mutex_lock)> lock(callback_mutex_lock);

    auto matches = [in_callback](const auto& linked) {
        return linked->callback()->asBinder() == in_callback->asBinder();  // compares binder object
    };
    auto it = std::remove_if(callbacks_.begin(), callbacks_.end(), matches);
    bool removed = (it != callbacks_.end());
    callbacks_.erase(it, callbacks_.end());  // calls unlinkToDeath on deleted callbacks.

    LOG(INFO) << "UdfpsHelper: unregistered Callback!";

    return removed ? ndk::ScopedAStatus::ok()
                   : ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_ARGUMENT);
}

}  // namespace syshelper
}  // namespace aidl
}  // namespace chen
}  // namespace vendor
}  // namespace aidl