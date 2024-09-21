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

#include <aidl/android/hardware/biometrics/fingerprint/BnSession.h>
#include <aidl/android/hardware/biometrics/fingerprint/ISessionCallback.h>
#include <aidl/vendor/chen/aidl/syshelper/IUdfpsHelper.h>

#include <android-base/properties.h>
#include <android-base/logging.h>

#include <android/hardware/biometrics/fingerprint/2.1/types.h>
#include <android/hardware/biometrics/fingerprint/2.2/IBiometricsFingerprintClientCallback.h>
#include <android/hardware/biometrics/fingerprint/2.3/IBiometricsFingerprint.h>

#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>

#include <vendor/oplus/hardware/biometrics/fingerprint/2.1/IBiometricsFingerprint.h>

#include "thread/WorkerThread.h"
#include "LockoutTracker.h"

#include <fstream>

#define FP_PRESS_PATH "/sys/kernel/oplus_display/notify_fppress"
#define DIMLAYER_PATH "/sys/kernel/oplus_display/dimlayer_hbm"

namespace aidl::android::hardware::biometrics::fingerprint {

namespace common = aidl::android::hardware::biometrics::common;
namespace keymaster = aidl::android::hardware::keymaster;

using ::android::sp;
using ::android::base::GetProperty;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::hardware::biometrics::fingerprint::V2_1::FingerprintError;
using ::android::hardware::biometrics::fingerprint::V2_1::RequestStatus;
using ::android::hardware::biometrics::fingerprint::V2_2::FingerprintAcquiredInfo;
using ::android::hardware::biometrics::fingerprint::V2_2::IBiometricsFingerprintClientCallback;
using ::android::hardware::biometrics::fingerprint::V2_3::IBiometricsFingerprint;

using namespace ::android::hardware::biometrics::fingerprint;

using aidl::vendor::chen::aidl::syshelper::IUdfpsHelper;

using IOplusBiometricsFingerprint =
    ::vendor::oplus::hardware::biometrics::fingerprint::V2_1::IBiometricsFingerprint;
using ::vendor::oplus::hardware::biometrics::fingerprint::V2_1::
    IBiometricsFingerprintClientCallbackEx;

enum class SessionState {
    IDLING,
    CLOSED,
    GENERATING_CHALLENGE,
    REVOKING_CHALLENGE,
    ENROLLING,
    AUTHENTICATING,
    DETECTING_INTERACTION,
    ENUMERATING_ENROLLMENTS,
    REMOVING_ENROLLMENTS,
    GETTING_AUTHENTICATOR_ID,
    INVALIDATING_AUTHENTICATOR_ID,
    RESETTING_LOCKOUT,
};

namespace {
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
    LOG(INFO) << "setFpPress " << value;
    set(FP_PRESS_PATH, value);
    return isUdfps();
}

typedef enum fingerprint_callback_cmd_Id {
    FINGERPRINT_CALLBACK_CMD_ID_NONE = 0,
    FINGERPRINT_CALLBACK_CMD_ID_BASE = 1200,
    FINGERPRINT_CALLBACK_CMD_ID_ON_TOUCH_DOWN = 1201,
    FINGERPRINT_CALLBACK_CMD_ID_ON_TOUCH_UP = 1202,
} fingerprint_callback_cmd_Id_t;
}

void onClientDeath(void* cookie);

class FingerprintCallback;
class Session : public BnSession {
   public:
    Session(int sensorId, int userId, std::shared_ptr<ISessionCallback> cb,
            IOplusBiometricsFingerprint* oplusFp, std::shared_ptr<IUdfpsHelper> helper,
            LockoutTracker lockoutTracker, WorkerThread* worker);

    ndk::ScopedAStatus generateChallenge() override;

    ndk::ScopedAStatus revokeChallenge(int64_t challenge) override;

    ndk::ScopedAStatus enroll(const keymaster::HardwareAuthToken& hat,
                              std::shared_ptr<common::ICancellationSignal>* out) override;

    ndk::ScopedAStatus authenticate(int64_t operationId,
                                    std::shared_ptr<common::ICancellationSignal>* out) override;

    ndk::ScopedAStatus detectInteraction(
        std::shared_ptr<common::ICancellationSignal>* out) override;

    ndk::ScopedAStatus enumerateEnrollments() override;

    ndk::ScopedAStatus removeEnrollments(const std::vector<int32_t>& enrollmentIds) override;

    ndk::ScopedAStatus getAuthenticatorId() override;

    ndk::ScopedAStatus invalidateAuthenticatorId() override;

    ndk::ScopedAStatus resetLockout(const keymaster::HardwareAuthToken& hat) override;

    ndk::ScopedAStatus close() override;

    ndk::ScopedAStatus onPointerDown(int32_t pointerId, int32_t x, int32_t y, float minor,
                                     float major) override;

    ndk::ScopedAStatus onPointerUp(int32_t pointerId) override;

    ndk::ScopedAStatus onUiReady() override;

    ndk::ScopedAStatus authenticateWithContext(
        int64_t operationId, const common::OperationContext& context,
        std::shared_ptr<common::ICancellationSignal>* out) override;

    ndk::ScopedAStatus enrollWithContext(
        const keymaster::HardwareAuthToken& hat, const common::OperationContext& context,
        std::shared_ptr<common::ICancellationSignal>* out) override;

    ndk::ScopedAStatus detectInteractionWithContext(
        const common::OperationContext& context,
        std::shared_ptr<common::ICancellationSignal>* out) override;

    ndk::ScopedAStatus onPointerDownWithContext(const PointerContext& context) override;

    ndk::ScopedAStatus onPointerUpWithContext(const PointerContext& context) override;

    ndk::ScopedAStatus onContextChanged(const common::OperationContext& context) override;

    ndk::ScopedAStatus onPointerCancelWithContext(const PointerContext& context) override;

    ndk::ScopedAStatus setIgnoreDisplayTouches(bool shouldIgnore) override;

    ndk::ScopedAStatus cancel();

    binder_status_t linkToDeath(AIBinder* binder);

    bool isClosed();

   private:
    friend FingerprintCallback;
    // Crashes the HAL if it's not currently idling because that would be an invalid state machine
    // transition. Otherwise, sets the scheduled state to the given state.
    void scheduleStateOrCrash(SessionState state);

    // Crashes the HAL if the provided state doesn't match the previously scheduled state.
    // Otherwise, transitions into the provided state, clears the scheduled state, and notifies
    // the client about the transition by calling ISessionCallback#onStateChanged.
    void enterStateOrCrash(SessionState state);

    // Sets the current state to SessionState::IDLING and notifies the client about the transition
    // by calling ISessionCallback#onStateChanged.
    void enterIdling();

    bool checkSensorLockout();
    void clearLockout(bool clearAttemptCounter);
    void startLockoutTimer(int64_t timeout);
    void lockoutTimerExpired();

    // lockout timer
    bool mIsLockoutTimerStarted = false;
    bool mIsLockoutTimerAborted = false;

    // The sensor and user IDs for which this session was created.
    int32_t mSensorId;
    int32_t mUserId;

    // Callback for talking to the framework. This callback must only be called from non-binder
    // threads to prevent nested binder calls and consequently a binder thread exhaustion.
    // Practically, it means that this callback should always be called from the worker thread.
    std::shared_ptr<ISessionCallback> mCb;

    // Vendor HIDL Service
    IOplusBiometricsFingerprint* mOplusFp;

    sp<FingerprintCallback> mOplusFpCallback;

    std::shared_ptr<IUdfpsHelper> mHelper;

    // Worker thread that allows to schedule tasks for asynchronous execution.
    WorkerThread* mWorker;

    // Simple representation of the session's state machine. These are atomic because they can be
    // modified from both the main and the worker threads.
    std::atomic<SessionState> mScheduledState;
    std::atomic<SessionState> mCurrentState;

    // Binder death handler.
    AIBinder_DeathRecipient* mDeathRecipient;

    LockoutTracker mLockoutTracker;

    sp<IOplusBiometricsFingerprint> mOplusBiometricsFingerprint;
    std::shared_ptr<IUdfpsHelper> mChenUdfpsHelper;
    sp<V2_1::IBiometricsFingerprintClientCallback> mClientCallback;
};

class FingerprintCallback : public IBiometricsFingerprintClientCallback,
                            public IBiometricsFingerprintClientCallbackEx {
   public:
    FingerprintCallback(std::shared_ptr<ISessionCallback> cb,
                        std::shared_ptr<IUdfpsHelper> helper,
                        Session* session,
                        LockoutTracker& tracker) 
                        : mCb(cb), 
                        mHelper(helper),  
                        mLockoutTracker(tracker),
                        mSession(session) {};
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
    std::shared_ptr<ISessionCallback> mCb;
    std::shared_ptr<IUdfpsHelper> mHelper;
    LockoutTracker& mLockoutTracker;
    Session* mSession;
};

}  // namespace aidl::android::hardware::biometrics::fingerprint
