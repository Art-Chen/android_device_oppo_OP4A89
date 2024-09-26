/*
 * Copyright (C) 2020 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Fingerprint.h"

#include <android-base/logging.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>

#include "UdfpsHelper.h"

using aidl::android::hardware::biometrics::fingerprint::Fingerprint;
using aidl::vendor::chen::aidl::syshelper::UdfpsHelper;

int main() {
    LOG(INFO) << "Fingerprint HAL started";
    ABinderProcess_setThreadPoolMaxThreadCount(5);
    std::shared_ptr<UdfpsHelper> udfpsHelper = ndk::SharedRefBase::make<UdfpsHelper>();
    std::shared_ptr<Fingerprint> hal = ndk::SharedRefBase::make<Fingerprint>(udfpsHelper);
    auto binder = hal->asBinder();

    if (hal->connected()) {
        const std::string instance = std::string(Fingerprint::descriptor) + "/default";
        binder_status_t status =
                AServiceManager_registerLazyService(binder.get(), instance.c_str());
        CHECK_EQ(status, STATUS_OK);
        AServiceManager_forceLazyServicesPersist(true);
    } else {
        LOG(FATAL) << "Fingerprint HAL is not connected";
    }

    const std::string instance = std::string(UdfpsHelper::descriptor) + "/default";
    binder_status_t status =
        AServiceManager_addService(udfpsHelper->asBinder().get(), instance.c_str());
    CHECK_EQ(status, STATUS_OK);

    LOG(INFO) << "Chen Udfps Helper Registered!";

    ABinderProcess_startThreadPool();
    ABinderProcess_joinThreadPool();
    return EXIT_FAILURE;  // should not reach
}
