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

#pragma once

#include <aidl/android/hardware/biometrics/fingerprint/BnFingerprint.h>
#include <aidl/vendor/chen/aidl/syshelper/IUdfpsHelper.h>

#include <android/hardware/biometrics/fingerprint/2.1/types.h>
#include <android/hardware/biometrics/fingerprint/2.2/IBiometricsFingerprintClientCallback.h>
#include <android/hardware/biometrics/fingerprint/2.3/IBiometricsFingerprint.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <vendor/oplus/hardware/biometrics/fingerprint/2.1/IBiometricsFingerprint.h>

#include <fstream>

#include "Session.h"
#include "thread/WorkerThread.h"
namespace aidl::android::hardware::biometrics::fingerprint {

using ::android::sp;
using ::android::base::GetProperty;
using ::android::hardware::hidl_string;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::biometrics::fingerprint::V2_1::FingerprintError;
using ::android::hardware::biometrics::fingerprint::V2_1::RequestStatus;
using ::android::hardware::biometrics::fingerprint::V2_2::FingerprintAcquiredInfo;
using ::android::hardware::biometrics::fingerprint::V2_2::IBiometricsFingerprintClientCallback;
using ::android::hardware::biometrics::fingerprint::V2_3::IBiometricsFingerprint;

using IOplusBiometricsFingerprint =
    ::vendor::oplus::hardware::biometrics::fingerprint::V2_1::IBiometricsFingerprint;

class Fingerprint : public BnFingerprint {
  public:
    Fingerprint();

    ndk::ScopedAStatus getSensorProps(std::vector<SensorProps>* out) override;

    ndk::ScopedAStatus createSession(int32_t sensorId, int32_t userId,
                                     const std::shared_ptr<ISessionCallback>& cb,
                                     std::shared_ptr<ISession>* out) override;
    bool connected() { return mOplusBiometricsFingerprint != nullptr; }

    static const char* type2String(FingerprintSensorType type);

    binder_status_t dump(int fd, const char** /*args*/, uint32_t numArgs);

  private:
    WorkerThread mWorker;
    LockoutTracker mLockoutTracker;
    std::shared_ptr<Session> mSession;
    FingerprintSensorType mSensorType;
    sp<IOplusBiometricsFingerprint> mOplusBiometricsFingerprint;
    std::shared_ptr<IUdfpsHelper> mChenUdfpsHelper;
};

}  // namespace aidl::android::hardware::biometrics::fingerprint
