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
// Delete the owned cookie.
void onCallbackUnlinked(void* cookie) {
    LinkedCallback* linked = reinterpret_cast<LinkedCallback*>(cookie);
    delete linked;
}
}  // namespace

UdfpsHelper::UdfpsHelper()
    : currentIsDownState(false),
      death_recipient_(AIBinder_DeathRecipient_new(&OnCallbackDiedWrapped)) {
    AIBinder_DeathRecipient_setOnUnlinked(death_recipient_.get(), onCallbackUnlinked);
    mThread = std::thread(startThread, this);
}

void UdfpsHelper::touchDown() {
    std::scoped_lock lock(fpdown_mutex_lock);

    if (currentIsDownState) return;
    LOG(INFO) << "FP Touch Down!";
    currentIsDownState = true;
    CallListeners(true);
    mWaitCV.notify_all();
}
void UdfpsHelper::touchUp() {
    std::scoped_lock lock(fpdown_mutex_lock);

    if (!currentIsDownState) return;

    LOG(INFO) << "FP Touch Up!";
    currentIsDownState = false;
    CallListeners(false);
    mWaitCV.notify_all();
}

::ndk::ScopedAStatus UdfpsHelper::getTouchStatus(bool* aidl_return) {
    *aidl_return = currentIsDownState;

    return ndk::ScopedAStatus::ok();
}

void UdfpsHelper::CallListeners(bool isDown) {
    // Notify all callbacks
    std::unique_lock<decltype(callback_mutex_lock)> lock(callback_mutex_lock);

    // is_dead notifies a callback and return true if it is dead.
    for (const auto& [_, callback] : callbacks_) {
        auto res = callback->onUdfpsTouchStatusChanged(isDown);
        if (!res.isOk()) {
            LOG(ERROR) << "Cannot call callback:" << res.getDescription()
                       << ". Do nothing here if callback is dead as it will be cleaned up later.";
        }
    }
    lock.unlock();
}

void UdfpsHelper::run() {
    std::unique_lock<std::mutex> runLock(run_mutex_lock);

    while (true) {
        if (!currentIsDownState) {
            mWaitCV.wait(runLock, [this] {
                return this->currentIsDownState;
            });
        }
        else {
            mWaitCV.wait_for(runLock, std::chrono::seconds(3), [this] {
                return !this->currentIsDownState;
            });
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

::ndk::ScopedAStatus UdfpsHelper::registerCallback(const std::shared_ptr<::aidl::vendor::chen::aidl::syshelper::IUdfpsCallback>& callback) {
    if (callback == nullptr) {
        // For now, this shouldn't happen because argument is not nullable.
        return ndk::ScopedAStatus::fromExceptionCode(EX_NULL_POINTER);
    }

    {
        std::lock_guard<decltype(callback_mutex_lock)> lock(callback_mutex_lock);
        auto linked_callback_result = LinkedCallback::Make(ref<UdfpsHelper>(), callback);
        if (!linked_callback_result.ok()) {
            return ndk::ScopedAStatus::fromStatus(-linked_callback_result.error().code());
        }
        callbacks_[*linked_callback_result] = callback;
        // unlock
    }

    auto res = callback->onUdfpsTouchStatusChanged(currentIsDownState);
    if (!res.isOk()) {
        LOG(DEBUG) << "Cannot call onUdfpsTouchStatusChanged:" << res.getDescription()
                   << ". Do nothing here if callback is dead as it will be cleaned up later.";
    }

    return ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus UdfpsHelper::unregisterCallback(const std::shared_ptr<::aidl::vendor::chen::aidl::syshelper::IUdfpsCallback>& in_callback) {
    if (in_callback == nullptr) {
        // For now, this shouldn't happen because argument is not nullable.
        return ndk::ScopedAStatus::fromExceptionCode(EX_NULL_POINTER);
    }

    std::lock_guard<decltype(callback_mutex_lock)> lock(callback_mutex_lock);

    auto matches = [in_callback](const auto& cb) {
        return cb->asBinder() == in_callback->asBinder();  // compares binder object
    };
    bool removed = false;
    for (auto it = callbacks_.begin(); it != callbacks_.end();) {
        if (it->second->asBinder() == in_callback->asBinder()) {
            auto status = AIBinder_unlinkToDeath(in_callback->asBinder().get(), death_recipient_.get(),
                                                 reinterpret_cast<void*>(it->first));
            if (status != STATUS_OK && status != STATUS_DEAD_OBJECT) {
                LOG(WARNING) << __func__
                             << "Cannot unlink to death: " << ::android::statusToString(status);
            }
            it = callbacks_.erase(it);
            removed = true;
        } else {
            it++;
        }
    }
    LOG(INFO) << "UdfpsHelper: unregistered Callback!";

    return removed ? ndk::ScopedAStatus::ok()
                   : ndk::ScopedAStatus::fromExceptionCode(EX_ILLEGAL_ARGUMENT);
}

}  // namespace syshelper
}  // namespace aidl
}  // namespace chen
}  // namespace vendor
}  // namespace aidl