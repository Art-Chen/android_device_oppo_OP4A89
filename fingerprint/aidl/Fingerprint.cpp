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

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/properties.h>
#include <android-base/stringprintf.h>

#include <android/binder_manager.h>
#include <android/binder_process.h>

#include "Session.h"

namespace aidl::android::hardware::biometrics::fingerprint {
namespace {
constexpr size_t MAX_WORKER_QUEUE_SIZE = 5;
constexpr int SENSOR_ID = 5;
constexpr common::SensorStrength SENSOR_STRENGTH = common::SensorStrength::STRONG;
constexpr int MAX_ENROLLMENTS_PER_USER = 5;
constexpr bool SUPPORTS_NAVIGATION_GESTURES = false;
constexpr char HW_COMPONENT_ID[] = "fingerprintSensor";
constexpr char HW_VERSION[] = "Goodix/G/1";
constexpr char FW_VERSION[] = "1.01";
constexpr char SERIAL_NUMBER[] = "00000001";
constexpr char SW_COMPONENT_ID[] = "matchingAlgorithm";
constexpr char SW_VERSION[] = "Art_Chen/1/0";

}  // namespace

Fingerprint::Fingerprint() : mWorker(MAX_WORKER_QUEUE_SIZE) {
    mSensorType = FingerprintSensorType::UNDER_DISPLAY_OPTICAL;
    mOplusBiometricsFingerprint = IOplusBiometricsFingerprint::getService();
}

ndk::ScopedAStatus Fingerprint::getSensorProps(std::vector<SensorProps>* out) {
    std::vector<common::ComponentInfo> componentInfo = {
            {HW_COMPONENT_ID, HW_VERSION, FW_VERSION, SERIAL_NUMBER, "" /* softwareVersion */},
            {SW_COMPONENT_ID, "" /* hardwareVersion */, "" /* firmwareVersion */,
             "" /* serialNumber */, SW_VERSION}};
    common::CommonProps commonProps = {SENSOR_ID, SENSOR_STRENGTH,
                                       MAX_ENROLLMENTS_PER_USER, componentInfo};

    SensorLocation sensorLocation;

    sensorLocation.sensorLocationX = 540;
    sensorLocation.sensorLocationY = 2156;
    sensorLocation.sensorRadius = 95;

    LOG(INFO) << "sensor type:" << ::android::internal::ToString(mSensorType)
              << " location:" << sensorLocation.toString();

    *out = {{commonProps,
             mSensorType,
             {sensorLocation},
             false, /* navigationGuesture */
             false, /* detectInteraction*/
             true, /* displayTouch*/
             false, /* controlIllumination*/
             std::nullopt}};
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Fingerprint::createSession(int32_t sensorId, int32_t userId,
                                              const std::shared_ptr<ISessionCallback>& cb,
                                              std::shared_ptr<ISession>* out) {
    CHECK(mSession == nullptr || mSession->isClosed()) << "Open session already exists!";

    std::string instanceName = std::string() + IUdfpsHelper::descriptor + "/default";
    if (AServiceManager_isDeclared(instanceName.c_str())) {
        mChenUdfpsHelper = IUdfpsHelper::fromBinder(ndk::SpAIBinder(AServiceManager_waitForService(instanceName.c_str())));
    }
    mSession = SharedRefBase::make<Session>(sensorId, userId, cb, mOplusBiometricsFingerprint.get(), mChenUdfpsHelper
                                            , mLockoutTracker, &mWorker);
    *out = mSession;

    mSession->linkToDeath(cb->asBinder().get());

    LOG(INFO) << __func__ << ": sensorId:" << sensorId << " userId:" << userId;
    return ndk::ScopedAStatus::ok();
}

binder_status_t Fingerprint::dump(int fd, const char** /*args*/, uint32_t numArgs) {
    if (fd < 0) {
        LOG(ERROR) << __func__ << "fd invalid: " << fd;
        return STATUS_BAD_VALUE;
    } else {
        LOG(INFO) << __func__ << " fd:" << fd << "numArgs:" << numArgs;
    }

    dprintf(fd, "----- FingerprintHal::dump -----\n");
    std::vector<SensorProps> sps(1);
    getSensorProps(&sps);
    for (auto& sp : sps) {
        ::android::base::WriteStringToFd(sp.toString(), fd);
    }

    fsync(fd);
    return STATUS_OK;
}

const char* Fingerprint::type2String(FingerprintSensorType type) {
    switch (type) {
        case FingerprintSensorType::REAR:
            return "rear";
        case FingerprintSensorType::POWER_BUTTON:
            return "side";
        case FingerprintSensorType::UNDER_DISPLAY_OPTICAL:
            return "udfps";
        case FingerprintSensorType::UNDER_DISPLAY_ULTRASONIC:
            return "udfps";
        default:
            return "unknown";
    }
}

}  // namespace aidl::android::hardware::biometrics::fingerprint
