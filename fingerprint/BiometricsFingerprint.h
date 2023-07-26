/*
 * Copyright (C) 2022 The LineageOS Project
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

#include <aidl/vendor/chen/aidl/syshelper/IUdfpsHelper.h>

#include <android-base/properties.h>
#include <android/hardware/biometrics/fingerprint/2.1/types.h>
#include <android/hardware/biometrics/fingerprint/2.2/IBiometricsFingerprintClientCallback.h>
#include <android/hardware/biometrics/fingerprint/2.3/IBiometricsFingerprint.h>
#include <android/log.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <log/log.h>

#include <vendor/oplus/hardware/biometrics/fingerprint/2.1/IBiometricsFingerprint.h>

#include <fstream>

namespace android {
namespace hardware {
namespace biometrics {
namespace fingerprint {
namespace V2_3 {
namespace implementation {

#define FP_PRESS_PATH "/sys/kernel/oplus_display/notify_fppress"
#define DIMLAYER_PATH "/sys/kernel/oplus_display/dimlayer_hbm"

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

using aidl::vendor::chen::aidl::syshelper::IUdfpsHelper;

using IOplusBiometricsFingerprint =
    vendor::oplus::hardware::biometrics::fingerprint::V2_1::IBiometricsFingerprint;
using ::vendor::oplus::hardware::biometrics::fingerprint::V2_1::
        IBiometricsFingerprintClientCallbackEx;

class BiometricsFingerprint : public IBiometricsFingerprint,
                              public IBiometricsFingerprintClientCallback,
                              public IBiometricsFingerprintClientCallbackEx {
  public:
    BiometricsFingerprint();

    // Methods from ::android::hardware::biometrics::fingerprint::V2_1::IBiometricsFingerprint
    // follow.
    Return<uint64_t> setNotify(
            const sp<V2_1::IBiometricsFingerprintClientCallback>& clientCallback) override;
    Return<uint64_t> preEnroll() override;
    Return<RequestStatus> enroll(const hidl_array<uint8_t, 69>& hat, uint32_t gid,
                                 uint32_t timeoutSec) override;
    Return<RequestStatus> postEnroll() override;
    Return<uint64_t> getAuthenticatorId() override;
    Return<RequestStatus> cancel() override;
    Return<RequestStatus> enumerate() override;
    Return<RequestStatus> remove(uint32_t gid, uint32_t fid) override;
    Return<RequestStatus> setActiveGroup(uint32_t gid, const hidl_string& storePath) override;
    Return<RequestStatus> authenticate(uint64_t operationId, uint32_t gid) override;

    // Methods from ::android::hardware::biometrics::fingerprint::V2_3::IBiometricsFingerprint
    // follow.
    Return<bool> isUdfps(uint32_t sensorID) override;
    Return<void> onFingerDown(uint32_t x, uint32_t y, float minor, float major) override;
    Return<void> onFingerUp() override;

    // Methods from
    // ::android::hardware::biometrics::fingerprint::V2_1::IBiometricsFingerprintClientCallback
    // follow.
    Return<void> onEnrollResult(uint64_t deviceId, uint32_t fingerId, uint32_t groupId,
                                uint32_t remaining) override;
    Return<void> onAcquired(uint64_t deviceId, V2_1::FingerprintAcquiredInfo acquiredInfo,
                            int32_t vendorCode) override;
    Return<void> onAuthenticated(uint64_t deviceId, uint32_t fingerId, uint32_t groupId,
                                 const hidl_vec<uint8_t>& token) override;
    Return<void> onError(uint64_t deviceId, FingerprintError error, int32_t vendorCode) override;
    Return<void> onRemoved(uint64_t deviceId, uint32_t fingerId, uint32_t groupId,
                           uint32_t remaining) override;
    Return<void> onEnumerate(uint64_t deviceId, uint32_t fingerId, uint32_t groupId,
                             uint32_t remaining) override;

    // Methods from
    // ::android::hardware::biometrics::fingerprint::V2_2::IBiometricsFingerprintClientCallback
    // follow.
    Return<void> onAcquired_2_2(uint64_t deviceId, FingerprintAcquiredInfo acquiredInfo,
                                int32_t vendorCode) override;

    // Methods from
    // ::vendor::oplus::hardware::biometrics::fingerprint::V2_1::IBiometricsFingerprintClientCallbackEx
    // follow.
    Return<void> onEngineeringInfoUpdated(uint32_t lenth, const hidl_vec<uint32_t>& keys,
                                          const hidl_vec<hidl_string>& values) override;
    Return<void> onFingerprintCmd(int32_t cmdId, const hidl_vec<int8_t>& result,
                                  uint32_t resultLen) override;

  private:
    /*
    * Write value to path and close file.
    */
    template <typename T>
    void set(const std::string& path, const T& value) {
        std::ofstream file(path);
        file << value;
    }

    template <typename T>
    T get(const std::string& path, const T& def) {
        std::ifstream file(path);
        T result;

        file >> result;
        return file.fail() ? def : result;
    }

    bool isUdfps() {
        // We need to rely on `persist.vendor.fingerprint.sensor_type` here because we can't get our
        // sensorId from anywhere.
        return GetProperty("persist.vendor.fingerprint.sensor_type", "") == "optical";
    }

    bool setDimlayerHbm(unsigned int value) {
        set(DIMLAYER_PATH, value);
        return isUdfps() && get(DIMLAYER_PATH, 0) == value;
    }

    bool setFpPress(unsigned int value) {
        set(FP_PRESS_PATH, value);
        return isUdfps();
    }

    sp<IOplusBiometricsFingerprint> mOplusBiometricsFingerprint;
    std::shared_ptr<IUdfpsHelper> mChenUdfpsHelper;
    sp<V2_1::IBiometricsFingerprintClientCallback> mClientCallback;

    typedef enum fingerprint_callback_cmd_Id {
        FINGERPRINT_CALLBACK_CMD_ID_NONE = 0,
        FINGERPRINT_CALLBACK_CMD_ID_BASE = 1200,
        FINGERPRINT_CALLBACK_CMD_ID_ON_TOUCH_DOWN = 1201,
        FINGERPRINT_CALLBACK_CMD_ID_ON_TOUCH_UP = 1202,
    } fingerprint_callback_cmd_Id_t;
};

}  // namespace implementation
}  // namespace V2_3
}  // namespace fingerprint
}  // namespace biometrics
}  // namespace hardware
}  // namespace android
