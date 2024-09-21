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

#include "Session.h"

#include <Legacy2Aidl.h>
#include <android-base/logging.h>
#include <android-base/stringprintf.h>
#include <hardware/fingerprint.h>

#include "CancellationSignal.h"

#undef LOG_TAG
#define LOG_TAG "FingerprintWrapperSession"

#define ENABLE 1
#define DISABLE 0

namespace aidl::android::hardware::biometrics::fingerprint {

using LockoutMode = LockoutTracker::LockoutMode;

void onClientDeath(void* cookie) {
    LOG(INFO) << "FingerprintService has died";
    Session* session = static_cast<Session*>(cookie);
    if (session && !session->isClosed()) {
        session->close();
    }
}

AcquiredInfo HidlToAidlAcquiredInfo(::android::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo info) {
    switch (info) {
        case android::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo::ACQUIRED_GOOD:
            return AcquiredInfo::GOOD;
        case android::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo::ACQUIRED_PARTIAL:
            return AcquiredInfo::PARTIAL;
        case android::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo::ACQUIRED_INSUFFICIENT:
            return AcquiredInfo::INSUFFICIENT;
        case android::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo::ACQUIRED_IMAGER_DIRTY:
            return AcquiredInfo::SENSOR_DIRTY;
        case android::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo::ACQUIRED_TOO_SLOW:
            return AcquiredInfo::TOO_SLOW;
        case android::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo::ACQUIRED_TOO_FAST:
            return AcquiredInfo::TOO_FAST;
        case android::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo::ACQUIRED_VENDOR:
            return AcquiredInfo::VENDOR;
        default:
            return AcquiredInfo::INSUFFICIENT;
    }
}

Error HidlToAidlFingerprintError(::android::hardware::biometrics::fingerprint::V2_1::FingerprintError error, int32_t* vendorCode) {
    *vendorCode = -1;

    switch (error) {
        case android::hardware::biometrics::fingerprint::V2_1::FingerprintError::ERROR_HW_UNAVAILABLE:
            return Error::HW_UNAVAILABLE;
        case android::hardware::biometrics::fingerprint::V2_1::FingerprintError::ERROR_UNABLE_TO_PROCESS:
            return Error::UNABLE_TO_PROCESS;
        case android::hardware::biometrics::fingerprint::V2_1::FingerprintError::ERROR_TIMEOUT:
            return Error::TIMEOUT;
        case android::hardware::biometrics::fingerprint::V2_1::FingerprintError::ERROR_NO_SPACE:
            return Error::NO_SPACE;
        case android::hardware::biometrics::fingerprint::V2_1::FingerprintError::ERROR_CANCELED:
            return Error::CANCELED;
        case android::hardware::biometrics::fingerprint::V2_1::FingerprintError::ERROR_UNABLE_TO_REMOVE:
            return Error::UNABLE_TO_REMOVE;
        case android::hardware::biometrics::fingerprint::V2_1::FingerprintError::ERROR_LOCKOUT:
            *vendorCode = FINGERPRINT_ERROR_LOCKOUT;
            return Error::VENDOR;
        case android::hardware::biometrics::fingerprint::V2_1::FingerprintError::ERROR_VENDOR:
            return Error::VENDOR;
        default:
            return Error::UNABLE_TO_PROCESS;
    }
}

Session::Session(int sensorId, int userId, std::shared_ptr<ISessionCallback> cb,
                 IOplusBiometricsFingerprint* oplusFp, std::shared_ptr<IUdfpsHelper> helper,
                 LockoutTracker lockoutTracker, WorkerThread* worker)
    : mSensorId(sensorId),
      mUserId(userId),
      mCb(std::move(cb)),
      mOplusFp(oplusFp),
      mHelper(helper),
      mWorker(worker),
      mScheduledState(SessionState::IDLING),
      mCurrentState(SessionState::IDLING),
      mLockoutTracker(lockoutTracker) {
    CHECK_GE(mSensorId, 0);
    CHECK_GE(mUserId, 0);
    CHECK(oplusFp);
    CHECK(mWorker);
    CHECK(mCb);

    mDeathRecipient = AIBinder_DeathRecipient_new(onClientDeath);

    mOplusFpCallback = new FingerprintCallback(mCb, mHelper, this, mLockoutTracker);

    mOplusFp->setHalCallback(mOplusFpCallback);
    auto deviceId = mOplusFp->setNotify(mOplusFpCallback);
    if (deviceId == 0) {
        LOG(FATAL) << "notify hal failed!!";
    }
    std::string path = ::android::base::StringPrintf("/data/vendor_de/%d/fpdata/", mUserId);

    mOplusFp->setActiveGroup(mUserId, hidl_string(path));
}

binder_status_t Session::linkToDeath(AIBinder* binder) {
    return AIBinder_linkToDeath(binder, mDeathRecipient, this);
}

void Session::scheduleStateOrCrash(SessionState state) {
    // TODO(b/166800618): call enterIdling from the terminal callbacks and restore these checks.
    // CHECK(mScheduledState == SessionState::IDLING);
    // CHECK(mCurrentState == SessionState::IDLING);
    mScheduledState = state;
}

void Session::enterStateOrCrash(SessionState state) {
    CHECK(mScheduledState == state);
    mCurrentState = state;
    mScheduledState = SessionState::IDLING;
}

void Session::enterIdling() {
    // TODO(b/166800618): call enterIdling from the terminal callbacks and rethink this conditional.
    if (mCurrentState != SessionState::CLOSED) {
        mCurrentState = SessionState::IDLING;
    }
}

bool Session::isClosed() {
    return mCurrentState == SessionState::CLOSED;
}

bool Session::checkSensorLockout() {
    LockoutMode lockoutMode = mLockoutTracker.getMode();
    if (lockoutMode == LockoutMode::kPermanent) {
        LOG(ERROR) << "Fail: lockout permanent";
        mCb->onLockoutPermanent();
        mIsLockoutTimerAborted = true;
        return true;
    } else if (lockoutMode == LockoutMode::kTimed) {
        int64_t timeLeft = mLockoutTracker.getLockoutTimeLeft();
        LOG(ERROR) << "Fail: lockout timed: " << timeLeft;
        mCb->onLockoutTimed(timeLeft);
        if (!mIsLockoutTimerStarted) startLockoutTimer(timeLeft);
        return true;
    }
    return false;
}

void Session::clearLockout(bool clearAttemptCounter) {
    mLockoutTracker.reset(clearAttemptCounter);
    mCb->onLockoutCleared();
}

void Session::startLockoutTimer(int64_t timeout) {
    mIsLockoutTimerAborted = false;
    std::function<void()> action =
        std::bind(&Session::lockoutTimerExpired, this);
    std::thread([timeout, action]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(timeout));
        action();
    }).detach();

    mIsLockoutTimerStarted = true;
}

void Session::lockoutTimerExpired() {
    if (!mIsLockoutTimerAborted)
        clearLockout(false);

    mIsLockoutTimerStarted = false;
    mIsLockoutTimerAborted = false;
}

ndk::ScopedAStatus Session::generateChallenge() {
    scheduleStateOrCrash(SessionState::GENERATING_CHALLENGE);

    mWorker->schedule(Callable::from([this] {
        enterStateOrCrash(SessionState::GENERATING_CHALLENGE);
        uint64_t challenge = mOplusFp->preEnroll();
        LOG(INFO) << "generateChallenge: " << challenge;
        mCb->onChallengeGenerated(challenge);
        
        enterIdling();
    }));

    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Session::revokeChallenge(int64_t challenge) {
    LOG(INFO) << "revokeChallenge";
    scheduleStateOrCrash(SessionState::REVOKING_CHALLENGE);

    mWorker->schedule(Callable::from([this, challenge] {
        enterStateOrCrash(SessionState::REVOKING_CHALLENGE);
        setFpPress(DISABLE);
        mOplusFp->postEnroll();
        setDimlayerHbm(DISABLE);
        mCb->onChallengeRevoked(challenge);
        enterIdling();
    }));

    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Session::enroll(const keymaster::HardwareAuthToken& hat,
                                   std::shared_ptr<common::ICancellationSignal>* out) {
    LOG(INFO) << "enroll";
    scheduleStateOrCrash(SessionState::ENROLLING);

    hidl_array<uint8_t, 69> hat_h;
    hw_auth_token_t authToken;
    translate(hat, authToken);
    translate(authToken, hat_h);
    mWorker->schedule(Callable::from([this, hat, hat_h] {
        enterStateOrCrash(SessionState::ENROLLING);
        setDimlayerHbm(ENABLE);
        auto ret = mOplusFp->enroll(hat_h, mUserId, 60);
        if (ret != RequestStatus::SYS_OK) {
            mCb->onError(Error::UNABLE_TO_PROCESS, RequestStatusToInt(ret));
        }
        enterIdling();
    }));

    *out = SharedRefBase::make<CancellationSignal>(this);
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Session::authenticate(int64_t operationId,
                                         std::shared_ptr<common::ICancellationSignal>* out) {
    LOG(INFO) << "authenticate";
    scheduleStateOrCrash(SessionState::AUTHENTICATING);

    mWorker->schedule(Callable::from([this, operationId] {
        enterStateOrCrash(SessionState::AUTHENTICATING);
        auto ret = mOplusFp->authenticate(operationId, mUserId);
        if (ret != RequestStatus::SYS_OK) {
            mCb->onError(Error::UNABLE_TO_PROCESS, RequestStatusToInt(ret));
        }
        enterIdling();
    }));

    *out = SharedRefBase::make<CancellationSignal>(this);
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Session::detectInteraction(std::shared_ptr<common::ICancellationSignal>* out) {
    LOG(INFO) << "detectInteraction";
    scheduleStateOrCrash(SessionState::DETECTING_INTERACTION);

    mWorker->schedule(Callable::from([this] {
        enterStateOrCrash(SessionState::DETECTING_INTERACTION);
        // not supported
        mCb->onError(Error::UNABLE_TO_PROCESS, 0);
        enterIdling();
    }));

    *out = SharedRefBase::make<CancellationSignal>(this);
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Session::enumerateEnrollments() {
    LOG(INFO) << "enumerateEnrollments";
    scheduleStateOrCrash(SessionState::ENUMERATING_ENROLLMENTS);

    mWorker->schedule(Callable::from([this] {
        enterStateOrCrash(SessionState::ENUMERATING_ENROLLMENTS);
        auto ret = mOplusFp->enumerate();
        if (ret != RequestStatus::SYS_OK) {
            mCb->onError(Error::UNABLE_TO_PROCESS, RequestStatusToInt(ret));
        }
        enterIdling();
    }));

    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Session::removeEnrollments(const std::vector<int32_t>& enrollmentIds) {
    LOG(INFO) << "removeEnrollments, size:" << enrollmentIds.size();
    scheduleStateOrCrash(SessionState::REMOVING_ENROLLMENTS);

    mWorker->schedule(Callable::from([this, enrollmentIds] {
        enterStateOrCrash(SessionState::REMOVING_ENROLLMENTS);
        for (int32_t fid : enrollmentIds) {
            auto ret = mOplusFp->remove(mUserId, fid);
            if (ret != RequestStatus::SYS_OK) {
                LOG(ERROR) << "Remove failed!! " << RequestStatusToInt(ret);
            }
        }
        enterIdling();
    }));

    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Session::getAuthenticatorId() {
    LOG(INFO) << "getAuthenticatorId";
    scheduleStateOrCrash(SessionState::GETTING_AUTHENTICATOR_ID);

    mWorker->schedule(Callable::from([this] {
        enterStateOrCrash(SessionState::GETTING_AUTHENTICATOR_ID);
        mCb->onAuthenticatorIdRetrieved(mOplusFp->getAuthenticatorId());
        enterIdling();
    }));

    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Session::invalidateAuthenticatorId() {
    LOG(INFO) << "invalidateAuthenticatorId";
    scheduleStateOrCrash(SessionState::INVALIDATING_AUTHENTICATOR_ID);

    mWorker->schedule(Callable::from([this] {
        enterStateOrCrash(SessionState::INVALIDATING_AUTHENTICATOR_ID);
        mCb->onAuthenticatorIdInvalidated(mOplusFp->getAuthenticatorId());
        enterIdling();
    }));

    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Session::resetLockout(const keymaster::HardwareAuthToken& hat) {
    LOG(INFO) << "resetLockout";
    scheduleStateOrCrash(SessionState::RESETTING_LOCKOUT);

    mWorker->schedule(Callable::from([this, hat] {
        enterStateOrCrash(SessionState::RESETTING_LOCKOUT);
        clearLockout(true);
        mIsLockoutTimerAborted = true;
        enterIdling();
    }));

    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Session::cancel() {
    LOG(INFO) << "cancel()";
    auto ret = mOplusFp->cancel();

    if (RequestStatusToInt(ret) == 0) {
        mCb->onError(Error::CANCELED, 0 /* vendorCode */);
        setFpPress(DISABLE);
        setDimlayerHbm(DISABLE);

        return ndk::ScopedAStatus::ok();
    } else {
        return ndk::ScopedAStatus::fromServiceSpecificError(RequestStatusToInt(ret));
    }
}

ndk::ScopedAStatus Session::close() {
    LOG(INFO) << "close";
    // TODO(b/166800618): call enterIdling from the terminal callbacks and restore this check.
    // CHECK(mCurrentState == SessionState::IDLING) << "Can't close a non-idling session.
    // Crashing.";

    // Cleanup UDFps HBM
    mCurrentState = SessionState::CLOSED;
    setFpPress(DISABLE);
    setDimlayerHbm(DISABLE);
    mCb->onSessionClosed();
    AIBinder_DeathRecipient_delete(mDeathRecipient);
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Session::onPointerDown(int32_t pointerId, int32_t x, int32_t y, float minor,
                                          float major) {
    LOG(INFO) << "onPointerDown";
    mWorker->schedule(Callable::from([this, pointerId, x, y, minor, major] {
        // FP Down
        setDimlayerHbm(ENABLE);
        setFpPress(ENABLE);
        checkSensorLockout();
        enterIdling();
    }));
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Session::onPointerUp(int32_t pointerId) {
    LOG(INFO) << "onPointerUp";
    mWorker->schedule(Callable::from([this, pointerId] {
        setFpPress(DISABLE);
        enterIdling();
    }));
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Session::onUiReady() {
    LOG(INFO) << "onUiReady";
    mWorker->schedule(Callable::from([this] {
        // onUiReady trigger on every press down. useless
        enterIdling();
    }));
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Session::authenticateWithContext(
        int64_t operationId, const common::OperationContext& /*context*/,
        std::shared_ptr<common::ICancellationSignal>* out) {
    return authenticate(operationId, out);
}

ndk::ScopedAStatus Session::enrollWithContext(const keymaster::HardwareAuthToken& hat,
                                              const common::OperationContext& /*context*/,
                                              std::shared_ptr<common::ICancellationSignal>* out) {
    return enroll(hat, out);
}

ndk::ScopedAStatus Session::detectInteractionWithContext(
        const common::OperationContext& /*context*/,
        std::shared_ptr<common::ICancellationSignal>* out) {
    return detectInteraction(out);
}

ndk::ScopedAStatus Session::onPointerDownWithContext(const PointerContext& context) {
    return onPointerDown(context.pointerId, context.x, context.y, context.minor, context.major);
}

ndk::ScopedAStatus Session::onPointerUpWithContext(const PointerContext& context) {
    return onPointerUp(context.pointerId);
}

ndk::ScopedAStatus Session::onContextChanged(const common::OperationContext& /*context*/) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Session::onPointerCancelWithContext(const PointerContext& /*context*/) {
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus Session::setIgnoreDisplayTouches(bool /*shouldIgnore*/) {
    return ndk::ScopedAStatus::ok();
}

// OPlus Hidl callbacks
Return<void> FingerprintCallback::onEnrollResult(uint64_t deviceId, uint32_t fingerId,
                                                   uint32_t groupId, uint32_t remaining) {
    LOG(INFO) << "onEnrollResult()";
#ifdef SEND_ACQUIRED_ON_DOWN_OR_UP
    mCb->onAcquired(AcquiredInfo::VENDOR, 0);
#endif
    if (mHelper)
        mHelper->touchUp();
    mCb->onEnrollmentProgress(fingerId, remaining);
    return Void();
}

Return<void> FingerprintCallback::onAcquired(uint64_t deviceId,
                                               V2_1::FingerprintAcquiredInfo acquiredInfo,
                                               int32_t vendorCode) {
    LOG(INFO) << "onAcquired()";
    auto info = HidlToAidlAcquiredInfo(acquiredInfo);
    mCb->onAcquired(info, vendorCode);
    return Void();
}

Return<void> FingerprintCallback::onAuthenticated(uint64_t deviceId, uint32_t fingerId,
                                                    uint32_t groupId,
                                                    const hidl_vec<uint8_t>& token) {
    setFpPress(DISABLE);
    LOG(INFO) << "onAuthenticated()";

    if (fingerId != 0) {
        setDimlayerHbm(DISABLE);
        hw_auth_token_t* hat = (hw_auth_token_t *) token.data();
        HardwareAuthToken authToken;
        translate(*hat, authToken);
        mCb->onAuthenticationSucceeded(fingerId, authToken);
        mLockoutTracker.reset(true);
    } else {
        mCb->onAuthenticationFailed();
        mLockoutTracker.addFailedAttempt();
        mSession->checkSensorLockout();
    }

    LOG(DEBUG) << "onAuthenticated: Send FP Touch Up";
#ifdef SEND_ACQUIRED_ON_DOWN_OR_UP
    mCb->onAcquired(AcquiredInfo::VENDOR, 0);
#endif
    if (mHelper)
        mHelper->touchUp();

    return Void();
}

Return<void> FingerprintCallback::onError(uint64_t deviceId, FingerprintError error,
                                            int32_t vendorCode) {
    setDimlayerHbm(DISABLE);
    setFpPress(DISABLE);

    LOG(INFO) << "onError()";

    if (error == android::hardware::biometrics::fingerprint::V2_1::FingerprintError::ERROR_NO_ERROR) {
        return Void();
    }
    int32_t convertVCode = -1;
    mCb->onError(HidlToAidlFingerprintError(error, &convertVCode), 
                        convertVCode != -1 ? convertVCode : vendorCode);
    return Void();
}

Return<void> FingerprintCallback::onRemoved(uint64_t deviceId, uint32_t fingerId,
                                              uint32_t groupId, uint32_t remaining) {
    LOG(INFO) << "onRemoved()";
    std::vector<int> enrollments;
    enrollments.push_back(fingerId);
    mCb->onEnrollmentsRemoved(enrollments);
    return Void();
}

Return<void> FingerprintCallback::onEnumerate(uint64_t deviceId, uint32_t fingerId,
                                                uint32_t groupId, uint32_t remaining) {
    LOG(INFO) << "onEnumerate()";
    static std::vector<int> enrollments;
    enrollments.push_back(fingerId);
    if (remaining == 0) {
        mCb->onEnrollmentsEnumerated(enrollments);
        enrollments.clear();
    }
    return Void();
}

Return<void> FingerprintCallback::onAcquired_2_2(uint64_t deviceId,
                                                   FingerprintAcquiredInfo acquiredInfo,
                                                   int32_t vendorCode) {
    LOG(INFO) << "onAcquired_2_2()";
    auto info = HidlToAidlAcquiredInfo(static_cast<V2_1::FingerprintAcquiredInfo>(acquiredInfo));
    mCb->onAcquired(info, vendorCode);
    return Void();
}

Return<void> FingerprintCallback::onEngineeringInfoUpdated(
    uint32_t /*lenth*/, const hidl_vec<uint32_t>& /*keys*/,
    const hidl_vec<hidl_string>& /*values*/) {
    return Void();
}

Return<void> FingerprintCallback::onFingerprintCmd(int32_t cmdId,
                                                     const hidl_vec<int8_t>& result,
                                                     uint32_t resultLen) {
    switch (cmdId) {
        case FINGERPRINT_CALLBACK_CMD_ID_ON_TOUCH_DOWN:
            LOG(DEBUG) << "onFingerprintCmd: FP Touch Down Detected!";
            if (mHelper)
                mHelper->touchDown();
#ifdef SEND_ACQUIRED_ON_DOWN_OR_UP
            mCb->onAcquired(AcquiredInfo::VENDOR, 1);
#endif
            break;
        case FINGERPRINT_CALLBACK_CMD_ID_ON_TOUCH_UP:
            LOG(DEBUG) << "onFingerprintCmd: FP Touch Up Detected!";
            if (mHelper)
                mHelper->touchUp();
#ifdef SEND_ACQUIRED_ON_DOWN_OR_UP
            mCb->onAcquired(AcquiredInfo::VENDOR, 0);
#endif
            break;
    }
    return Void();
}

}  // namespace aidl::android::hardware::biometrics::fingerprint
