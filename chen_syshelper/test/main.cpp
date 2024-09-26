#include <android-base/logging.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>

#include <signal.h>
#include <unistd.h>
#include <iostream>

#include <aidl/vendor/chen/aidl/syshelper/IUdfpsHelper.h>
#include <aidl/vendor/chen/aidl/syshelper/IUdfpsCallback.h>
#include <aidl/vendor/chen/aidl/syshelper/BnUdfpsCallback.h>

using aidl::vendor::chen::aidl::syshelper::IUdfpsHelper;
using aidl::vendor::chen::aidl::syshelper::IUdfpsCallback;

class ChenUdfpsHelperCallback : public ::aidl::vendor::chen::aidl::syshelper::BnUdfpsCallback {
   public:
    ChenUdfpsHelperCallback() {};

    ::ndk::ScopedAStatus onUdfpsTouchStatusChanged(bool in_isDown) override {
        LOG(INFO) << "onUdfpsTouchStatusChanged " << in_isDown;
        return ::ndk::ScopedAStatus::fromExceptionCode(EX_NULL_POINTER);
    }
};

int main() {
    ABinderProcess_setThreadPoolMaxThreadCount(5);
    std::string instanceName = std::string() + IUdfpsHelper::descriptor + "/default";
    bool isSupportChenSysHelper = AServiceManager_isDeclared(instanceName.c_str());
    if (!isSupportChenSysHelper) {
        LOG(FATAL) << "Chen System Helper is NOT Declared!!";
    }
    std::shared_ptr<IUdfpsHelper> udfpsHelper = IUdfpsHelper::fromBinder(ndk::SpAIBinder(AServiceManager_waitForService(instanceName.c_str())));
    const std::shared_ptr<IUdfpsCallback> callback = ndk::SharedRefBase::make<ChenUdfpsHelperCallback>();
    auto ret = udfpsHelper->registerCallback(callback);
    LOG(INFO) << ret.getDescription();

    ABinderProcess_startThreadPool();
    ABinderProcess_joinThreadPool();
    return 0;
}