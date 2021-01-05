/*
 * Copyright (C) 2019 The LineageOS Project
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

#define LOG_TAG "FingerprintInscreenService"

#include "FingerprintInscreen.h"
#include <hidl/HidlTransportSupport.h>
#include <android-base/logging.h>
#include <fstream>
#include <cmath>
#include <thread>

#define FINGERPRINT_ACQUIRED_VENDOR 6

#define FP_PRESS_PATH "/sys/kernel/oppo_display/notify_fppress"
#define DIMLAYER_PATH "/sys/kernel/oppo_display/dimlayer_hbm"
#define AOD_MODE_PATH "/sys/kernel/oppo_display/aod_light_mode_set"
#define NOTIFY_BLANK_PATH "/sys/kernel/oppo_display/notify_panel_blank"
#define POWER_STATUS "/sys/kernel/oppo_display/power_status"
#define DC_DIM_PATH "/sys/kernel/oppo_display/dimlayer_bl_en"

namespace vendor {
namespace lineage {
namespace biometrics {
namespace fingerprint {
namespace inscreen {
namespace V1_0 {
namespace implementation {

bool dcDimState;

/*
 * Write value to path and close file.
 */
template <typename T>
static void set(const std::string& path, const T& value) {
    std::ofstream file(path);
    file << value;
}

template <typename T>
static T get(const std::string& path, const T& def) {
    std::ifstream file(path);
    T result;

    file >> result;
    return file.fail() ? def : result;
}

FingerprintInscreen::FingerprintInscreen() {
    this->mFodCircleVisible = false;
	this->mFingerPressed = false;
    this->mVendorFpService = IBiometricsFingerprint::getService();
}

Return<int32_t> FingerprintInscreen::getPositionX() {
    return 445;
}

Return<int32_t> FingerprintInscreen::getPositionY() {
    return 2061;
}

Return<int32_t> FingerprintInscreen::getSize() {
    return 190;
}

Return<void> FingerprintInscreen::onStartEnroll() {
    return Void();
}

Return<void> FingerprintInscreen::onFinishEnroll() {
    return Void();
}

Return<void> FingerprintInscreen::onPress() {
    mFingerPressed = true;
    if (mFingerPressed) {
        set(FP_PRESS_PATH, 1);
    }
    return Void();
}

Return<void> FingerprintInscreen::onRelease() {
    
    mFingerPressed = false;
    set(FP_PRESS_PATH, 0);
     
    return Void();
}

Return<void> FingerprintInscreen::onShowFODView() {
    if (!mFodCircleVisible) {
        dcDimState = get(DC_DIM_PATH, 0);
        set(DC_DIM_PATH, 0);
    }
    if(get(POWER_STATUS, 3) || get(POWER_STATUS, 4)) {
	    set(NOTIFY_BLANK_PATH, 1);
        set(AOD_MODE_PATH, 1);
	}
	this->mVendorFpService->setScreenState(::vendor::oppo::hardware::biometrics::fingerprint::V2_1::FingerprintScreenState::FINGERPRINT_SCREEN_ON);
    set(DIMLAYER_PATH, 1);
    return Void();
}

Return<void> FingerprintInscreen::onHideFODView() {
    if (mFodCircleVisible) {
        set(DC_DIM_PATH, dcDimState);
    }
    this->mFodCircleVisible = false;
	this->mVendorFpService->setScreenState(::vendor::oppo::hardware::biometrics::fingerprint::V2_1::FingerprintScreenState::FINGERPRINT_SCREEN_ON);
    set(FP_PRESS_PATH, 0);
    set(DIMLAYER_PATH, 0);

    return Void();
}

Return<bool> FingerprintInscreen::handleAcquired(int32_t acquiredInfo, int32_t vendorCode) {
    std::lock_guard<std::mutex> _lock(mCallbackLock);
    if (mCallback == nullptr) {
        return false;
    }

    if (acquiredInfo == FINGERPRINT_ACQUIRED_VENDOR) {
        if (mFodCircleVisible && vendorCode == 0) {
            Return<void> ret = mCallback->onFingerDown();
            if (!ret.isOk()) {
                LOG(ERROR) << "FingerDown() error: " << ret.description();
            }
            return true;
        }

        if (mFodCircleVisible && vendorCode == 1) {
            Return<void> ret = mCallback->onFingerUp();
            if (!ret.isOk()) {
                LOG(ERROR) << "FingerUp() error: " << ret.description();
            }
            return true;
        }
    }

    return false;
}

Return<bool> FingerprintInscreen::handleError(int32_t error, int32_t vendorCode) {
    LOG(ERROR) << "error: " << error << ", vendorCode: " << vendorCode << "\n";
    return false;
}

Return<void> FingerprintInscreen::setLongPressEnabled(bool) {
    return Void();
}

Return<int32_t> FingerprintInscreen::getDimAmount(int32_t) {
    return 0;
}

Return<bool> FingerprintInscreen::shouldBoostBrightness() {
    return false;
}

Return<void> FingerprintInscreen::setCallback(const sp<IFingerprintInscreenCallback>& callback) {
    {
        std::lock_guard<std::mutex> _lock(mCallbackLock);
        mCallback = callback;
    }

    return Void();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace inscreen
}  // namespace fingerprint
}  // namespace biometrics
}  // namespace lineage
}  // namespace vendor
