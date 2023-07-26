#include <android-base/logging.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>

#include "ALSHelper.h"
#include "UdfpsHelper.h"

using aidl::vendor::chen::aidl::syshelper::ALSHelper;
using aidl::vendor::chen::aidl::syshelper::UdfpsHelper;

int main() {
    ABinderProcess_setThreadPoolMaxThreadCount(10);
    std::shared_ptr<ALSHelper> alsHelper = ndk::SharedRefBase::make<ALSHelper>();
    std::shared_ptr<UdfpsHelper> udfpsHelper = ndk::SharedRefBase::make<UdfpsHelper>();

    std::string instance = std::string() + ALSHelper::descriptor + "/default";
    binder_status_t status =
        AServiceManager_addService(alsHelper->asBinder().get(), instance.c_str());
    CHECK_EQ(status, STATUS_OK);

    instance = std::string() + UdfpsHelper::descriptor + "/default";
    status = AServiceManager_addService(udfpsHelper->asBinder().get(), instance.c_str());
    CHECK_EQ(status, STATUS_OK);

    LOG(INFO) << "Chen SysHelper Registered!";

    ABinderProcess_joinThreadPool();
    return EXIT_FAILURE;  // Unreachable
}